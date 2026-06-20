#include "MyForm.h"
#include "Solver.h"

#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Windows::Forms::DataVisualization::Charting;

// ===== Утилиты конверсии std::string <-> System::String^ =====

static String^ stdToCli(const std::string& s) {
    array<unsigned char>^ bytes = gcnew array<unsigned char>((int)s.size());
    for (int i = 0; i < (int)s.size(); ++i) bytes[i] = (unsigned char)s[i];
    return System::Text::Encoding::UTF8->GetString(bytes);
}

static String^ FmtD(double v) {
    std::ostringstream oss; oss.precision(12); oss << v;
    return stdToCli(oss.str());
}

static String^ FmtSci(double v) {
    std::ostringstream oss; oss << std::scientific << std::setprecision(4) << v;
    return stdToCli(oss.str());
}

static String^ FmtFixed(double v, int prec) {
    std::ostringstream oss; oss << std::fixed << std::setprecision(prec) << v;
    return stdToCli(oss.str());
}

// Обратная конвертация: System::String^ → std::string (UTF-8).
// Нужна, чтобы вставлять текст из контролов в std::ostringstream.
static std::string cliToStd(System::String^ s) {
    if (s == nullptr) return "";
    array<unsigned char>^ bytes = System::Text::Encoding::UTF8->GetBytes(s);
    std::string r((int)bytes->Length, '\0');
    for (int i = 0; i < bytes->Length; ++i) r[i] = (char)bytes[i];
    return r;
}

namespace Lab3 {

    // ===== Вспомогательные функции работы с UI =====

    static void EnsureGridColumns(DataGridView^ dgv, array<String^>^ headers) {
        if (dgv->ColumnCount != headers->Length) {
            dgv->Columns->Clear();
            dgv->ColumnCount = headers->Length;
        }
        for (int i = 0; i < headers->Length; ++i) {
            dgv->Columns[i]->HeaderText = headers[i];
            dgv->Columns[i]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;
            dgv->Columns[i]->DefaultCellStyle->Font = gcnew System::Drawing::Font(L"Consolas", 9);
        }
    }

    static void FillSolutionsChart(Chart^ ch, const std::vector<double>& x,
        const std::vector<double>& y1, const std::vector<double>& y2,
        String^ name1, String^ name2)
    {
        ch->Series->Clear();

        // Первая кривая — сплошная толстая линия.
        Series^ s1 = gcnew Series(name1);
        s1->ChartType = SeriesChartType::Line;
        s1->BorderWidth = 3;
        s1->Color = Color::FromArgb(0, 90, 180);

        // Вторая кривая почти совпадает с первой, поэтому рисуем её
        // редкими маркерами поверх — так видны обе: сплошная линия и точки,
        // лежащие на ней. Линию у второй серии тоже оставляем (тонкую,
        // другого цвета), на случай если решения заметно разойдутся.
        Series^ s2 = gcnew Series(name2);
        s2->ChartType = SeriesChartType::Line;
        s2->BorderWidth = 1;
        s2->Color = Color::FromArgb(220, 60, 50);
        s2->MarkerStyle = MarkerStyle::Circle;
        s2->MarkerSize = 6;
        s2->MarkerColor = Color::FromArgb(220, 60, 50);
        s2->MarkerStep = 1;

        int N = (int)x.size();
        // Для второй серии маркеры должны быть редкими (иначе сольются в кашу).
        // Линию первой серии можно рисовать чаще.
        int step1 = (N > 1500) ? (N / 1500) : 1; // плотность линии u(x)
        int step2 = (N > 40) ? (N / 40) : 1; // плотность маркеров v(x): ~40 точек

        for (int i = 0; i < N; i += step1) s1->Points->AddXY(x[i], y1[i]);
        if ((N - 1) % step1 != 0)          s1->Points->AddXY(x[N - 1], y1[N - 1]);

        for (int i = 0; i < N; i += step2) s2->Points->AddXY(x[i], y2[i]);
        if ((N - 1) % step2 != 0)          s2->Points->AddXY(x[N - 1], y2[N - 1]);

        ch->Series->Add(s1);
        ch->Series->Add(s2);
        ch->ChartAreas[0]->RecalculateAxesScale();
    }

    static void FillErrorChart(Chart^ ch, const std::vector<double>& x,
        const std::vector<double>& err, String^ name)
    {
        ch->Series->Clear();
        Series^ s = gcnew Series(name);
        s->ChartType = SeriesChartType::Line;
        s->BorderWidth = 2;
        s->Color = Color::FromArgb(220, 130, 30);

        int N = (int)x.size();
        int step = (N > 1500) ? (N / 1500) : 1;
        for (int i = 0; i < N; i += step) s->Points->AddXY(x[i], err[i]);
        if ((N - 1) % step != 0)         s->Points->AddXY(x[N - 1], err[N - 1]);

        ch->Series->Add(s);
        ch->ChartAreas[0]->RecalculateAxesScale();
    }

