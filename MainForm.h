// MainForm.h — главная форма приложения (C++/CLI WinForms)
#pragma once
#pragma execution_character_set("utf-8")

#include "Solver.h"
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

namespace BalanceMethodApp {

using namespace System;
using namespace System::Drawing;
using namespace System::Windows::Forms;
using namespace System::Windows::Forms::DataVisualization::Charting;

// ==== Утилиты конверсии std::string <-> System::String^ через UTF-8 ====
inline String^ stdToCli(const std::string& s) {
    array<unsigned char>^ bytes = gcnew array<unsigned char>((int)s.size());
    for (int i = 0; i < (int)s.size(); ++i) bytes[i] = (unsigned char)(unsigned char)s[i];
    return System::Text::Encoding::UTF8->GetString(bytes);
}

inline String^ FmtD(double v) {
    std::ostringstream oss;
    oss.precision(7);
    oss << v;
    return stdToCli(oss.str());
}
inline String^ FmtSci(double v) {
    std::ostringstream oss;
    oss << std::scientific << std::setprecision(4) << v;
    return stdToCli(oss.str());
}
inline String^ FmtFixed(double v, int prec) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(prec) << v;
    return stdToCli(oss.str());
}

public ref class MainForm : public Form {
public:
    MainForm() {
        BuildUI();
    }

private:
    // === Поля ввода ===
    NumericUpDown^ numN;
    ComboBox^ cmbBcL;  TextBox^ txtGammaL;  TextBox^ txtSigmaL;  Label^ lblSigmaL;
    ComboBox^ cmbBcR;  TextBox^ txtGammaR;  TextBox^ txtSigmaR;  Label^ lblSigmaR;
    Button^ btnSolveTest;
    Button^ btnSolveMain;
    Button^ btnConv;
    Button^ btnWide;

    // === Вкладки ===
    TabControl^ tabs;

    Label^ lblTestInfo;
    Chart^ chTestSols;
    Chart^ chTestErr;
    DataGridView^ dgvTest;

    Label^ lblMainInfo;
    Chart^ chMainSols;
    Chart^ chMainErr;
    DataGridView^ dgvMain;

    DataGridView^ dgvConvTest;
    DataGridView^ dgvConvMain;

    DataGridView^ dgvWide;

    // ============================================================
    void BuildUI() {
        this->Text = L"Лабораторная 2 — Метод баланса (вариант 1)";
        this->ClientSize = System::Drawing::Size(1280, 800);
        this->StartPosition = FormStartPosition::CenterScreen;
        this->MinimumSize = System::Drawing::Size(1024, 600);
        this->Font = gcnew System::Drawing::Font(L"Segoe UI", 9);

        // Верхняя панель
        Panel^ top = gcnew Panel();
        top->Dock = DockStyle::Top;
        top->Height = 130;
        top->BackColor = Color::FromArgb(245, 245, 248);
        this->Controls->Add(top);

        // n
        Label^ lblN = gcnew Label();
        lblN->Text = L"Число разбиений n:";
        lblN->Location = Point(10, 12);
        lblN->Width = 130;
        top->Controls->Add(lblN);

        numN = gcnew NumericUpDown();
        numN->Location = Point(140, 10);
        numN->Width = 100;
        numN->Minimum = 4;
        numN->Maximum = 1000000;
        numN->Value = 100;
        top->Controls->Add(numN);

        Label^ lblVar = gcnew Label();
        lblVar->Text = L"Вариант 1:  ξ = 0.4,  k₁=x+1, k₂=x,  q₁=x, q₂=x²,  f₁=x, f₂=exp(-x),  μ₁=0, μ₂=1";
        lblVar->Location = Point(260, 12);
        lblVar->Width = 800;
        lblVar->ForeColor = Color::FromArgb(20, 60, 130);
        top->Controls->Add(lblVar);

        // Левое ГУ
        GroupBox^ grpL = gcnew GroupBox();
        grpL->Text = L"Левое граничное условие при x = 0";
        grpL->Location = Point(10, 38);
        grpL->Size = System::Drawing::Size(400, 85);
        top->Controls->Add(grpL);

        Label^ lblTL = gcnew Label();
        lblTL->Text = L"Тип:";
        lblTL->Location = Point(10, 25);
        lblTL->Width = 35;
        grpL->Controls->Add(lblTL);

        cmbBcL = gcnew ComboBox();
        cmbBcL->DropDownStyle = ComboBoxStyle::DropDownList;
        cmbBcL->Items->Add(L"Дирихле:  u(0) = γ");
        cmbBcL->Items->Add(L"Нейман:   −k(0)·u'(0) = γ");
        cmbBcL->Items->Add(L"Робин:    −k(0)·u'(0) + σ·u(0) = γ");
        cmbBcL->SelectedIndex = 0;
        cmbBcL->Location = Point(50, 22);
        cmbBcL->Width = 270;
        cmbBcL->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::OnBcLeftChanged);
        grpL->Controls->Add(cmbBcL);

        Label^ lblGL = gcnew Label();
        lblGL->Text = L"γ:"; lblGL->Location = Point(10, 56); lblGL->Width = 15;
        grpL->Controls->Add(lblGL);

        txtGammaL = gcnew TextBox();
        txtGammaL->Text = L"0";
        txtGammaL->Location = Point(28, 53); txtGammaL->Width = 70;
        grpL->Controls->Add(txtGammaL);

        lblSigmaL = gcnew Label();
        lblSigmaL->Text = L"σ:"; lblSigmaL->Location = Point(110, 56);
        lblSigmaL->Width = 15; lblSigmaL->Visible = false;
        grpL->Controls->Add(lblSigmaL);

        txtSigmaL = gcnew TextBox();
        txtSigmaL->Text = L"1";
        txtSigmaL->Location = Point(128, 53); txtSigmaL->Width = 70;
        txtSigmaL->Visible = false;
        grpL->Controls->Add(txtSigmaL);

        // Правое ГУ
        GroupBox^ grpR = gcnew GroupBox();
        grpR->Text = L"Правое граничное условие при x = 1";
        grpR->Location = Point(420, 38);
        grpR->Size = System::Drawing::Size(400, 85);
        top->Controls->Add(grpR);

        Label^ lblTR = gcnew Label();
        lblTR->Text = L"Тип:"; lblTR->Location = Point(10, 25); lblTR->Width = 35;
        grpR->Controls->Add(lblTR);

        cmbBcR = gcnew ComboBox();
        cmbBcR->DropDownStyle = ComboBoxStyle::DropDownList;
        cmbBcR->Items->Add(L"Дирихле:  u(1) = γ");
        cmbBcR->Items->Add(L"Нейман:    k(1)·u'(1) = γ");
        cmbBcR->Items->Add(L"Робин:     k(1)·u'(1) + σ·u(1) = γ");
        cmbBcR->SelectedIndex = 0;
        cmbBcR->Location = Point(50, 22);
        cmbBcR->Width = 270;
        cmbBcR->SelectedIndexChanged += gcnew EventHandler(this, &MainForm::OnBcRightChanged);
        grpR->Controls->Add(cmbBcR);

        Label^ lblGR = gcnew Label();
        lblGR->Text = L"γ:"; lblGR->Location = Point(10, 56); lblGR->Width = 15;
        grpR->Controls->Add(lblGR);

        txtGammaR = gcnew TextBox();
        txtGammaR->Text = L"1";
        txtGammaR->Location = Point(28, 53); txtGammaR->Width = 70;
        grpR->Controls->Add(txtGammaR);

        lblSigmaR = gcnew Label();
        lblSigmaR->Text = L"σ:"; lblSigmaR->Location = Point(110, 56);
        lblSigmaR->Width = 15; lblSigmaR->Visible = false;
        grpR->Controls->Add(lblSigmaR);

        txtSigmaR = gcnew TextBox();
        txtSigmaR->Text = L"1";
        txtSigmaR->Location = Point(128, 53); txtSigmaR->Width = 70;
        txtSigmaR->Visible = false;
        grpR->Controls->Add(txtSigmaR);

        // Кнопки
        btnSolveTest = gcnew Button();
        btnSolveTest->Text = L"Решить тестовую";
        btnSolveTest->Location = Point(840, 40);
        btnSolveTest->Size = System::Drawing::Size(180, 30);
        btnSolveTest->Click += gcnew EventHandler(this, &MainForm::OnSolveTest);
        top->Controls->Add(btnSolveTest);

        btnSolveMain = gcnew Button();
        btnSolveMain->Text = L"Решить основную";
        btnSolveMain->Location = Point(1030, 40);
        btnSolveMain->Size = System::Drawing::Size(180, 30);
        btnSolveMain->Click += gcnew EventHandler(this, &MainForm::OnSolveMain);
        top->Controls->Add(btnSolveMain);

        btnConv = gcnew Button();
        btnConv->Text = L"Проверка порядка";
        btnConv->Location = Point(840, 76);
        btnConv->Size = System::Drawing::Size(180, 30);
        btnConv->Click += gcnew EventHandler(this, &MainForm::OnConvergence);
        top->Controls->Add(btnConv);

        btnWide = gcnew Button();
        btnWide->Text = L"Широкий диапазон n";
        btnWide->Location = Point(1030, 76);
        btnWide->Size = System::Drawing::Size(180, 30);
        btnWide->Click += gcnew EventHandler(this, &MainForm::OnWideRange);
        top->Controls->Add(btnWide);

        // Вкладки
        tabs = gcnew TabControl();
        tabs->Dock = DockStyle::Fill;
        this->Controls->Add(tabs);
        tabs->BringToFront();

        BuildTestTab();
        BuildMainTab();
        BuildConvergenceTab();
        BuildWideRangeTab();
    }

    Chart^ MakeChart(String^ title) {
        Chart^ ch = gcnew Chart();
        ChartArea^ ca = gcnew ChartArea(L"area");
        ca->AxisX->Title = L"x";
        ca->AxisY->Title = L"u";
        ca->AxisX->MajorGrid->LineColor = Color::LightGray;
        ca->AxisY->MajorGrid->LineColor = Color::LightGray;
        ca->AxisX->LabelStyle->Format = L"0.##";
        ch->ChartAreas->Add(ca);
        Title^ t = gcnew Title(title);
        t->Font = gcnew System::Drawing::Font(L"Segoe UI", 10, FontStyle::Bold);
        ch->Titles->Add(t);
        Legend^ lg = gcnew Legend(L"legend");
        lg->Docking = Docking::Bottom;
        ch->Legends->Add(lg);
        return ch;
    }

    DataGridView^ MakeDgv(array<String^>^ headers) {
        DataGridView^ dgv = gcnew DataGridView();
        dgv->ReadOnly = true;
        dgv->AllowUserToAddRows = false;
        dgv->AllowUserToResizeRows = false;
        dgv->RowHeadersVisible = false;
        dgv->ColumnCount = headers->Length;
        for (int i = 0; i < headers->Length; ++i) {
            dgv->Columns[i]->HeaderText = headers[i];
            dgv->Columns[i]->AutoSizeMode = DataGridViewAutoSizeColumnMode::Fill;
            dgv->Columns[i]->DefaultCellStyle->Font = gcnew System::Drawing::Font(L"Consolas", 9);
        }
        return dgv;
    }

    void BuildTestTab() {
        TabPage^ tp = gcnew TabPage(L"Тестовая задача");
        tabs->TabPages->Add(tp);

        SplitContainer^ sc = gcnew SplitContainer();
        sc->Dock = DockStyle::Fill;
        sc->Orientation = Orientation::Horizontal;
        tp->Controls->Add(sc);
        sc->SplitterDistance = 90;

        lblTestInfo = gcnew Label();
        lblTestInfo->Dock = DockStyle::Fill;
        lblTestInfo->Text = L"Нажмите «Решить тестовую» для расчёта.";
        lblTestInfo->Font = gcnew System::Drawing::Font(L"Consolas", 9);
        lblTestInfo->TextAlign = ContentAlignment::TopLeft;
        sc->Panel1->Controls->Add(lblTestInfo);

        SplitContainer^ sc2 = gcnew SplitContainer();
        sc2->Dock = DockStyle::Fill;
        sc2->Orientation = Orientation::Vertical;
        sc->Panel2->Controls->Add(sc2);

        SplitContainer^ scCharts = gcnew SplitContainer();
        scCharts->Dock = DockStyle::Fill;
        scCharts->Orientation = Orientation::Horizontal;
        sc2->Panel1->Controls->Add(scCharts);

        chTestSols = MakeChart(L"u(x) аналитическое и v(x) численное");
        chTestSols->Dock = DockStyle::Fill;
        scCharts->Panel1->Controls->Add(chTestSols);

        chTestErr = MakeChart(L"Погрешность u(xᵢ) − v(xᵢ)");
        chTestErr->Dock = DockStyle::Fill;
        scCharts->Panel2->Controls->Add(chTestErr);

        array<String^>^ heads = { L"i", L"xᵢ", L"u(xᵢ)", L"v(xᵢ)", L"u − v" };
        dgvTest = MakeDgv(heads);
        dgvTest->Dock = DockStyle::Fill;
        sc2->Panel2->Controls->Add(dgvTest);
    }

    void BuildMainTab() {
        TabPage^ tp = gcnew TabPage(L"Основная задача");
        tabs->TabPages->Add(tp);

        SplitContainer^ sc = gcnew SplitContainer();
        sc->Dock = DockStyle::Fill;
        sc->Orientation = Orientation::Horizontal;
        tp->Controls->Add(sc);
        sc->SplitterDistance = 90;

        lblMainInfo = gcnew Label();
        lblMainInfo->Dock = DockStyle::Fill;
        lblMainInfo->Text = L"Нажмите «Решить основную» для расчёта.";
        lblMainInfo->Font = gcnew System::Drawing::Font(L"Consolas", 9);
        lblMainInfo->TextAlign = ContentAlignment::TopLeft;
        sc->Panel1->Controls->Add(lblMainInfo);

        SplitContainer^ sc2 = gcnew SplitContainer();
        sc2->Dock = DockStyle::Fill;
        sc2->Orientation = Orientation::Vertical;
        sc->Panel2->Controls->Add(sc2);

        SplitContainer^ scCharts = gcnew SplitContainer();
        scCharts->Dock = DockStyle::Fill;
        scCharts->Orientation = Orientation::Horizontal;
        sc2->Panel1->Controls->Add(scCharts);

        chMainSols = MakeChart(L"v(x), n   и   v₂(x), 2n");
        chMainSols->Dock = DockStyle::Fill;
        scCharts->Panel1->Controls->Add(chMainSols);

        chMainErr = MakeChart(L"Разность v(xᵢ) − v₂(x₂ᵢ)");
        chMainErr->Dock = DockStyle::Fill;
        scCharts->Panel2->Controls->Add(chMainErr);

        array<String^>^ heads = { L"i", L"xᵢ", L"v(xᵢ)", L"v₂(x₂ᵢ)", L"v − v₂" };
        dgvMain = MakeDgv(heads);
        dgvMain->Dock = DockStyle::Fill;
        sc2->Panel2->Controls->Add(dgvMain);
    }

    void BuildConvergenceTab() {
        TabPage^ tp = gcnew TabPage(L"Проверка порядка");
        tabs->TabPages->Add(tp);

        Label^ lbl = gcnew Label();
        lbl->Text = L"Тестовая задача: ε₁ = max|u−v|.   Основная задача: ε₂ = max|v − v₂|.\n"
                    L"При удвоении n ожидается отношение ε(n)/ε(2n) ≈ 4 (порядок сходимости 2).";
        lbl->Dock = DockStyle::Top;
        lbl->Height = 40;
        lbl->Padding = System::Windows::Forms::Padding(8, 4, 4, 4);
        tp->Controls->Add(lbl);

        SplitContainer^ sc = gcnew SplitContainer();
        sc->Dock = DockStyle::Fill;
        sc->Orientation = Orientation::Vertical;
        tp->Controls->Add(sc);
        sc->BringToFront();
        lbl->BringToFront();

        Label^ ll = gcnew Label();
        ll->Text = L"Тестовая задача (ε₁)";
        ll->Dock = DockStyle::Top;
        ll->Height = 22;
        ll->Padding = System::Windows::Forms::Padding(4);
        ll->Font = gcnew System::Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);
        sc->Panel1->Controls->Add(ll);

        array<String^>^ h1 = { L"n", L"ε₁", L"ε₁(n)/ε₁(2n)" };
        dgvConvTest = MakeDgv(h1);
        dgvConvTest->Dock = DockStyle::Fill;
        sc->Panel1->Controls->Add(dgvConvTest);
        ll->BringToFront();

        Label^ lr = gcnew Label();
        lr->Text = L"Основная задача (ε₂, удвоение шага)";
        lr->Dock = DockStyle::Top;
        lr->Height = 22;
        lr->Padding = System::Windows::Forms::Padding(4);
        lr->Font = gcnew System::Drawing::Font(L"Segoe UI", 9, FontStyle::Bold);
        sc->Panel2->Controls->Add(lr);

        array<String^>^ h2 = { L"n", L"ε₂", L"ε₂(n)/ε₂(2n)" };
        dgvConvMain = MakeDgv(h2);
        dgvConvMain->Dock = DockStyle::Fill;
        sc->Panel2->Controls->Add(dgvConvMain);
        lr->BringToFront();
    }

    void BuildWideRangeTab() {
        TabPage^ tp = gcnew TabPage(L"Широкий диапазон n");
        tabs->TabPages->Add(tp);

        Label^ lbl = gcnew Label();
        lbl->Text = L"Таблица 6: погрешности при n = 10, 20, 50, …, 1 000 000.\n"
                    L"Для построения графиков сходимости используйте столбцы −lg(ε₁) и −lg(ε₂) в зависимости от lg(n).";
        lbl->Dock = DockStyle::Top;
        lbl->Height = 40;
        lbl->Padding = System::Windows::Forms::Padding(8, 4, 4, 4);
        tp->Controls->Add(lbl);

        array<String^>^ heads = { L"n", L"ε₁ (тест)", L"ε₂ (осн.)", L"−lg ε₁", L"−lg ε₂" };
        dgvWide = MakeDgv(heads);
        dgvWide->Dock = DockStyle::Fill;
        tp->Controls->Add(dgvWide);
        dgvWide->BringToFront();
        lbl->BringToFront();
    }

    // ============================================================
    //                   ОБРАБОТЧИКИ СОБЫТИЙ
    // ============================================================
    void OnBcLeftChanged(Object^, EventArgs^) {
        bool isRobin = (cmbBcL->SelectedIndex == 2);
        lblSigmaL->Visible = isRobin;
        txtSigmaL->Visible = isRobin;
    }
    void OnBcRightChanged(Object^, EventArgs^) {
        bool isRobin = (cmbBcR->SelectedIndex == 2);
        lblSigmaR->Visible = isRobin;
        txtSigmaR->Visible = isRobin;
    }

    static double ParseDouble(String^ s, double dflt) {
        if (s == nullptr || s->Length == 0) return dflt;
        try {
            return Double::Parse(s->Replace(",", "."),
                System::Globalization::CultureInfo::InvariantCulture);
        } catch (...) { return dflt; }
    }

    Numerics::BoundaryCondition GetBcL() {
        Numerics::BoundaryCondition bc;
        bc.type  = (Numerics::BCType)cmbBcL->SelectedIndex;
        bc.gamma = ParseDouble(txtGammaL->Text, 0.0);
        bc.sigma = ParseDouble(txtSigmaL->Text, 0.0);
        return bc;
    }
    Numerics::BoundaryCondition GetBcR() {
        Numerics::BoundaryCondition bc;
        bc.type  = (Numerics::BCType)cmbBcR->SelectedIndex;
        bc.gamma = ParseDouble(txtGammaR->Text, 0.0);
        bc.sigma = ParseDouble(txtSigmaR->Text, 0.0);
        return bc;
    }

    // ---------- Решение тестовой задачи ----------
    void OnSolveTest(Object^, EventArgs^) {
        try {
            int n = (int)numN->Value;
            auto bcL = GetBcL();
            auto bcR = GetBcR();

            this->Cursor = Cursors::WaitCursor;

            auto problem = Numerics::variant1_test();
            auto v = Numerics::solve_bvp(problem, n, bcL, bcR);
            auto u = Numerics::analytical_test_solution(bcL, bcR);

            const double h = 1.0 / n;
            std::vector<double> x(n+1), uVals(n+1), errs(n+1);
            double maxErr = 0.0; int idxMax = 0;
            for (int i = 0; i <= n; ++i) {
                x[i] = i * h;
                uVals[i] = u(x[i]);
                errs[i]  = uVals[i] - v[i];
                if (std::fabs(errs[i]) > maxErr) {
                    maxErr = std::fabs(errs[i]);
                    idxMax = i;
                }
            }

            std::ostringstream oss;
            oss << "СПРАВКА (тестовая задача):\n";
            oss << "Для решения задачи использована равномерная сетка с числом разбиений n = " << n
                << " (число узлов " << (n+1) << "), h = 1/" << n << ".\n";
            oss << "Заданная точность ε = 0.5e-06.\n";
            oss.precision(4); oss << std::scientific;
            oss << "Достигнутая погрешность ε₁ = max|u(xᵢ) - v(xᵢ)| = " << maxErr << ".\n";
            oss.precision(6); oss << std::fixed;
            oss << "Максимум достигается в узле x = " << x[idxMax]
                << " (i = " << idxMax << ").\n";
            oss << "Статус: " << (maxErr <= 0.5e-6 ? "ε₁ ≤ ε ✓ (точность достигнута)"
                                                  : "ε₁ > ε — увеличьте n");
            lblTestInfo->Text = stdToCli(oss.str());

            FillSolutionsChart(chTestSols, x, uVals, v, L"u(x) аналит.", L"v(x) числен.");
            FillErrorChart(chTestErr, x, errs, L"u − v");
            FillTable(dgvTest, x, uVals, v, errs);
        } catch (std::exception& e) {
            MessageBox::Show(stdToCli(e.what()), L"Ошибка");
        } catch (Exception^ ex) {
            MessageBox::Show(ex->Message, L"Ошибка");
        }
        this->Cursor = Cursors::Default;
    }

    // ---------- Решение основной задачи ----------
    void OnSolveMain(Object^, EventArgs^) {
        try {
            int n = (int)numN->Value;
            if (2*n > 2000000) {
                MessageBox::Show(L"Для оценки погрешности нужен расчёт на сетке 2n. "
                                 L"Слишком большой n.", L"Внимание");
                return;
            }
            auto bcL = GetBcL();
            auto bcR = GetBcR();

            this->Cursor = Cursors::WaitCursor;

            auto problem = Numerics::variant1_main();
            auto v  = Numerics::solve_bvp(problem, n,    bcL, bcR);
            auto v2 = Numerics::solve_bvp(problem, 2*n,  bcL, bcR);

            const double h = 1.0 / n;
            std::vector<double> x(n+1), v2Common(n+1), diffs(n+1);
            double maxDiff = 0.0; int idxMax = 0;
            for (int i = 0; i <= n; ++i) {
                x[i] = i * h;
                v2Common[i] = v2[2*i];
                diffs[i] = v[i] - v2Common[i];
                if (std::fabs(diffs[i]) > maxDiff) {
                    maxDiff = std::fabs(diffs[i]);
                    idxMax = i;
                }
            }

            std::ostringstream oss;
            oss << "СПРАВКА (основная задача):\n";
            oss << "Для решения задачи использована равномерная сетка с числом разбиений n = " << n
                << " (число узлов " << (n+1) << "), h = 1/" << n << ".\n";
            oss << "Контрольная сетка: 2n = " << (2*n) << ".\n";
            oss << "Заданная точность ε = 0.5e-06.\n";
            oss.precision(4); oss << std::scientific;
            oss << "Достигнутая точность ε₂ = max|v(xᵢ) − v₂(x₂ᵢ)| = " << maxDiff << ".\n";
            oss.precision(6); oss << std::fixed;
            oss << "Максимум разности — в узле x = " << x[idxMax]
                << " (i = " << idxMax << ").\n";
            oss << "Статус: " << (maxDiff <= 0.5e-6 ? "ε₂ ≤ ε ✓ (точность достигнута)"
                                                   : "ε₂ > ε — увеличьте n");
            lblMainInfo->Text = stdToCli(oss.str());

            FillSolutionsChart(chMainSols, x, v, v2Common, L"v(x), n", L"v₂(x), 2n");
            FillErrorChart(chMainErr, x, diffs, L"v − v₂");
            FillTable(dgvMain, x, v, v2Common, diffs);
        } catch (std::exception& e) {
            MessageBox::Show(stdToCli(e.what()), L"Ошибка");
        } catch (Exception^ ex) {
            MessageBox::Show(ex->Message, L"Ошибка");
        }
        this->Cursor = Cursors::Default;
    }

    // ---------- Проверка порядка сходимости ----------
    void OnConvergence(Object^, EventArgs^) {
        try {
            auto bcL = GetBcL();
            auto bcR = GetBcR();
            this->Cursor = Cursors::WaitCursor;

            const int ns[] = {10, 20, 40, 80, 160, 320};
            const int K = (int)(sizeof(ns)/sizeof(ns[0]));

            auto problemTest = Numerics::variant1_test();
            auto problemMain = Numerics::variant1_main();
            std::vector<double> e1(K), e2(K);

            for (int k = 0; k < K; ++k) {
                int n = ns[k];

                auto v = Numerics::solve_bvp(problemTest, n, bcL, bcR);
                auto u = Numerics::analytical_test_solution(bcL, bcR);
                double h = 1.0 / n;
                double me = 0.0;
                for (int i = 0; i <= n; ++i)
                    me = std::max(me, std::fabs(u(i*h) - v[i]));
                e1[k] = me;

                auto vM  = Numerics::solve_bvp(problemMain, n,   bcL, bcR);
                auto v2M = Numerics::solve_bvp(problemMain, 2*n, bcL, bcR);
                double md = 0.0;
                for (int i = 0; i <= n; ++i)
                    md = std::max(md, std::fabs(vM[i] - v2M[2*i]));
                e2[k] = md;
            }

            FillConvDgv(dgvConvTest, ns, e1, K);
            FillConvDgv(dgvConvMain, ns, e2, K);
            tabs->SelectedIndex = 2;
        } catch (Exception^ ex) {
            MessageBox::Show(ex->Message, L"Ошибка");
        }
        this->Cursor = Cursors::Default;
    }

    void FillConvDgv(DataGridView^ dgv, const int* ns, const std::vector<double>& e, int K) {
        dgv->Rows->Clear();
        for (int k = 0; k < K; ++k) {
            String^ ratio = (k > 0 && e[k] > 0.0)
                ? FmtFixed(e[k-1] / e[k], 3)
                : L"—";
            dgv->Rows->Add(ns[k].ToString(), FmtSci(e[k]), ratio);
        }
    }

    // ---------- Широкий диапазон n ----------
    void OnWideRange(Object^, EventArgs^) {
        try {
            auto bcL = GetBcL();
            auto bcR = GetBcR();
            this->Cursor = Cursors::WaitCursor;

            const int ns[] = {10, 20, 50, 80, 100, 200, 500, 800, 1000,
                              2000, 5000, 10000, 50000, 100000, 500000, 1000000};
            const int K = (int)(sizeof(ns)/sizeof(ns[0]));

            auto problemTest = Numerics::variant1_test();
            auto problemMain = Numerics::variant1_main();

            dgvWide->Rows->Clear();
            for (int k = 0; k < K; ++k) {
                int n = ns[k];

                auto v = Numerics::solve_bvp(problemTest, n, bcL, bcR);
                auto u = Numerics::analytical_test_solution(bcL, bcR);
                double h = 1.0 / n;
                double e1 = 0.0;
                for (int i = 0; i <= n; ++i)
                    e1 = std::max(e1, std::fabs(u(i*h) - v[i]));

                double e2 = -1.0;
                if (2 * n <= 2000000) {
                    auto vM  = Numerics::solve_bvp(problemMain, n, bcL, bcR);
                    auto v2M = Numerics::solve_bvp(problemMain, 2*n, bcL, bcR);
                    e2 = 0.0;
                    for (int i = 0; i <= n; ++i)
                        e2 = std::max(e2, std::fabs(vM[i] - v2M[2*i]));
                }

                String^ s_e2     = (e2 < 0)        ? L"—" : FmtSci(e2);
                String^ s_neg_l1 = (e1 > 0.0)      ? FmtFixed(-std::log10(e1), 3) : L"—";
                String^ s_neg_l2 = (e2 > 0.0)      ? FmtFixed(-std::log10(e2), 3) : L"—";

                dgvWide->Rows->Add(n.ToString(), FmtSci(e1), s_e2, s_neg_l1, s_neg_l2);
                Application::DoEvents(); // обновлять таблицу постепенно
            }
            tabs->SelectedIndex = 3;
        } catch (Exception^ ex) {
            MessageBox::Show(ex->Message, L"Ошибка");
        }
        this->Cursor = Cursors::Default;
    }

    // ============================================================
    //                  ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
    // ============================================================
    void FillSolutionsChart(Chart^ ch, const std::vector<double>& x,
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
        // обязательно добавим последнюю точку
        if ((N - 1) % step != 0) {
            s1->Points->AddXY(x[N-1], y1[N-1]);
            s2->Points->AddXY(x[N-1], y2[N-1]);
        }

        ch->Series->Add(s1);
        ch->Series->Add(s2);
        ch->ChartAreas[0]->RecalculateAxesScale();
    }

    void FillErrorChart(Chart^ ch, const std::vector<double>& x,
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
        ch->ChartAreas[0]->AxisY->Title = L"u − v";
        ch->ChartAreas[0]->RecalculateAxesScale();
    }

    void FillTable(DataGridView^ dgv, const std::vector<double>& x,
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
};

} // namespace BalanceMethodApp
