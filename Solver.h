// Solver.h — Лабораторная 3. Метод баланса. Вариант 7.
//
// Стационарное уравнение теплопроводности с разрывными коэффициентами:
//     d/dx( k(x) du/dx ) − q(x) u(x) = − f(x),   x ∈ (0, 1)
//
// Точка разрыва коэффициентов:  ξ = π / 4.
// Коэффициенты варианта 7:
//     k₁(x) = √2·cos(x),    k₂(x) = 2
//     q₁(x) = x,            q₂(x) = x²
//     f₁(x) = sin(2x),      f₂(x) = sin(x)
//
// Граничные условия:
//     1-я краевая задача:  u(0) = μ₁ = 1,  u(1) = μ₂ = 1.
//     3-я краевая задача (теплообмен по Ньютону):
//         w(0) = − γ₁ ( u(0) − θ₁ ),  γ₁ = 4,  θ₁ = 2;
//         w(1) = − γ₂ ( θ₂ − u(1) ),  γ₂ = 3,  θ₂ = 1.
//         Здесь w(x) = − k(x) u'(x) — функция теплового потока.
//
// Тестовая задача получается заморозкой коэффициентов основной задачи
// на пределы в точке разрыва ξ:
//     k₁* = √2·cos(π/4) = 1,        k₂* = 2
//     q₁* = π/4,                     q₂* = (π/4)²
//     f₁* = sin(π/2) = 1,           f₂* = sin(π/4) = √2/2
//
// Реализованы две схемы аппроксимации граничных условий
// (применимы только к 3-й краевой задаче):
//     Классическая  — односторонняя разностная аппроксимация u'(0), u'(1);
//                     порядок аппроксимации ГУ = 1, схема сходится с
//                     порядком 1 (Модуль 7, п. 7.3).
//     Улучшенная    — интегрирование уравнения по полуячейке [0, h/2]
//                     (соотв. [1−h/2, 1]) методом баланса; порядок
//                     аппроксимации ГУ = 2, схема сходится со 2-м
//                     порядком (Модуль 7, п. 7.4).

#pragma once

#include "Progonka.h"
#include <vector>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <algorithm>

namespace BalanceSolver {

    // ===================== Параметры варианта 7 =====================

    constexpr double XI = 0.7853981633974483; // π/4
    constexpr double MU1 = 1.0;
    constexpr double MU2 = 1.0;
    constexpr double GAMMA1 = 4.0;
    constexpr double GAMMA2 = 3.0;
    constexpr double THETA1 = 2.0;
    constexpr double THETA2 = 1.0;

    // Коэффициенты тестовой задачи (пределы коэффициентов основной в ξ).
    constexpr double K1S = 1.0;                       // √2·cos(π/4)
    constexpr double K2S = 2.0;
    constexpr double Q1S = 0.7853981633974483;        // π/4
    constexpr double Q2S = 0.6168502750680849;        // (π/4)²
    constexpr double F1S = 1.0;                       // sin(π/2)
    constexpr double F2S = 0.7071067811865476;        // sin(π/4) = √2/2

    // ===================== Перечисления режима задачи =====================

    // Тип краевой задачи.
    enum class ProblemKind {
        First = 0, // 1-я краевая (заданы u(0), u(1))
        Third = 1  // 3-я краевая (теплообмен на обоих концах)
    };

    // Тип задачи (тестовая / основная).
    enum class ProblemType {
        Test = 0,
        Main = 1
    };

    // Схема аппроксимации граничных условий (актуально для 3-й краевой).
    enum class BCScheme {
        Classical = 0, // односторонняя разностная аппроксимация
        Improved = 1  // интегрирование по полуячейке (метод баланса)
    };

    // ===================== Кусочные функции и данные задачи =====================

    // Кусочно-определённая функция со скачком в xi и аналитической
    // первообразной на каждом куске. Интегралы по отрезку, пересекающему ξ,
    // разбиваются на сумму двух кусков — это позволяет считать интегралы
    // разностной схемы точно (а не численно).
    //
    // Опционально можно задать I1/I2 — определённый интеграл по куску ∫_a^b,
    // вычисленный численно устойчивым способом (без вычитания близких больших
    // чисел). Если I1/I2 заданы, integral() использует их; иначе — обычную
    // разность первообразных G(b) − G(a).
    struct Piecewise {
        double xi = XI;
        std::function<double(double)> g1, g2; // значение слева / справа от xi
        std::function<double(double)> G1, G2; // соответствующие первообразные
        std::function<double(double, double)> I1, I2; // ∫_a^b на куске (опц., устойчиво)