    static void FillTable(DataGridView^ dgv, const std::vector<double>& x,
        const std::vector<double>& y1, const std::vector<double>& y2,
        const std::vector<double>& err)
    {
        dgv->Rows->Clear();
        int N = (int)x.size();
        std::vector<int> idx;
        if (N <= 45) {
            for (int i = 0; i < N; ++i) idx.push_back(i);
        }
        else {
            // Включаем оба граничных узла: i = 0 и i = N−1 (обязательное требование бланка отчёта).
            // Выводим ~41 равномерно распределённую строку.
            for (int k = 0; k < 41; ++k)
                idx.push_back((int)((long long)k * (N - 1) / 40));
            std::sort(idx.begin(), idx.end());
            idx.erase(std::unique(idx.begin(), idx.end()), idx.end());
        }
        for (size_t k = 0; k < idx.size(); ++k) {
            int i = idx[k];
            dgv->Rows->Add(i.ToString(),
                FmtFixed(x[i], 10),
                FmtD(y1[i]),
                FmtD(y2[i]),
                FmtSci(err[i]));
        }
    }

    // ===== Чтение конфигурации из контролов =====
    // Методы класса возвращают int-коды (см. MyForm.h). Преобразование
    // в перечисления BalanceSolver делается локально в .cpp.

    static BalanceSolver::ProblemKind KindFromInt(int k) {
        return (k == 0) ? BalanceSolver::ProblemKind::First
            : BalanceSolver::ProblemKind::Third;
    }
    static BalanceSolver::ProblemType TypeFromInt(int t) {
        return (t == 0) ? BalanceSolver::ProblemType::Test
            : BalanceSolver::ProblemType::Main;
    }
    static BalanceSolver::BCScheme SchemeFromInt(int s) {
        return (s == 0) ? BalanceSolver::BCScheme::Classical
            : BalanceSolver::BCScheme::Improved;
    }

    void MyForm::UpdateSchemeEnabled() {
        // Селектор схемы аппроксимации ГУ имеет смысл только для 3-й краевой:
        // в 1-й краевой задаче u задано в граничных узлах точно.
        bool kindThird = this->rbKindThird->Checked;
        this->grpScheme->Enabled = kindThird;
    }

    int MyForm::CurrentKind() {
        return this->rbKindFirst->Checked ? 0 : 1;
    }

    int MyForm::CurrentType() {
        return this->rbTypeTest->Checked ? 0 : 1;
    }

    int MyForm::CurrentScheme() {
        // Для 1-й краевой селектор схемы игнорируется.
        return this->rbSchemeImproved->Checked ? 1 : 0;
    }

    String^ MyForm::ConfigSuffix() {
        int kind = CurrentKind();
        int type = CurrentType();
        String^ s1 = (kind == 0) ? L"1-я краевая" : L"3-я краевая";
        String^ s2 = (type == 0) ? L"тестовая" : L"основная";
        String^ s3 = L"";
        if (kind == 1) {
            s3 = (CurrentScheme() == 1)
                ? L", улучшенная аппроксимация ГУ"
                : L", классическая аппроксимация ГУ";
        }
        return String::Format(L"{0}, {1}{2}", s1, s2, s3);
    }

    System::Void MyForm::rbKind_CheckedChanged(System::Object^, System::EventArgs^) {
        UpdateSchemeEnabled();
    }

    // ===== Внутренние процедуры расчёта =====
    // Не методы MyForm, чтобы не упоминать BCScheme в .h-файле дважды.

    // Считает ε₁ — максимум модуля разности численного и аналитического решений
    // (для тестовой задачи). Возвращает также узел x*, в котором достигнут максимум.
    static double MaxErrTest(BalanceSolver::ProblemKind kind,
        BalanceSolver::BCScheme scheme, int n,
        int* idxMaxOut = nullptr)
    {
        auto problem = BalanceSolver::variant7_test();
        auto v = BalanceSolver::solve_bvp(problem, n, kind, scheme);
        auto u = BalanceSolver::analytical_test_solution(kind);

        double h = 1.0 / n;
        double me = 0.0;
        int    iMax = 0;
        for (int i = 0; i <= n; ++i) {
            double d = std::fabs(u(i * h) - v[i]);
            if (d > me) { me = d; iMax = i; }
        }
        if (idxMaxOut) *idxMaxOut = iMax;
        return me;
    }

