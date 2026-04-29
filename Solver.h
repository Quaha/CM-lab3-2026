// Solver.h — численное ядро (чистый нативный C++).
// Вариант 1, метод баланса (Тихонов–Самарский) + прогонка.
#pragma once

#include <vector>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <algorithm>

namespace Numerics {

enum class BCType { Dirichlet = 0, Neumann = 1, Robin = 2 };

struct BoundaryCondition {
    BCType type = BCType::Dirichlet;
    double sigma = 0.0; // для Робина
    double gamma = 0.0;
};

// Кусочно-определённая функция со скачком в xi и аналитической первообразной.
struct Piecewise {
    double xi = 0.0;
    std::function<double(double)> g1, g2; // значение слева/справа от xi
    std::function<double(double)> G1, G2; // соответствующие первообразные

    double value(double x) const { return (x < xi) ? g1(x) : g2(x); }

    // Интеграл от a до b. Если интервал пересекает xi — разбивается.
    double integral(double a, double b) const {
        if (b <= xi) return G1(b) - G1(a);
        if (a >= xi) return G2(b) - G2(a);
        return (G1(xi) - G1(a)) + (G2(b) - G2(xi));
    }
};

struct ProblemData {
    double xi = 0.0;
    Piecewise k, q, f, invK;
};

// =====================  ВАРИАНТ 1  =====================
// Основная задача: k1=x+1, k2=x;  q1=x, q2=x^2;  f1=x, f2=exp(-x);  xi=0.4
inline ProblemData variant1_main() {
    const double xi = 0.4;
    ProblemData p; p.xi = xi;
    p.k.xi = p.q.xi = p.f.xi = p.invK.xi = xi;

    p.k.g1 = [](double x){ return x + 1.0; };
    p.k.g2 = [](double x){ return x; };
    p.k.G1 = [](double x){ return 0.5*x*x + x; };
    p.k.G2 = [](double x){ return 0.5*x*x; };

    p.invK.g1 = [](double x){ return 1.0/(x+1.0); };
    p.invK.g2 = [](double x){ return 1.0/x; };
    p.invK.G1 = [](double x){ return std::log(x+1.0); };
    p.invK.G2 = [](double x){ return std::log(x); };

    p.q.g1 = [](double x){ return x; };
    p.q.g2 = [](double x){ return x*x; };
    p.q.G1 = [](double x){ return 0.5*x*x; };
    p.q.G2 = [](double x){ return x*x*x/3.0; };

    p.f.g1 = [](double x){ return x; };
    p.f.g2 = [](double x){ return std::exp(-x); };
    p.f.G1 = [](double x){ return 0.5*x*x; };
    p.f.G2 = [](double x){ return -std::exp(-x); };

    return p;
}

// Тестовая задача: коэффициенты заморожены к пределам в xi
//   k1*=1.4, k2*=0.4;  q1*=0.4, q2*=0.16;  f1*=0.4, f2*=exp(-0.4)
inline ProblemData variant1_test() {
    const double xi = 0.4;
    const double k1s = xi + 1.0;     // 1.4
    const double k2s = xi;           // 0.4
    const double q1s = xi;           // 0.4
    const double q2s = xi*xi;        // 0.16
    const double f1s = xi;           // 0.4
    const double f2s = std::exp(-xi);

    ProblemData p; p.xi = xi;
    p.k.xi = p.q.xi = p.f.xi = p.invK.xi = xi;

    p.k.g1 = [k1s](double){ return k1s; };
    p.k.g2 = [k2s](double){ return k2s; };
    p.k.G1 = [k1s](double x){ return k1s*x; };
    p.k.G2 = [k2s](double x){ return k2s*x; };

    p.invK.g1 = [k1s](double){ return 1.0/k1s; };
    p.invK.g2 = [k2s](double){ return 1.0/k2s; };
    p.invK.G1 = [k1s](double x){ return x/k1s; };
    p.invK.G2 = [k2s](double x){ return x/k2s; };

    p.q.g1 = [q1s](double){ return q1s; };
    p.q.g2 = [q2s](double){ return q2s; };
    p.q.G1 = [q1s](double x){ return q1s*x; };
    p.q.G2 = [q2s](double x){ return q2s*x; };

    p.f.g1 = [f1s](double){ return f1s; };
    p.f.g2 = [f2s](double){ return f2s; };
    p.f.G1 = [f1s](double x){ return f1s*x; };
    p.f.G2 = [f2s](double x){ return f2s*x; };

    return p;
}

// =====================  АНАЛИТИЧЕСКОЕ РЕШЕНИЕ ТЕСТОВОЙ ЗАДАЧИ  =====================
// На каждом куске уравнение линейное с постоянными коэффициентами:
//   -k* u'' + q* u = f*  =>  u(x) = A cosh(λ(x-x0)) + B sinh(λ(x-x0)) + p
//   λ = sqrt(q*/k*),  p = f*/q*
// j=1: x0=0, j=2: x0=1.  Неизвестные A1,B1,A2,B2 определяются 4 условиями.

struct AnalyticalSolution {
    double xi = 0.4;
    double lam1 = 0, lam2 = 0;
    double A1 = 0, B1 = 0, p1 = 0;
    double A2 = 0, B2 = 0, p2 = 0;