        double value(double x) const { return (x < xi) ? g1(x) : g2(x); }

        // Определённый интеграл по одному куску (слева/справа от xi).
        double part1(double a, double b) const { return I1 ? I1(a, b) : (G1(b) - G1(a)); }
        double part2(double a, double b) const { return I2 ? I2(a, b) : (G2(b) - G2(a)); }

        double integral(double a, double b) const {
            if (b <= xi) return part1(a, b);
            if (a >= xi) return part2(a, b);
            return part1(a, xi) + part2(xi, b);
        }
    };

    struct ProblemData {
        double xi = XI;
        Piecewise k, q, f, invK; // invK ≡ 1 / k(x)
    };

    // ===================== ВАРИАНТ 7: ОСНОВНАЯ ЗАДАЧА =====================
    // Все первообразные взяты аналитически — это исключает квадратурный шум.
    //   ∫ √2·cos(x) dx = √2·sin(x)
    //   ∫ 2 dx        = 2x
    //   ∫ dx/(√2·cos x) = (1/√2)·ln|sec x + tan x|
    //   ∫ dx/2          = x/2
    //   ∫ x dx          = x²/2
    //   ∫ x² dx         = x³/3
    //   ∫ sin(2x) dx    = − cos(2x)/2
    //   ∫ sin(x) dx     = − cos(x)

    inline ProblemData variant7_main() {
        ProblemData p;
        p.xi = p.k.xi = p.q.xi = p.f.xi = p.invK.xi = XI;

        p.k.g1 = [](double x) { return std::sqrt(2.0) * std::cos(x); };
        p.k.g2 = [](double) { return 2.0; };
        p.k.G1 = [](double x) { return std::sqrt(2.0) * std::sin(x); };
        p.k.G2 = [](double x) { return 2.0 * x; };

        p.invK.g1 = [](double x) { return 1.0 / (std::sqrt(2.0) * std::cos(x)); };
        p.invK.g2 = [](double) { return 0.5; };
        p.invK.G1 = [](double x) {
            // ∫ dx/(√2·cos x) = (1/√2)·ln|sec x + tan x|.
            double s = 1.0 / std::cos(x) + std::tan(x);
            return (1.0 / std::sqrt(2.0)) * std::log(std::fabs(s));
            };
        // Численно устойчивый определённый интеграл ∫_a^b dx/(√2·cos x):
        // вместо разности логарифмов G(b)−G(a) = (1/√2)·ln(s_b/s_a) используем
        // log1p, чтобы избежать сокращения близких больших логарифмов на мелком h.
        //   ln(s_b/s_a) = ln(1 + (s_b − s_a)/s_a) = log1p((s_b − s_a)/s_a).
        p.invK.I1 = [](double a, double b) {
            double sa = 1.0 / std::cos(a) + std::tan(a);
            double sb = 1.0 / std::cos(b) + std::tan(b);
            return (1.0 / std::sqrt(2.0)) * std::log1p((sb - sa) / sa);
            };
        p.invK.G2 = [](double x) { return 0.5 * x; };

        p.q.g1 = [](double x) { return x; };
        p.q.g2 = [](double x) { return x * x; };
        p.q.G1 = [](double x) { return 0.5 * x * x; };
        p.q.G2 = [](double x) { return x * x * x / 3.0; };

        p.f.g1 = [](double x) { return std::sin(2.0 * x); };
        p.f.g2 = [](double x) { return std::sin(x); };
        p.f.G1 = [](double x) { return -0.5 * std::cos(2.0 * x); };
        p.f.G2 = [](double x) { return -std::cos(x); };

        return p;
    }

    // ===================== ВАРИАНТ 7: ТЕСТОВАЯ ЗАДАЧА =====================
    // Коэффициенты — кусочно-постоянные, равные пределам в точке ξ.

    inline ProblemData variant7_test() {
        ProblemData p;
        p.xi = p.k.xi = p.q.xi = p.f.xi = p.invK.xi = XI;

        p.k.g1 = [](double) { return K1S; };
        p.k.g2 = [](double) { return K2S; };
        p.k.G1 = [](double x) { return K1S * x; };
        p.k.G2 = [](double x) { return K2S * x; };

        p.invK.g1 = [](double) { return 1.0 / K1S; };
        p.invK.g2 = [](double) { return 1.0 / K2S; };
        p.invK.G1 = [](double x) { return x / K1S; };
        p.invK.G2 = [](double x) { return x / K2S; };

        p.q.g1 = [](double) { return Q1S; };
        p.q.g2 = [](double) { return Q2S; };
        p.q.G1 = [](double x) { return Q1S * x; };
        p.q.G2 = [](double x) { return Q2S * x; };

        p.f.g1 = [](double) { return F1S; };
        p.f.g2 = [](double) { return F2S; };
        p.f.G1 = [](double x) { return F1S * x; };
        p.f.G2 = [](double x) { return F2S * x; };

        return p;
    }