    // Считает ε₂ — правило Рунге для основной задачи: max|v(xᵢ) − v₂(x₂ᵢ)|.
    static double MaxErrMain(BalanceSolver::ProblemKind kind,
        BalanceSolver::BCScheme scheme, int n,
        int* idxMaxOut = nullptr)
    {
        auto problem = BalanceSolver::variant7_main();
        auto v = BalanceSolver::solve_bvp(problem, n, kind, scheme);
        auto v2 = BalanceSolver::solve_bvp(problem, 2 * n, kind, scheme);

        double me = 0.0;
        int    iMax = 0;
        for (int i = 0; i <= n; ++i) {
            double d = std::fabs(v[i] - v2[2 * i]);
            if (d > me) { me = d; iMax = i; }
        }
        if (idxMaxOut) *idxMaxOut = iMax;
        return me;
    }

    // ===== Обработчик «Решить» =====

    System::Void MyForm::btnSolve_Click(System::Object^, System::EventArgs^) {
        try {
            int  n = (int)numN->Value;
            auto kind = KindFromInt(CurrentKind());
            auto type = TypeFromInt(CurrentType());
            auto scheme = SchemeFromInt(CurrentScheme());

            if (type == BalanceSolver::ProblemType::Main && 2 * n > 2000000) {
                MessageBox::Show(
                    L"Для оценки точности нужен расчёт на сетке 2n. n слишком велик.",
                    L"Внимание");
                return;
            }

            this->Cursor = Cursors::WaitCursor;

            const double h = 1.0 / n;
            std::vector<double> x(n + 1), y1(n + 1), y2(n + 1), err(n + 1);
            double maxAbs = 0.0;
            int    iMax = 0;

            String^ titleSols;
            String^ titleErr;
            array<String^>^ tableHeads;

            if (type == BalanceSolver::ProblemType::Test) {
                // ---- Тестовая задача: u(x) — аналитика, v(x) — численное решение, ε₁ = max|u − v|.
                auto problem = BalanceSolver::variant7_test();
                auto v = BalanceSolver::solve_bvp(problem, n, kind, scheme);
                auto u = BalanceSolver::analytical_test_solution(kind);

                for (int i = 0; i <= n; ++i) {
                    x[i] = i * h;
                    y1[i] = u(x[i]); // u(xᵢ)
                    y2[i] = v[i];    // v(xᵢ)
                    err[i] = y1[i] - y2[i];
                    if (std::fabs(err[i]) > maxAbs) {
                        maxAbs = std::fabs(err[i]);
                        iMax = i;
                    }
                }

                titleSols = L"u(x) аналитическое и v(x) численное";
                titleErr = L"Погрешность u(xᵢ) − v(xᵢ)";
                tableHeads = gcnew array<String^>{ L"i", L"xᵢ", L"u(xᵢ)", L"v(xᵢ)", L"u − v" };

                std::ostringstream oss;
                oss << "СПРАВКА (тестовая задача)\n";
                oss << "Конфигурация: " << cliToStd(ConfigSuffix()) << "\n";
                oss << "Число разбиений n = " << n << " (узлов " << (n + 1) << "), h = 1/" << n << ".\n";
                oss << "Заданная погрешность: ε = 0.5·10⁻⁶.\n";
                oss << std::scientific << std::setprecision(4);
                oss << "Достигнутая ε₁ = max|u(xᵢ) − v(xᵢ)| = " << maxAbs << ".\n";
                oss << std::fixed << std::setprecision(6);
                oss << "Максимум в узле x = " << x[iMax] << " (i = " << iMax << ").\n";
                oss << "Статус: "
                    << (maxAbs <= 0.5e-6 ? "ε₁ ≤ ε  ✓  (точность достигнута)"
                        : "ε₁ > ε  —  увеличьте n");
                lblInfo->Text = stdToCli(oss.str());

            }
            else {
                // ---- Основная задача: v(x) на n, v₂(x) на 2n, ε₂ = max|v − v₂|.
                auto problem = BalanceSolver::variant7_main();
                auto v = BalanceSolver::solve_bvp(problem, n, kind, scheme);
                auto v2 = BalanceSolver::solve_bvp(problem, 2 * n, kind, scheme);

                for (int i = 0; i <= n; ++i) {
                    x[i] = i * h;
                    y1[i] = v[i];        // v(xᵢ)
                    y2[i] = v2[2 * i];   // v₂(x₂ᵢ)
                    err[i] = y1[i] - y2[i];
                    if (std::fabs(err[i]) > maxAbs) {
                        maxAbs = std::fabs(err[i]);
                        iMax = i;
                    }
                }

                titleSols = L"v(x) на n и v₂(x) на 2n";
                titleErr = L"Разность v(xᵢ) − v₂(x₂ᵢ)";
                tableHeads = gcnew array<String^>{ L"i", L"xᵢ", L"v(xᵢ)", L"v₂(x₂ᵢ)", L"v − v₂" };

                std::ostringstream oss;
                oss << "СПРАВКА (основная задача)\n";
                oss << "Конфигурация: " << cliToStd(ConfigSuffix()) << "\n";
                oss << "Число разбиений n = " << n << " (узлов " << (n + 1) << "), h = 1/" << n << ".\n";
                oss << "Контрольная сетка: 2n = " << (2 * n) << ".\n";
                oss << "Заданная точность: ε = 0.5·10⁻⁶.\n";
                oss << std::scientific << std::setprecision(4);
                oss << "Достигнутая ε₂ = max|v(xᵢ) − v₂(x₂ᵢ)| = " << maxAbs << ".\n";
                oss << std::fixed << std::setprecision(6);
                oss << "Максимум в узле x = " << x[iMax] << " (i = " << iMax << ").\n";
                oss << "Статус: "
                    << (maxAbs <= 0.5e-6 ? "ε₂ ≤ ε  ✓  (точность достигнута)"
                        : "ε₂ > ε  —  увеличьте n");
                lblInfo->Text = stdToCli(oss.str());
            }

            // Заголовки графиков
            this->tiSols->Text = titleSols;
            this->tiErr->Text = titleErr;

            FillSolutionsChart(chSols, x, y1, y2,
                (type == BalanceSolver::ProblemType::Test) ? L"u(x)" : L"v(x), n",
                (type == BalanceSolver::ProblemType::Test) ? L"v(x)" : L"v₂(x), 2n");
            FillErrorChart(chErr, x, err,
                (type == BalanceSolver::ProblemType::Test) ? L"u − v" : L"v − v₂");

            EnsureGridColumns(dgvSolve, tableHeads);
            FillTable(dgvSolve, x, y1, y2, err);

            tabs->SelectedIndex = 0;
        }
        catch (std::exception& e) {
            MessageBox::Show(stdToCli(e.what()), L"Ошибка");
        }
        catch (Exception^ ex) {
            MessageBox::Show(ex->Message, L"Ошибка");
        }
        this->Cursor = Cursors::Default;
    }