    double operator()(double x) const {
        if (x < xi) return A1 * std::cosh(lam1 * x) + B1 * std::sinh(lam1 * x) + p1;
        return A2 * std::cosh(lam2 * (x - 1.0)) + B2 * std::sinh(lam2 * (x - 1.0)) + p2;
    }
};

inline std::vector<double> solve4x4(double M[4][4], double b[4]) {
    double A[4][5];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) A[i][j] = M[i][j];
        A[i][4] = b[i];
    }
    for (int k = 0; k < 4; ++k) {
        int piv = k;
        for (int i = k+1; i < 4; ++i)
            if (std::fabs(A[i][k]) > std::fabs(A[piv][k])) piv = i;
        if (piv != k)
            for (int j = 0; j < 5; ++j) std::swap(A[k][j], A[piv][j]);
        if (std::fabs(A[k][k]) < 1e-300)
            throw std::runtime_error("Вырожденная матрица в аналитическом решении");
        for (int i = k+1; i < 4; ++i) {
            double r = A[i][k] / A[k][k];
            for (int j = k; j < 5; ++j) A[i][j] -= r * A[k][j];
        }
    }
    std::vector<double> x(4);
    for (int i = 3; i >= 0; --i) {
        double s = A[i][4];
        for (int j = i+1; j < 4; ++j) s -= A[i][j] * x[j];
        x[i] = s / A[i][i];
    }
    return x;
}

inline AnalyticalSolution analytical_test_solution(
    const BoundaryCondition& bcL, const BoundaryCondition& bcR)
{
    const double xi = 0.4;
    const double k1s = 1.4, k2s = 0.4;
    const double q1s = 0.4, q2s = 0.16;
    const double f1s = 0.4, f2s = std::exp(-0.4);

    AnalyticalSolution sol;
    sol.xi = xi;
    sol.lam1 = std::sqrt(q1s / k1s);
    sol.lam2 = std::sqrt(q2s / k2s);
    sol.p1 = f1s / q1s;
    sol.p2 = f2s / q2s;

    double M[4][4] = {{0}};
    double b[4] = {0};

    // Стр. 0: левое ГУ при x=0.  u(0) = A1 + p1,  u'(0) = lam1*B1.
    if (bcL.type == BCType::Dirichlet) {
        // u(0) = γ_L  =>  A1 = γ_L − p1
        M[0][0] = 1.0;
        b[0] = bcL.gamma - sol.p1;
    } else {
        // -k1* u'(0) + σ u(0) = γ  =>  σ A1 − k1* λ1 B1 = γ − σ p1
        double sg = (bcL.type == BCType::Robin) ? bcL.sigma : 0.0;
        M[0][0] = sg;
        M[0][1] = -k1s * sol.lam1;
        b[0] = bcL.gamma - sg * sol.p1;
    }

    // Стр. 1: правое ГУ при x=1.  u(1) = A2 + p2,  u'(1) = lam2*B2.
    if (bcR.type == BCType::Dirichlet) {
        M[1][2] = 1.0;
        b[1] = bcR.gamma - sol.p2;
    } else {
        // k2* u'(1) + σ u(1) = γ  =>  σ A2 + k2* λ2 B2 = γ − σ p2
        double sg = (bcR.type == BCType::Robin) ? bcR.sigma : 0.0;
        M[1][2] = sg;
        M[1][3] = k2s * sol.lam2;
        b[1] = bcR.gamma - sg * sol.p2;
    }

    // Стр. 2: непрерывность u в xi:  u1(xi) = u2(xi)
    double c1 = std::cosh(sol.lam1 * xi);
    double s1 = std::sinh(sol.lam1 * xi);
    double c2 = std::cosh(sol.lam2 * (xi - 1.0));
    double s2 = std::sinh(sol.lam2 * (xi - 1.0));
    M[2][0] = c1; M[2][1] = s1; M[2][2] = -c2; M[2][3] = -s2;
    b[2] = sol.p2 - sol.p1;

    // Стр. 3: непрерывность потока:  k1* u1'(xi) = k2* u2'(xi)
    M[3][0] =  k1s * sol.lam1 * s1;
    M[3][1] =  k1s * sol.lam1 * c1;
    M[3][2] = -k2s * sol.lam2 * s2;
    M[3][3] = -k2s * sol.lam2 * c2;
    b[3] = 0.0;

    auto x = solve4x4(M, b);
    sol.A1 = x[0]; sol.B1 = x[1]; sol.A2 = x[2]; sol.B2 = x[3];
    return sol;
}