    // ===================== АНАЛИТИЧЕСКОЕ РЕШЕНИЕ ТЕСТОВОЙ ЗАДАЧИ =====================
    // На каждом куске коэффициенты постоянны, уравнение принимает вид
    //     − k* u''(x) + q* u(x) = f*    ⇒    u'' = (q*/k*) u − f*/k*.
    // Общее решение:
    //     u(x) = A·cosh(λ·(x − x₀)) + B·sinh(λ·(x − x₀)) + p,
    // где λ = √(q*/k*), p = f*/q* (частное решение).
    // Берём x₀ = 0 на левом куске, x₀ = 1 на правом — это улучшает
    // обусловленность системы 4×4 на коэффициенты A₁,B₁,A₂,B₂.
    //
    // Четыре константы определяются:
    //   – левым ГУ при x = 0,
    //   – непрерывностью u в точке ξ,
    //   – непрерывностью теплового потока w = − k·u' в точке ξ,
    //   – правым ГУ при x = 1.

    struct AnalyticalSolution {
        double xi = XI;
        double lam1 = 0, lam2 = 0;
        double p1 = 0, p2 = 0;
        double A1 = 0, B1 = 0;
        double A2 = 0, B2 = 0;

        double operator()(double x) const {
            if (x < xi) return A1 * std::cosh(lam1 * x) + B1 * std::sinh(lam1 * x) + p1;
            return         A2 * std::cosh(lam2 * (x - 1.0)) + B2 * std::sinh(lam2 * (x - 1.0)) + p2;
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
            for (int i = k + 1; i < 4; ++i)
                if (std::fabs(A[i][k]) > std::fabs(A[piv][k])) piv = i;
            if (piv != k)
                for (int j = 0; j < 5; ++j) std::swap(A[k][j], A[piv][j]);
            if (std::fabs(A[k][k]) < 1e-300)
                throw std::runtime_error("Вырожденная матрица в аналитическом решении");
            for (int i = k + 1; i < 4; ++i) {
                double r = A[i][k] / A[k][k];
                for (int j = k; j < 5; ++j) A[i][j] -= r * A[k][j];
            }
        }
        std::vector<double> x(4);
        for (int i = 3; i >= 0; --i) {
            double s = A[i][4];
            for (int j = i + 1; j < 4; ++j) s -= A[i][j] * x[j];
            x[i] = s / A[i][i];
        }
        return x;
    }

