// Progonka.h — метод прогонки для трёхдиагональной СЛАУ.
//
// Каноническая форма (Самарский):
//     A[i] · v[i−1] − C[i] · v[i] + B[i] · v[i+1] = − F[i],   i = 0 .. N−1,
// где A[0] = 0 и B[N−1] = 0 (граничные строки).
//
// Прямой ход:  v[i] = α[i] · v[i+1] + β[i].
// Обратный ход: по найденному v[N−1] восстанавливаем v[i] от конца к началу.
//
// Условие применимости (теорема об устойчивости прогонки):
//     |C[i]| ≥ |A[i]| + |B[i]|  с хотя бы одним строгим неравенством.
// Для построенной методом баланса схемы оно выполняется автоматически
// (см. Модуль 4, п. 4.3).

#pragma once

#include <vector>

namespace BalanceSolver {

inline std::vector<double> progonka(
    const std::vector<double>& A,
    const std::vector<double>& C,
    const std::vector<double>& B,
    const std::vector<double>& F)
{
    int N = (int)A.size();
    std::vector<double> alpha(N), beta(N), v(N);

    // Прямой ход.
    alpha[0] = B[0] / C[0];
    beta[0]  = F[0] / C[0];
    for (int i = 1; i < N - 1; ++i) {
        double denom = C[i] - A[i] * alpha[i - 1];
        alpha[i] = B[i] / denom;
        beta[i]  = (F[i] + A[i] * beta[i - 1]) / denom;
    }

    // Последний узел: v[N−1] находим напрямую.
    int last = N - 1;
    v[last] = (F[last] + A[last] * beta[last - 1])
            / (C[last] - A[last] * alpha[last - 1]);

    // Обратный ход.
    for (int i = last - 1; i >= 0; --i) {
        v[i] = alpha[i] * v[i + 1] + beta[i];
    }

    return v;
}

} // namespace BalanceSolver