// =====================  ПРОГОНКА  =====================
// Система: A[i] v[i-1] − C[i] v[i] + B[i] v[i+1] = −F[i]
//          A[0] = 0,  B[N-1] = 0
inline std::vector<double> thomas(
    const std::vector<double>& A,
    const std::vector<double>& C,
    const std::vector<double>& B,
    const std::vector<double>& F)
{
    int N = (int)A.size();
    std::vector<double> alpha(N), beta(N), v(N);
    // Прямой ход: v[i] = alpha[i]*v[i+1] + beta[i]
    alpha[0] = B[0] / C[0];
    beta[0]  = F[0] / C[0];
    for (int i = 1; i < N - 1; ++i) {
        double denom = C[i] - A[i] * alpha[i-1];
        alpha[i] = B[i] / denom;
        beta[i]  = (F[i] + A[i] * beta[i-1]) / denom;
    }
    int last = N - 1;
    v[last] = (F[last] + A[last] * beta[last-1]) / (C[last] - A[last] * alpha[last-1]);
    for (int i = last - 1; i >= 0; --i)
        v[i] = alpha[i] * v[i+1] + beta[i];
    return v;
}

// =====================  МЕТОД БАЛАНСА  =====================
// Внутренние узлы:  A_i = a_i/h^2,  B_i = a_{i+1}/h^2,  C_i = A_i + B_i + d_i,  F_i = phi_i,
//   где a_i = h / ∫_{x_{i-1}}^{x_i} dx/k(x) — гармоническое осреднение по полуцелым,
//       d_i = (1/h) ∫ q dx,  phi_i = (1/h) ∫ f dx  по [x_{i-1/2}, x_{i+1/2}].
// Граничные узлы: интеграл по полуячейке [0, h/2] либо [1−h/2, 1].
inline std::vector<double> solve_bvp(
    const ProblemData& p, int n,
    const BoundaryCondition& bcL, const BoundaryCondition& bcR)
{
    const double h = 1.0 / n;
    const int N = n + 1;
    std::vector<double> A(N, 0.0), C(N, 0.0), B(N, 0.0), F(N, 0.0);

    // a[i] для i=1..n: гармоническое осреднение по [x_{i-1}, x_i]
    std::vector<double> a(n + 2, 0.0);
    for (int i = 1; i <= n; ++i) {
        double xL = (i - 1) * h, xR = i * h;
        double intInvK = p.invK.integral(xL, xR);
        a[i] = h / intInvK;
    }

    // Внутренние узлы
    for (int i = 1; i <= n - 1; ++i) {
        double xL = (i - 0.5) * h, xR = (i + 0.5) * h;
        double d_i   = p.q.integral(xL, xR) / h;
        double phi_i = p.f.integral(xL, xR) / h;
        A[i] = a[i]   / (h*h);
        B[i] = a[i+1] / (h*h);
        C[i] = A[i] + B[i] + d_i;
        F[i] = phi_i;
    }

    // Левое граничное условие
    if (bcL.type == BCType::Dirichlet) {
        A[0] = 0; B[0] = 0; C[0] = 1.0; F[0] = bcL.gamma;
    } else {
        // Интегрирование уравнения по [0, h/2]:
        //   (a1/h)(v1 − v0) − σ v0 + γ − dT v0 + φT = 0
        // приводится к   −C0 v0 + B0 v1 = −F0  с
        //   C0 = a1/h + σ + dT,   B0 = a1/h,   F0 = γ + φT
        double dT = p.q.integral(0.0, h/2.0);
        double fT = p.f.integral(0.0, h/2.0);
        double sg = (bcL.type == BCType::Robin) ? bcL.sigma : 0.0;
        A[0] = 0;
        B[0] = a[1] / h;
        C[0] = a[1] / h + sg + dT;
        F[0] = bcL.gamma + fT;
    }

    // Правое граничное условие
    if (bcR.type == BCType::Dirichlet) {
        A[n] = 0; B[n] = 0; C[n] = 1.0; F[n] = bcR.gamma;
    } else {
        double dT = p.q.integral(1.0 - h/2.0, 1.0);
        double fT = p.f.integral(1.0 - h/2.0, 1.0);
        double sg = (bcR.type == BCType::Robin) ? bcR.sigma : 0.0;
        A[n] = a[n] / h;
        B[n] = 0;
        C[n] = a[n] / h + sg + dT;
        F[n] = bcR.gamma + fT;
    }

    return thomas(A, C, B, F);
}

} // namespace Numerics