    inline AnalyticalSolution analytical_test_solution(ProblemKind kind) {
        AnalyticalSolution sol;
        sol.xi = XI;
        sol.lam1 = std::sqrt(Q1S / K1S);
        sol.lam2 = std::sqrt(Q2S / K2S);
        sol.p1 = F1S / Q1S;
        sol.p2 = F2S / Q2S;

        double M[4][4] = { { 0 } };
        double b[4] = { 0 };

        // ---- Строка 0: левое ГУ при x = 0 ----
        // u(0)  = A1 + p1,       u'(0) = lam1·B1
        if (kind == ProblemKind::First) {
            // 1-я краевая: u(0) = μ₁  ⇒  A1 = μ₁ − p1
            M[0][0] = 1.0;
            b[0] = MU1 - sol.p1;
        }
        else {
            // 3-я краевая: w(0) = − γ₁(u(0) − θ₁), w = − k·u'
            //   ⇒  − k₁*·u'(0) = − γ₁(u(0) − θ₁)
            //   ⇒  γ₁·u(0) − k₁*·u'(0) = γ₁·θ₁
            //   ⇒  γ₁·A1 − k₁*·lam1·B1 = γ₁·(θ₁ − p1)
            M[0][0] = GAMMA1;
            M[0][1] = -K1S * sol.lam1;
            b[0] = GAMMA1 * (THETA1 - sol.p1);
        }

        // ---- Строка 1: правое ГУ при x = 1 ----
        // u(1)  = A2 + p2,       u'(1) = lam2·B2
        if (kind == ProblemKind::First) {
            M[1][2] = 1.0;
            b[1] = MU2 - sol.p2;
        }
        else {
            // 3-я краевая: w(1) = − γ₂(θ₂ − u(1)) = γ₂·(u(1) − θ₂)
            //   ⇒  − k₂*·u'(1) = γ₂·u(1) − γ₂·θ₂
            //   ⇒  γ₂·u(1) + k₂*·u'(1) = γ₂·θ₂
            //   ⇒  γ₂·A2 + k₂*·lam2·B2 = γ₂·(θ₂ − p2)
            M[1][2] = GAMMA2;
            M[1][3] = K2S * sol.lam2;
            b[1] = GAMMA2 * (THETA2 - sol.p2);
        }

        // ---- Строка 2: непрерывность температуры в ξ ----
        double c1 = std::cosh(sol.lam1 * sol.xi);
        double s1 = std::sinh(sol.lam1 * sol.xi);
        double c2 = std::cosh(sol.lam2 * (sol.xi - 1.0));
        double s2 = std::sinh(sol.lam2 * (sol.xi - 1.0));
        M[2][0] = c1; M[2][1] = s1;
        M[2][2] = -c2; M[2][3] = -s2;
        b[2] = sol.p2 - sol.p1;

        // ---- Строка 3: непрерывность теплового потока в ξ ----
        // w(x) = − k(x)·u'(x).  Условие w₁(ξ) = w₂(ξ) ⇔ k₁*·u₁'(ξ) = k₂*·u₂'(ξ).
        //   u₁'(ξ) = lam1·(A1·sinh + B1·cosh)
        //   u₂'(ξ) = lam2·(A2·sinh(lam2·(ξ−1)) + B2·cosh(lam2·(ξ−1)))
        M[3][0] = K1S * sol.lam1 * s1;
        M[3][1] = K1S * sol.lam1 * c1;
        M[3][2] = -K2S * sol.lam2 * s2;
        M[3][3] = -K2S * sol.lam2 * c2;
        b[3] = 0.0;

        auto x = solve4x4(M, b);
        sol.A1 = x[0]; sol.B1 = x[1]; sol.A2 = x[2]; sol.B2 = x[3];
        return sol;
    }

    // ===================== МЕТОД БАЛАНСА (РАЗНОСТНАЯ СХЕМА) =====================
    // Сетка размерности n: xᵢ = i·h, i = 0..n, h = 1/n.
    // Вспомогательная сетка: xᵢ₊₀.₅ = (i + 0.5)·h, i = 0..n−1.
    //
    // Внутренние узлы (одинаково для обеих схем):
    //     − (aᵢ/h²) · vᵢ₋₁ + ((aᵢ + aᵢ₊₁)/h² + dᵢ) · vᵢ − (aᵢ₊₁/h²) · vᵢ₊₁ = φᵢ,
    // где
    //     aᵢ  = h / ∫_{xᵢ₋₁}^{xᵢ} dx / k(x)                 (i = 1..n)
    //     dᵢ  = (1/h) · ∫_{xᵢ₋₀.₅}^{xᵢ₊₀.₅} q(x) dx          (i = 1..n−1)
    //     φᵢ  = (1/h) · ∫_{xᵢ₋₀.₅}^{xᵢ₊₀.₅} f(x) dx          (i = 1..n−1)
    //
    // aᵢ — гармоническое осреднение коэффициента теплопроводности по полуцелым.
    // Все три интеграла берутся в замкнутом виде через первообразные,
    // заданные в Piecewise; в окрестности точки разрыва ξ интеграл
    // автоматически разбивается на два куска.
    //
    // Граничные узлы:
    //     1-я краевая:  v₀ = μ₁, vₙ = μ₂ (точно).
    //     3-я краевая, классическая аппроксимация (Модуль 7, п. 7.3):
    //         Левое: u'(0) ≈ (v₁ − v₀) / h.  Подставив в w(0) + γ₁·v₀ = γ₁·θ₁,
    //         где w(0) = − k(0)·u'(0), и приведя к канонической форме, получим
    //             (k(0)/h + γ₁)·v₀ − (k(0)/h)·v₁ = γ₁·θ₁.
    //         Правое: u'(1) ≈ (vₙ − vₙ₋₁) / h.  Из k(1)·u'(1) + γ₂·vₙ = γ₂·θ₂
    //             (k(1)/h + γ₂)·vₙ − (k(1)/h)·vₙ₋₁ = γ₂·θ₂.
    //     3-я краевая, улучшенная аппроксимация (Модуль 7, п. 7.4):
    //         Уравнение интегрируется по полуячейке [0, h/2] (соответственно
    //         [1−h/2, 1]). Поток в полуцелом узле выражается через a₁ (соотв. aₙ),
    //         поток на границе берётся из ГУ. Получается схема со 2-м порядком:
    //             (a₁/h + γ₁ + d̃₀)·v₀ − (a₁/h)·v₁ = γ₁·θ₁ + φ̃₀,
    //         где d̃₀ = ∫₀^{h/2} q(x) dx, φ̃₀ = ∫₀^{h/2} f(x) dx.
    //
    // Получаемая СЛАУ — трёхдиагональная; решается методом прогонки в
    // форме Самарского, реализованной в Progonka.h:
    //     A[i] · v[i−1] − C[i] · v[i] + B[i] · v[i+1] = − F[i].

