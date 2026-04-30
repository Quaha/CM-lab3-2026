#include "MyForm.h"
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
    std::ostringstream oss; oss.precision(7); oss << v;
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

static double ParseDouble(String^ s, double dflt) {
    if (s == nullptr || s->Length == 0) return dflt;
    try {
        return Double::Parse(s->Replace(",", "."),
            System::Globalization::CultureInfo::InvariantCulture);
    } catch (...) { return dflt; }
}

namespace Lab3 {

    // ===== Helpers (приватные функции, не члены MyForm) =====
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
        Series^ s1 = gcnew Series(name1);
        s1->ChartType = SeriesChartType::Line;
        s1->BorderWidth = 2;
        s1->Color = Color::FromArgb(0, 90, 180);

        Series^ s2 = gcnew Series(name2);
        s2->ChartType = SeriesChartType::Line;
        s2->BorderWidth = 2;
        s2->Color = Color::FromArgb(220, 60, 50);
        s2->BorderDashStyle = ChartDashStyle::Dash;

        int N = (int)x.size();
        int step = (N > 1500) ? (N / 1500) : 1;
        for (int i = 0; i < N; i += step) {
            s1->Points->AddXY(x[i], y1[i]);
            s2->Points->AddXY(x[i], y2[i]);
        }
        if ((N - 1) % step != 0) {
            s1->Points->AddXY(x[N-1], y1[N-1]);
            s2->Points->AddXY(x[N-1], y2[N-1]);
        }

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
        if ((N - 1) % step != 0) s->Points->AddXY(x[N-1], err[N-1]);

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
        if (N <= 25) {
            for (int i = 0; i < N; ++i) idx.push_back(i);
        } else {
            for (int k = 0; k < 21; ++k) idx.push_back((int)((long long)k * (N - 1) / 20));
            std::sort(idx.begin(), idx.end());
            idx.erase(std::unique(idx.begin(), idx.end()), idx.end());
        }
        for (size_t k = 0; k < idx.size(); ++k) {
            int i = idx[k];
            dgv->Rows->Add(i.ToString(),
                FmtFixed(x[i], 6),
                FmtD(y1[i]),
                FmtD(y2[i]),
                FmtSci(err[i]));
        }
    }

    static BalanceSolver::BoundaryCondition MakeBc(int idx, double gamma, double sigma) {
        BalanceSolver::BoundaryCondition bc;
        bc.type  = (BalanceSolver::BCType)idx;
        bc.gamma = gamma;
        bc.sigma = sigma;
        return bc;
    }

    // ====================== ОБРАБОТЧИКИ ======================

    System::Void MyForm::cmbBcL_SelectedIndexChanged(System::Object^, System::EventArgs^) {
        bool isThirdType = (cmbBcL->SelectedIndex == 2);
        lblSigmaL->Visible = isThirdType;
        txtSigmaL->Visible = isThirdType;
    }

    System::Void MyForm::cmbBcR_SelectedIndexChanged(System::Object^, System::EventArgs^) {
        bool isThirdType = (cmbBcR->SelectedIndex == 2);
        lblSigmaR->Visible = isThirdType;
        txtSigmaR->Visible = isThirdType;
    }

    System::Void MyForm::btnSolveTest_Click(System::Object^, System::EventArgs^) {
        try {
            int n = (int)numN->Value;
            auto bcL = MakeBc(cmbBcL->SelectedIndex,
                              ParseDouble(txtGammaL->Text, 0.0),
                              ParseDouble(txtSigmaL->Text, 0.0));
            auto bcR = MakeBc(cmbBcR->SelectedIndex,
                              ParseDouble(txtGammaR->Text, 0.0),
                              ParseDouble(txtSigmaR->Text, 0.0));

            this->Cursor = Cursors::WaitCursor;

            auto problem = BalanceSolver::variant1_test();
            auto v = BalanceSolver::solve_bvp(problem, n, bcL, bcR);
            auto u = BalanceSolver::analytical_test_solution(bcL, bcR);

            const double h = 1.0 / n;
            std::vector<double> x(n+1), uVals(n+1), errs(n+1);
            double maxErr = 0.0; int idxMax = 0;
            for (int i = 0; i <= n; ++i) {
                x[i] = i * h;
                uVals[i] = u(x[i]);
                errs[i]  = uVals[i] - v[i];
                if (std::fabs(errs[i]) > maxErr) { maxErr = std::fabs(errs[i]); idxMax = i; }
            }

            std::ostringstream oss;
            oss << "СПРАВКА (тестовая задача):\n";
            oss << "Число разбиений n = " << n << " (узлов " << (n+1) << "), h = 1/" << n << ".\n";
            oss << "Заданная точность: 0.5e-06.\n";
            oss << std::scientific << std::setprecision(4);
            oss << "Достигнутая ε₁ = max|u(xᵢ) − v(xᵢ)| = " << maxErr << ".\n";
            oss << std::fixed << std::setprecision(6);
            oss << "Максимум в узле x = " << x[idxMax] << " (i = " << idxMax << ").\n";
            oss << "Статус: " << (maxErr <= 0.5e-6 ? "ε₁ ≤ ε ✓ (точность достигнута)"
                                                  : "ε₁ > ε — увеличьте n");
            lblTestInfo->Text = stdToCli(oss.str());

            FillSolutionsChart(chTestSols, x, uVals, v, L"u(x) аналит.", L"v(x) числен.");
            FillErrorChart(chTestErr, x, errs, L"u − v");

            array<String^>^ heads = { L"i", L"xᵢ", L"u(xᵢ)", L"v(xᵢ)", L"u − v" };
            EnsureGridColumns(dgvTest, heads);
            FillTable(dgvTest, x, uVals, v, errs);
        } catch (std::exception& e) {
            MessageBox::Show(stdToCli(e.what()), L"Ошибка");
        } catch (Exception^ ex) {
            MessageBox::Show(ex->Message, L"Ошибка");
        }
        this->Cursor = Cursors::Default;
    }

    System::Void MyForm::btnSolveMain_Click(System::Object^, System::EventArgs^) {
        try {
            int n = (int)numN->Value;
            if (2*n > 2000000) {
                MessageBox::Show(L"Для оценки погрешности нужен расчёт на сетке 2n. Слишком большой n.",
                                 L"Внимание");
                return;
            }
            auto bcL = MakeBc(cmbBcL->SelectedIndex,
                              ParseDouble(txtGammaL->Text, 0.0),
                              ParseDouble(txtSigmaL->Text, 0.0));
            auto bcR = MakeBc(cmbBcR->SelectedIndex,
                              ParseDouble(txtGammaR->Text, 0.0),
                              ParseDouble(txtSigmaR->Text, 0.0));

            this->Cursor = Cursors::WaitCursor;

            auto problem = BalanceSolver::variant1_main();
            auto v  = BalanceSolver::solve_bvp(problem, n,    bcL, bcR);
            auto v2 = BalanceSolver::solve_bvp(problem, 2*n,  bcL, bcR);

            const double h = 1.0 / n;
            std::vector<double> x(n+1), v2Common(n+1), diffs(n+1);
            double maxDiff = 0.0; int idxMax = 0;
            for (int i = 0; i <= n; ++i) {
                x[i] = i * h;
                v2Common[i] = v2[2*i];
                diffs[i] = v[i] - v2Common[i];
                if (std::fabs(diffs[i]) > maxDiff) { maxDiff = std::fabs(diffs[i]); idxMax = i; }
            }

            std::ostringstream oss;
            oss << "СПРАВКА (основная задача):\n";
            oss << "Число разбиений n = " << n << " (узлов " << (n+1) << "), h = 1/" << n << ".\n";
            oss << "Контрольная сетка: 2n = " << (2*n) << ".\n";
            oss << "Заданная точность: 0.5e-06.\n";
            oss << std::scientific << std::setprecision(4);
            oss << "Достигнутая ε₂ = max|v(xᵢ) − v₂(x₂ᵢ)| = " << maxDiff << ".\n";
            oss << std::fixed << std::setprecision(6);
            oss << "Максимум в узле x = " << x[idxMax] << " (i = " << idxMax << ").\n";
            oss << "Статус: " << (maxDiff <= 0.5e-6 ? "ε₂ ≤ ε ✓ (точность достигнута)"
                                                   : "ε₂ > ε — увеличьте n");
            lblMainInfo->Text = stdToCli(oss.str());

            FillSolutionsChart(chMainSols, x, v, v2Common, L"v(x), n", L"v₂(x), 2n");
            FillErrorChart(chMainErr, x, diffs, L"v − v₂");

            array<String^>^ heads = { L"i", L"xᵢ", L"v(xᵢ)", L"v₂(x₂ᵢ)", L"v − v₂" };
            EnsureGridColumns(dgvMain, heads);
            FillTable(dgvMain, x, v, v2Common, diffs);
        } catch (std::exception& e) {
            MessageBox::Show(stdToCli(e.what()), L"Ошибка");
        } catch (Exception^ ex) {
            MessageBox::Show(ex->Message, L"Ошибка");
        }
        this->Cursor = Cursors::Default;
    }

    System::Void MyForm::btnConv_Click(System::Object^, System::EventArgs^) {
        try {
            auto bcL = MakeBc(cmbBcL->SelectedIndex,
                              ParseDouble(txtGammaL->Text, 0.0),
                              ParseDouble(txtSigmaL->Text, 0.0));
            auto bcR = MakeBc(cmbBcR->SelectedIndex,
                              ParseDouble(txtGammaR->Text, 0.0),
                              ParseDouble(txtSigmaR->Text, 0.0));
            this->Cursor = Cursors::WaitCursor;

            const int ns[] = {10, 20, 40, 80, 160, 320};
            const int K = (int)(sizeof(ns)/sizeof(ns[0]));

            auto problemTest = BalanceSolver::variant1_test();
            auto problemMain = BalanceSolver::variant1_main();
            std::vector<double> e1(K), e2(K);

            for (int k = 0; k < K; ++k) {
                int n = ns[k];

                auto v = BalanceSolver::solve_bvp(problemTest, n, bcL, bcR);
                auto u = BalanceSolver::analytical_test_solution(bcL, bcR);
                double h = 1.0 / n;
                double me = 0.0;
                for (int i = 0; i <= n; ++i) me = std::max(me, std::fabs(u(i*h) - v[i]));
                e1[k] = me;

                auto vM  = BalanceSolver::solve_bvp(problemMain, n,   bcL, bcR);
                auto v2M = BalanceSolver::solve_bvp(problemMain, 2*n, bcL, bcR);
                double md = 0.0;
                for (int i = 0; i <= n; ++i) md = std::max(md, std::fabs(vM[i] - v2M[2*i]));
                e2[k] = md;
            }

            array<String^>^ heads = { L"n", L"ε", L"ε(n)/ε(2n)" };
            EnsureGridColumns(dgvConvTest, heads);
            EnsureGridColumns(dgvConvMain, heads);

            dgvConvTest->Rows->Clear();
            dgvConvMain->Rows->Clear();
            for (int k = 0; k < K; ++k) {
                String^ r1 = (k > 0 && e1[k] > 0.0) ? FmtFixed(e1[k-1]/e1[k], 3) : L"—";
                String^ r2 = (k > 0 && e2[k] > 0.0) ? FmtFixed(e2[k-1]/e2[k], 3) : L"—";
                dgvConvTest->Rows->Add(ns[k].ToString(), FmtSci(e1[k]), r1);
                dgvConvMain->Rows->Add(ns[k].ToString(), FmtSci(e2[k]), r2);
            }
            tabs->SelectedIndex = 2;
        } catch (Exception^ ex) {
            MessageBox::Show(ex->Message, L"Ошибка");
        }
        this->Cursor = Cursors::Default;
    }

    System::Void MyForm::btnWide_Click(System::Object^, System::EventArgs^) {
        try {
            auto bcL = MakeBc(cmbBcL->SelectedIndex,
                              ParseDouble(txtGammaL->Text, 0.0),
                              ParseDouble(txtSigmaL->Text, 0.0));
            auto bcR = MakeBc(cmbBcR->SelectedIndex,
                              ParseDouble(txtGammaR->Text, 0.0),
                              ParseDouble(txtSigmaR->Text, 0.0));
            this->Cursor = Cursors::WaitCursor;

            const int ns[] = {10, 20, 50, 80, 100, 200, 500, 800, 1000,
                              2000, 5000, 10000, 50000, 100000, 500000, 1000000};
            const int K = (int)(sizeof(ns)/sizeof(ns[0]));

            auto problemTest = BalanceSolver::variant1_test();
            auto problemMain = BalanceSolver::variant1_main();

            array<String^>^ heads = { L"n", L"ε₁ (тест)", L"ε₂ (осн.)", L"−lg ε₁", L"−lg ε₂" };
            EnsureGridColumns(dgvWide, heads);
            dgvWide->Rows->Clear();

            for (int k = 0; k < K; ++k) {
                int n = ns[k];

                auto v = BalanceSolver::solve_bvp(problemTest, n, bcL, bcR);
                auto u = BalanceSolver::analytical_test_solution(bcL, bcR);
                double h = 1.0 / n;
                double e1 = 0.0;
                for (int i = 0; i <= n; ++i) e1 = std::max(e1, std::fabs(u(i*h) - v[i]));

                double e2 = -1.0;
                if (2 * n <= 2000000) {
                    auto vM  = BalanceSolver::solve_bvp(problemMain, n, bcL, bcR);
                    auto v2M = BalanceSolver::solve_bvp(problemMain, 2*n, bcL, bcR);
                    e2 = 0.0;
                    for (int i = 0; i <= n; ++i) e2 = std::max(e2, std::fabs(vM[i] - v2M[2*i]));
                }

                String^ s_e2     = (e2 < 0)   ? L"—" : FmtSci(e2);
                String^ s_neg_l1 = (e1 > 0.0) ? FmtFixed(-std::log10(e1), 3) : L"—";
                String^ s_neg_l2 = (e2 > 0.0) ? FmtFixed(-std::log10(e2), 3) : L"—";

                dgvWide->Rows->Add(n.ToString(), FmtSci(e1), s_e2, s_neg_l1, s_neg_l2);
                Application::DoEvents();
            }
            tabs->SelectedIndex = 3;
        } catch (Exception^ ex) {
            MessageBox::Show(ex->Message, L"Ошибка");
        }
        this->Cursor = Cursors::Default;
    }

}