    // ===== «Проверка порядка» =====

    System::Void MyForm::btnConv_Click(System::Object^, System::EventArgs^) {
        try {
            auto kind = KindFromInt(CurrentKind());
            auto scheme = SchemeFromInt(CurrentScheme());

            this->Cursor = Cursors::WaitCursor;

            // Ряд n удваивается от 10 до верхней границы, которую задаёт поле
            // «Число разбиений n» (numN). Так можно прямо в окне менять, как
            // далеко считать, без перекомпиляции. Минимальный потолок — 320,
            // чтобы таблица 4-5 бланка отчёта всегда заполнялась полностью.
            int nMax = (int)numN->Value;
            if (nMax < 320) nMax = 320;

            std::vector<int> ns;
            for (int n = 10; n <= nMax; n *= 2) ns.push_back(n);
            const int K = (int)ns.size();

            std::vector<double> e1(K), e2(K);

            for (int k = 0; k < K; ++k) {
                int n = ns[k];
                e1[k] = MaxErrTest(kind, scheme, n);
                e2[k] = MaxErrMain(kind, scheme, n);
            }

            array<String^>^ heads = {
                L"n",
                L"ε₁ (тестовая)",
                L"ε(n)/ε(2n) — тест",
                L"ε₂ (основная)",
                L"ε(n)/ε(2n) — осн."
            };
            EnsureGridColumns(dgvConv, heads);
            dgvConv->Rows->Clear();

            for (int k = 0; k < K; ++k) {
                String^ r1 = (k > 0 && e1[k] > 0.0) ? FmtFixed(e1[k - 1] / e1[k], 3) : L"—";
                String^ r2 = (k > 0 && e2[k] > 0.0) ? FmtFixed(e2[k - 1] / e2[k], 3) : L"—";
                dgvConv->Rows->Add(
                    ns[k].ToString(),
                    FmtSci(e1[k]), r1,
                    FmtSci(e2[k]), r2
                );
                Application::DoEvents();
            }

            tabs->SelectedIndex = 1;
        }
        catch (Exception^ ex) {
            MessageBox::Show(ex->Message, L"Ошибка");
        }
        this->Cursor = Cursors::Default;
    }

} // namespace Lab3