    inline std::vector<double> solve_bvp(const ProblemData& p, int n,
        ProblemKind kind, BCScheme scheme)
    {
        if (n < 2) throw std::runtime_error("Размерность сетки n должна быть ≥ 2");

        const double h = 1.0 / n;
        const int    N = n + 1;

        std::vector<double> A(N, 0.0), C(N, 0.0), B(N, 0.0), F(N, 0.0);

        // Гармонические коэффициенты aᵢ на участках [xᵢ₋₁, xᵢ], i = 1..n.
        std::vector<double> a(n + 2, 0.0);
        for (int i = 1; i <= n; ++i) {
            double xL = (i - 1) * h;
            double xR = i * h;
            double intInvK = p.invK.integral(xL, xR);
            a[i] = h / intInvK;
        }

        // ---- Внутренние узлы i = 1..n−1 ----
        for (int i = 1; i <= n - 1; ++i) {
            double xL = (i - 0.5) * h;
            double xR = (i + 0.5) * h;
            double d_i = p.q.integral(xL, xR) / h;
            double phi_i = p.f.integral(xL, xR) / h;

            A[i] = a[i] / (h * h);
            B[i] = a[i + 1] / (h * h);
            C[i] = A[i] + B[i] + d_i;
            F[i] = phi_i;
        }

        // ---- Левое граничное условие ----
        if (kind == ProblemKind::First) {
            // v₀ = μ₁
            A[0] = 0.0;  C[0] = 1.0;  B[0] = 0.0;  F[0] = MU1;
        }
        else if (scheme == BCScheme::Classical) {
            // Классическая аппроксимация (порядок 1).
            // (k(0)/h + γ₁)·v₀ − (k(0)/h)·v₁ = γ₁·θ₁
            double k0 = p.k.value(0.0);
            A[0] = 0.0;
            C[0] = k0 / h + GAMMA1;
            B[0] = k0 / h;
            F[0] = GAMMA1 * THETA1;
        }
        else {
            // Улучшенная аппроксимация (метод баланса, порядок 2).
            // (a₁/h + γ₁ + d̃₀)·v₀ − (a₁/h)·v₁ = γ₁·θ₁ + φ̃₀
            double dTilde = p.q.integral(0.0, h / 2.0); // ∫ q dx
            double phiTilde = p.f.integral(0.0, h / 2.0); // ∫ f dx
            A[0] = 0.0;
            C[0] = a[1] / h + GAMMA1 + dTilde;
            B[0] = a[1] / h;
            F[0] = GAMMA1 * THETA1 + phiTilde;
        }

        // ---- Правое граничное условие ----
        if (kind == ProblemKind::First) {
            A[n] = 0.0;  C[n] = 1.0;  B[n] = 0.0;  F[n] = MU2;
        }
        else if (scheme == BCScheme::Classical) {
            // (k(1)/h + γ₂)·vₙ − (k(1)/h)·vₙ₋₁ = γ₂·θ₂
            double k1 = p.k.value(1.0);
            A[n] = k1 / h;
            C[n] = k1 / h + GAMMA2;
            B[n] = 0.0;
            F[n] = GAMMA2 * THETA2;
        }
        else {
            double dTilde = p.q.integral(1.0 - h / 2.0, 1.0);
            double phiTilde = p.f.integral(1.0 - h / 2.0, 1.0);
            A[n] = a[n] / h;
            C[n] = a[n] / h + GAMMA2 + dTilde;
            B[n] = 0.0;
            F[n] = GAMMA2 * THETA2 + phiTilde;
        }

        return progonka(A, C, B, F);
    }

    // ===================== СЕРВИСНАЯ ОБЁРТКА =====================
    // Возвращает ProblemData по типу задачи (тестовая / основная).
    inline ProblemData get_problem(ProblemType type) {
        return (type == ProblemType::Test) ? variant7_test() : variant7_main();
    }

} // namespace BalanceSolver