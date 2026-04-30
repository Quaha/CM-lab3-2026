#pragma once
#pragma execution_character_set("utf-8")

#include "Solver.h"

namespace Lab3 {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Windows::Forms::DataVisualization::Charting;

    /// <summary>
    /// Главная форма приложения "Метод баланса".
    /// </summary>
    public ref class MyForm : public System::Windows::Forms::Form
    {
    public:
        MyForm(void)
        {
            InitializeComponent();
            // Изначально σ-поля скрыты (Дирихле выбран по умолчанию)
            lblSigmaL->Visible = false; txtSigmaL->Visible = false;
            lblSigmaR->Visible = false; txtSigmaR->Visible = false;
        }

    protected:
        ~MyForm()
        {
            if (components) { delete components; }
        }

    // ===== Поля контролов (видны дизайнеру) =====
    private: System::ComponentModel::IContainer^ components;

    private: System::Windows::Forms::Panel^ panelTop;
    private: System::Windows::Forms::Label^ lblN;
    private: System::Windows::Forms::NumericUpDown^ numN;
    private: System::Windows::Forms::Label^ lblVar;

    private: System::Windows::Forms::GroupBox^ grpL;
    private: System::Windows::Forms::Label^ lblTL;
    private: System::Windows::Forms::ComboBox^ cmbBcL;
    private: System::Windows::Forms::Label^ lblGammaL;
    private: System::Windows::Forms::TextBox^ txtGammaL;
    private: System::Windows::Forms::Label^ lblSigmaL;
    private: System::Windows::Forms::TextBox^ txtSigmaL;

    private: System::Windows::Forms::GroupBox^ grpR;
    private: System::Windows::Forms::Label^ lblTR;
    private: System::Windows::Forms::ComboBox^ cmbBcR;
    private: System::Windows::Forms::Label^ lblGammaR;
    private: System::Windows::Forms::TextBox^ txtGammaR;
    private: System::Windows::Forms::Label^ lblSigmaR;
    private: System::Windows::Forms::TextBox^ txtSigmaR;

    private: System::Windows::Forms::Button^ btnSolveTest;
    private: System::Windows::Forms::Button^ btnSolveMain;
    private: System::Windows::Forms::Button^ btnConv;
    private: System::Windows::Forms::Button^ btnWide;

    private: System::Windows::Forms::TabControl^ tabs;

    private: System::Windows::Forms::TabPage^ tabTest;
    private: System::Windows::Forms::SplitContainer^ scTest;
    private: System::Windows::Forms::Label^ lblTestInfo;
    private: System::Windows::Forms::SplitContainer^ scTestInner;
    private: System::Windows::Forms::SplitContainer^ scTestCharts;
    private: System::Windows::Forms::DataVisualization::Charting::Chart^ chTestSols;
    private: System::Windows::Forms::DataVisualization::Charting::ChartArea^ caTestSols;
    private: System::Windows::Forms::DataVisualization::Charting::Title^ tiTestSols;
    private: System::Windows::Forms::DataVisualization::Charting::Legend^ lgTestSols;
    private: System::Windows::Forms::DataVisualization::Charting::Chart^ chTestErr;
    private: System::Windows::Forms::DataVisualization::Charting::ChartArea^ caTestErr;
    private: System::Windows::Forms::DataVisualization::Charting::Title^ tiTestErr;
    private: System::Windows::Forms::DataVisualization::Charting::Legend^ lgTestErr;
    private: System::Windows::Forms::DataGridView^ dgvTest;

    private: System::Windows::Forms::TabPage^ tabMain;
    private: System::Windows::Forms::SplitContainer^ scMain;
    private: System::Windows::Forms::Label^ lblMainInfo;
    private: System::Windows::Forms::SplitContainer^ scMainInner;
    private: System::Windows::Forms::SplitContainer^ scMainCharts;
    private: System::Windows::Forms::DataVisualization::Charting::Chart^ chMainSols;
    private: System::Windows::Forms::DataVisualization::Charting::ChartArea^ caMainSols;
    private: System::Windows::Forms::DataVisualization::Charting::Title^ tiMainSols;
    private: System::Windows::Forms::DataVisualization::Charting::Legend^ lgMainSols;
    private: System::Windows::Forms::DataVisualization::Charting::Chart^ chMainErr;
    private: System::Windows::Forms::DataVisualization::Charting::ChartArea^ caMainErr;
    private: System::Windows::Forms::DataVisualization::Charting::Title^ tiMainErr;
    private: System::Windows::Forms::DataVisualization::Charting::Legend^ lgMainErr;
    private: System::Windows::Forms::DataGridView^ dgvMain;

    private: System::Windows::Forms::TabPage^ tabConv;
    private: System::Windows::Forms::Label^ lblConvHelp;
    private: System::Windows::Forms::SplitContainer^ scConv;
    private: System::Windows::Forms::Label^ lblConvTest;
    private: System::Windows::Forms::DataGridView^ dgvConvTest;
    private: System::Windows::Forms::Label^ lblConvMain;
    private: System::Windows::Forms::DataGridView^ dgvConvMain;

    private: System::Windows::Forms::TabPage^ tabWide;
    private: System::Windows::Forms::Label^ lblWideHelp;
    private: System::Windows::Forms::DataGridView^ dgvWide;

#pragma region Windows Form Designer generated code
        void InitializeComponent(void)
        {
            this->panelTop = (gcnew System::Windows::Forms::Panel());
            this->lblN = (gcnew System::Windows::Forms::Label());
            this->numN = (gcnew System::Windows::Forms::NumericUpDown());
            this->lblVar = (gcnew System::Windows::Forms::Label());
            this->grpL = (gcnew System::Windows::Forms::GroupBox());
            this->lblTL = (gcnew System::Windows::Forms::Label());
            this->cmbBcL = (gcnew System::Windows::Forms::ComboBox());
            this->lblGammaL = (gcnew System::Windows::Forms::Label());
            this->txtGammaL = (gcnew System::Windows::Forms::TextBox());
            this->lblSigmaL = (gcnew System::Windows::Forms::Label());
            this->txtSigmaL = (gcnew System::Windows::Forms::TextBox());
            this->grpR = (gcnew System::Windows::Forms::GroupBox());
            this->lblTR = (gcnew System::Windows::Forms::Label());
            this->cmbBcR = (gcnew System::Windows::Forms::ComboBox());
            this->lblGammaR = (gcnew System::Windows::Forms::Label());
            this->txtGammaR = (gcnew System::Windows::Forms::TextBox());
            this->lblSigmaR = (gcnew System::Windows::Forms::Label());
            this->txtSigmaR = (gcnew System::Windows::Forms::TextBox());
            this->btnSolveTest = (gcnew System::Windows::Forms::Button());
            this->btnSolveMain = (gcnew System::Windows::Forms::Button());
            this->btnConv = (gcnew System::Windows::Forms::Button());
            this->btnWide = (gcnew System::Windows::Forms::Button());
            this->tabs = (gcnew System::Windows::Forms::TabControl());

            this->tabTest = (gcnew System::Windows::Forms::TabPage());
            this->scTest = (gcnew System::Windows::Forms::SplitContainer());
            this->lblTestInfo = (gcnew System::Windows::Forms::Label());
            this->scTestInner = (gcnew System::Windows::Forms::SplitContainer());
            this->scTestCharts = (gcnew System::Windows::Forms::SplitContainer());
            this->chTestSols = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
            this->chTestErr  = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
            this->dgvTest = (gcnew System::Windows::Forms::DataGridView());

            this->tabMain = (gcnew System::Windows::Forms::TabPage());
            this->scMain = (gcnew System::Windows::Forms::SplitContainer());
            this->lblMainInfo = (gcnew System::Windows::Forms::Label());
            this->scMainInner = (gcnew System::Windows::Forms::SplitContainer());
            this->scMainCharts = (gcnew System::Windows::Forms::SplitContainer());
            this->chMainSols = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
            this->chMainErr  = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
            this->dgvMain = (gcnew System::Windows::Forms::DataGridView());

            this->tabConv = (gcnew System::Windows::Forms::TabPage());
            this->lblConvHelp = (gcnew System::Windows::Forms::Label());
            this->scConv = (gcnew System::Windows::Forms::SplitContainer());
            this->lblConvTest = (gcnew System::Windows::Forms::Label());
            this->dgvConvTest = (gcnew System::Windows::Forms::DataGridView());
            this->lblConvMain = (gcnew System::Windows::Forms::Label());
            this->dgvConvMain = (gcnew System::Windows::Forms::DataGridView());

            this->tabWide = (gcnew System::Windows::Forms::TabPage());
            this->lblWideHelp = (gcnew System::Windows::Forms::Label());
            this->dgvWide = (gcnew System::Windows::Forms::DataGridView());

            this->panelTop->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numN))->BeginInit();
            this->grpL->SuspendLayout();
            this->grpR->SuspendLayout();
            this->tabs->SuspendLayout();
            this->tabTest->SuspendLayout();
            this->scTest->Panel1->SuspendLayout(); this->scTest->Panel2->SuspendLayout(); this->scTest->SuspendLayout();
            this->scTestInner->Panel1->SuspendLayout(); this->scTestInner->Panel2->SuspendLayout(); this->scTestInner->SuspendLayout();
            this->scTestCharts->Panel1->SuspendLayout(); this->scTestCharts->Panel2->SuspendLayout(); this->scTestCharts->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chTestSols))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chTestErr))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvTest))->BeginInit();

            this->tabMain->SuspendLayout();
            this->scMain->Panel1->SuspendLayout(); this->scMain->Panel2->SuspendLayout(); this->scMain->SuspendLayout();
            this->scMainInner->Panel1->SuspendLayout(); this->scMainInner->Panel2->SuspendLayout(); this->scMainInner->SuspendLayout();
            this->scMainCharts->Panel1->SuspendLayout(); this->scMainCharts->Panel2->SuspendLayout(); this->scMainCharts->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chMainSols))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chMainErr))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvMain))->BeginInit();

            this->tabConv->SuspendLayout();
            this->scConv->Panel1->SuspendLayout(); this->scConv->Panel2->SuspendLayout(); this->scConv->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvConvTest))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvConvMain))->BeginInit();

            this->tabWide->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvWide))->BeginInit();

            this->SuspendLayout();

            // panelTop
            this->panelTop->BackColor = System::Drawing::Color::FromArgb(245, 245, 248);
            this->panelTop->Controls->Add(this->btnWide);
            this->panelTop->Controls->Add(this->btnConv);
            this->panelTop->Controls->Add(this->btnSolveMain);
            this->panelTop->Controls->Add(this->btnSolveTest);
            this->panelTop->Controls->Add(this->grpR);
            this->panelTop->Controls->Add(this->grpL);
            this->panelTop->Controls->Add(this->lblVar);
            this->panelTop->Controls->Add(this->numN);
            this->panelTop->Controls->Add(this->lblN);
            this->panelTop->Dock = System::Windows::Forms::DockStyle::Top;
            this->panelTop->Location = System::Drawing::Point(0, 0);
            this->panelTop->Name = L"panelTop";
            this->panelTop->Size = System::Drawing::Size(1280, 130);
            this->panelTop->TabIndex = 0;

            // lblN
            this->lblN->AutoSize = true;
            this->lblN->Location = System::Drawing::Point(10, 12);
            this->lblN->Name = L"lblN";
            this->lblN->Size = System::Drawing::Size(125, 15);
            this->lblN->Text = L"Число разбиений n:";

            // numN
            this->numN->Location = System::Drawing::Point(140, 10);
            this->numN->Name = L"numN";
            this->numN->Size = System::Drawing::Size(100, 23);
            this->numN->Minimum = System::Decimal(gcnew cli::array<System::Int32>(4) {4, 0, 0, 0});
            this->numN->Maximum = System::Decimal(gcnew cli::array<System::Int32>(4) {1000000, 0, 0, 0});
            this->numN->Value   = System::Decimal(gcnew cli::array<System::Int32>(4) {100, 0, 0, 0});

            // lblVar
            this->lblVar->AutoSize = true;
            this->lblVar->ForeColor = System::Drawing::Color::FromArgb(20, 60, 130);
            this->lblVar->Location = System::Drawing::Point(260, 12);
            this->lblVar->Name = L"lblVar";
            this->lblVar->Size = System::Drawing::Size(700, 15);
            this->lblVar->Text = L"Вариант 1: ξ = 0.4, k₁=x+1, k₂=x, q₁=x, q₂=x², f₁=x, f₂=exp(-x), μ₁=0, μ₂=1";

            // grpL
            this->grpL->Controls->Add(this->txtSigmaL);
            this->grpL->Controls->Add(this->lblSigmaL);
            this->grpL->Controls->Add(this->txtGammaL);
            this->grpL->Controls->Add(this->lblGammaL);
            this->grpL->Controls->Add(this->cmbBcL);
            this->grpL->Controls->Add(this->lblTL);
            this->grpL->Location = System::Drawing::Point(10, 38);
            this->grpL->Name = L"grpL";
            this->grpL->Size = System::Drawing::Size(400, 85);
            this->grpL->TabStop = false;
            this->grpL->Text = L"Левое граничное условие при x = 0";

            // lblTL
            this->lblTL->AutoSize = true;
            this->lblTL->Location = System::Drawing::Point(10, 25);
            this->lblTL->Size = System::Drawing::Size(35, 15);
            this->lblTL->Text = L"Тип:";

            // cmbBcL
            this->cmbBcL->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->cmbBcL->FormattingEnabled = true;
            this->cmbBcL->Items->AddRange(gcnew cli::array<System::Object^>(3) {
                L"1-го рода:  u(0) = γ",
                L"2-го рода:  −k(0)·u'(0) = γ",
                L"3-го рода:  −k(0)·u'(0) + σ·u(0) = γ"
            });
            this->cmbBcL->Location = System::Drawing::Point(50, 22);
            this->cmbBcL->Name = L"cmbBcL";
            this->cmbBcL->Size = System::Drawing::Size(330, 23);
            this->cmbBcL->SelectedIndex = 0;
            this->cmbBcL->SelectedIndexChanged +=
                gcnew System::EventHandler(this, &MyForm::cmbBcL_SelectedIndexChanged);

            // lblGammaL
            this->lblGammaL->AutoSize = true;
            this->lblGammaL->Location = System::Drawing::Point(10, 56);
            this->lblGammaL->Size = System::Drawing::Size(15, 15);
            this->lblGammaL->Text = L"γ:";

            // txtGammaL
            this->txtGammaL->Location = System::Drawing::Point(28, 53);
            this->txtGammaL->Name = L"txtGammaL";
            this->txtGammaL->Size = System::Drawing::Size(70, 23);
            this->txtGammaL->Text = L"0";

            // lblSigmaL
            this->lblSigmaL->AutoSize = true;
            this->lblSigmaL->Location = System::Drawing::Point(110, 56);
            this->lblSigmaL->Size = System::Drawing::Size(15, 15);
            this->lblSigmaL->Text = L"σ:";

            // txtSigmaL
            this->txtSigmaL->Location = System::Drawing::Point(128, 53);
            this->txtSigmaL->Name = L"txtSigmaL";
            this->txtSigmaL->Size = System::Drawing::Size(70, 23);
            this->txtSigmaL->Text = L"1";

            // grpR
            this->grpR->Controls->Add(this->txtSigmaR);
            this->grpR->Controls->Add(this->lblSigmaR);
            this->grpR->Controls->Add(this->txtGammaR);
            this->grpR->Controls->Add(this->lblGammaR);
            this->grpR->Controls->Add(this->cmbBcR);
            this->grpR->Controls->Add(this->lblTR);
            this->grpR->Location = System::Drawing::Point(420, 38);
            this->grpR->Name = L"grpR";
            this->grpR->Size = System::Drawing::Size(400, 85);
            this->grpR->TabStop = false;
            this->grpR->Text = L"Правое граничное условие при x = 1";

            // lblTR
            this->lblTR->AutoSize = true;
            this->lblTR->Location = System::Drawing::Point(10, 25);
            this->lblTR->Size = System::Drawing::Size(35, 15);
            this->lblTR->Text = L"Тип:";

            // cmbBcR
            this->cmbBcR->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
            this->cmbBcR->FormattingEnabled = true;
            this->cmbBcR->Items->AddRange(gcnew cli::array<System::Object^>(3) {
                L"1-го рода:  u(1) = γ",
                L"2-го рода:   k(1)·u'(1) = γ",
                L"3-го рода:   k(1)·u'(1) + σ·u(1) = γ"
            });
            this->cmbBcR->Location = System::Drawing::Point(50, 22);
            this->cmbBcR->Name = L"cmbBcR";
            this->cmbBcR->Size = System::Drawing::Size(330, 23);
            this->cmbBcR->SelectedIndex = 0;
            this->cmbBcR->SelectedIndexChanged +=
                gcnew System::EventHandler(this, &MyForm::cmbBcR_SelectedIndexChanged);

            // lblGammaR
            this->lblGammaR->AutoSize = true;
            this->lblGammaR->Location = System::Drawing::Point(10, 56);
            this->lblGammaR->Size = System::Drawing::Size(15, 15);
            this->lblGammaR->Text = L"γ:";

            // txtGammaR
            this->txtGammaR->Location = System::Drawing::Point(28, 53);
            this->txtGammaR->Name = L"txtGammaR";
            this->txtGammaR->Size = System::Drawing::Size(70, 23);
            this->txtGammaR->Text = L"1";

            // lblSigmaR
            this->lblSigmaR->AutoSize = true;
            this->lblSigmaR->Location = System::Drawing::Point(110, 56);
            this->lblSigmaR->Size = System::Drawing::Size(15, 15);
            this->lblSigmaR->Text = L"σ:";

            // txtSigmaR
            this->txtSigmaR->Location = System::Drawing::Point(128, 53);
            this->txtSigmaR->Name = L"txtSigmaR";
            this->txtSigmaR->Size = System::Drawing::Size(70, 23);
            this->txtSigmaR->Text = L"1";

            // btnSolveTest
            this->btnSolveTest->Location = System::Drawing::Point(840, 40);
            this->btnSolveTest->Name = L"btnSolveTest";
            this->btnSolveTest->Size = System::Drawing::Size(180, 30);
            this->btnSolveTest->Text = L"Решить тестовую";
            this->btnSolveTest->UseVisualStyleBackColor = true;
            this->btnSolveTest->Click += gcnew System::EventHandler(this, &MyForm::btnSolveTest_Click);

            // btnSolveMain
            this->btnSolveMain->Location = System::Drawing::Point(1030, 40);
            this->btnSolveMain->Name = L"btnSolveMain";
            this->btnSolveMain->Size = System::Drawing::Size(180, 30);
            this->btnSolveMain->Text = L"Решить основную";
            this->btnSolveMain->UseVisualStyleBackColor = true;
            this->btnSolveMain->Click += gcnew System::EventHandler(this, &MyForm::btnSolveMain_Click);

            // btnConv
            this->btnConv->Location = System::Drawing::Point(840, 76);
            this->btnConv->Name = L"btnConv";
            this->btnConv->Size = System::Drawing::Size(180, 30);
            this->btnConv->Text = L"Проверка порядка";
            this->btnConv->UseVisualStyleBackColor = true;
            this->btnConv->Click += gcnew System::EventHandler(this, &MyForm::btnConv_Click);

            // btnWide
            this->btnWide->Location = System::Drawing::Point(1030, 76);
            this->btnWide->Name = L"btnWide";
            this->btnWide->Size = System::Drawing::Size(180, 30);
            this->btnWide->Text = L"Широкий диапазон n";
            this->btnWide->UseVisualStyleBackColor = true;
            this->btnWide->Click += gcnew System::EventHandler(this, &MyForm::btnWide_Click);

            // tabs
            this->tabs->Controls->Add(this->tabTest);
            this->tabs->Controls->Add(this->tabMain);
            this->tabs->Controls->Add(this->tabConv);
            this->tabs->Controls->Add(this->tabWide);
            this->tabs->Dock = System::Windows::Forms::DockStyle::Fill;
            this->tabs->Location = System::Drawing::Point(0, 130);
            this->tabs->Name = L"tabs";
            this->tabs->SelectedIndex = 0;
            this->tabs->Size = System::Drawing::Size(1280, 670);

            // ====== Вкладка 1: Тестовая ======
            this->tabTest->Controls->Add(this->scTest);
            this->tabTest->Location = System::Drawing::Point(4, 24);
            this->tabTest->Name = L"tabTest";
            this->tabTest->Padding = System::Windows::Forms::Padding(3);
            this->tabTest->Size = System::Drawing::Size(1272, 642);
            this->tabTest->Text = L"Тестовая задача";
            this->tabTest->UseVisualStyleBackColor = true;

            this->scTest->Dock = System::Windows::Forms::DockStyle::Fill;
            this->scTest->Orientation = System::Windows::Forms::Orientation::Horizontal;
            this->scTest->Panel1->Controls->Add(this->lblTestInfo);
            this->scTest->Panel2->Controls->Add(this->scTestInner);
            this->scTest->Size = System::Drawing::Size(1266, 636);
            this->scTest->SplitterDistance = 90;

            this->lblTestInfo->Dock = System::Windows::Forms::DockStyle::Fill;
            this->lblTestInfo->Font = (gcnew System::Drawing::Font(L"Consolas", 9));
            this->lblTestInfo->Text = L"Нажмите «Решить тестовую» для расчёта.";
            this->lblTestInfo->TextAlign = System::Drawing::ContentAlignment::TopLeft;

            this->scTestInner->Dock = System::Windows::Forms::DockStyle::Fill;
            this->scTestInner->Orientation = System::Windows::Forms::Orientation::Vertical;
            this->scTestInner->Panel1->Controls->Add(this->scTestCharts);
            this->scTestInner->Panel2->Controls->Add(this->dgvTest);

            this->scTestCharts->Dock = System::Windows::Forms::DockStyle::Fill;
            this->scTestCharts->Orientation = System::Windows::Forms::Orientation::Horizontal;
            this->scTestCharts->Panel1->Controls->Add(this->chTestSols);
            this->scTestCharts->Panel2->Controls->Add(this->chTestErr);

            this->caTestSols = gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea();
            this->caTestSols->Name = L"area";
            this->chTestSols->ChartAreas->Add(this->caTestSols);
            this->tiTestSols = gcnew System::Windows::Forms::DataVisualization::Charting::Title();
            this->tiTestSols->Name = L"Title1";
            this->tiTestSols->Text = L"u(x) аналит. и v(x) числен.";
            this->chTestSols->Titles->Add(this->tiTestSols);
            this->lgTestSols = gcnew System::Windows::Forms::DataVisualization::Charting::Legend();
            this->lgTestSols->Name = L"legend";
            this->chTestSols->Legends->Add(this->lgTestSols);
            this->chTestSols->Dock = System::Windows::Forms::DockStyle::Fill;
            this->chTestSols->Name = L"chTestSols";

            this->caTestErr = gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea();
            this->caTestErr->Name = L"area";
            this->chTestErr->ChartAreas->Add(this->caTestErr);
            this->tiTestErr = gcnew System::Windows::Forms::DataVisualization::Charting::Title();
            this->tiTestErr->Name = L"Title1";
            this->tiTestErr->Text = L"Погрешность u − v";
            this->chTestErr->Titles->Add(this->tiTestErr);
            this->lgTestErr = gcnew System::Windows::Forms::DataVisualization::Charting::Legend();
            this->lgTestErr->Name = L"legend";
            this->chTestErr->Legends->Add(this->lgTestErr);
            this->chTestErr->Dock = System::Windows::Forms::DockStyle::Fill;
            this->chTestErr->Name = L"chTestErr";

            this->dgvTest->AllowUserToAddRows = false;
            this->dgvTest->AllowUserToResizeRows = false;
            this->dgvTest->ColumnHeadersHeightSizeMode =
                System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            this->dgvTest->Dock = System::Windows::Forms::DockStyle::Fill;
            this->dgvTest->Name = L"dgvTest";
            this->dgvTest->ReadOnly = true;
            this->dgvTest->RowHeadersVisible = false;

            // ====== Вкладка 2: Основная ======
            this->tabMain->Controls->Add(this->scMain);
            this->tabMain->Location = System::Drawing::Point(4, 24);
            this->tabMain->Name = L"tabMain";
            this->tabMain->Padding = System::Windows::Forms::Padding(3);
            this->tabMain->Size = System::Drawing::Size(1272, 642);
            this->tabMain->Text = L"Основная задача";
            this->tabMain->UseVisualStyleBackColor = true;

            this->scMain->Dock = System::Windows::Forms::DockStyle::Fill;
            this->scMain->Orientation = System::Windows::Forms::Orientation::Horizontal;
            this->scMain->Panel1->Controls->Add(this->lblMainInfo);
            this->scMain->Panel2->Controls->Add(this->scMainInner);
            this->scMain->SplitterDistance = 90;

            this->lblMainInfo->Dock = System::Windows::Forms::DockStyle::Fill;
            this->lblMainInfo->Font = (gcnew System::Drawing::Font(L"Consolas", 9));
            this->lblMainInfo->Text = L"Нажмите «Решить основную» для расчёта.";
            this->lblMainInfo->TextAlign = System::Drawing::ContentAlignment::TopLeft;

            this->scMainInner->Dock = System::Windows::Forms::DockStyle::Fill;
            this->scMainInner->Orientation = System::Windows::Forms::Orientation::Vertical;
            this->scMainInner->Panel1->Controls->Add(this->scMainCharts);
            this->scMainInner->Panel2->Controls->Add(this->dgvMain);

            this->scMainCharts->Dock = System::Windows::Forms::DockStyle::Fill;
            this->scMainCharts->Orientation = System::Windows::Forms::Orientation::Horizontal;
            this->scMainCharts->Panel1->Controls->Add(this->chMainSols);
            this->scMainCharts->Panel2->Controls->Add(this->chMainErr);

            this->caMainSols = gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea();
            this->caMainSols->Name = L"area";
            this->chMainSols->ChartAreas->Add(this->caMainSols);
            this->tiMainSols = gcnew System::Windows::Forms::DataVisualization::Charting::Title();
            this->tiMainSols->Name = L"Title1";
            this->tiMainSols->Text = L"v(x), n  и  v₂(x), 2n";
            this->chMainSols->Titles->Add(this->tiMainSols);
            this->lgMainSols = gcnew System::Windows::Forms::DataVisualization::Charting::Legend();
            this->lgMainSols->Name = L"legend";
            this->chMainSols->Legends->Add(this->lgMainSols);
            this->chMainSols->Dock = System::Windows::Forms::DockStyle::Fill;
            this->chMainSols->Name = L"chMainSols";

            this->caMainErr = gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea();
            this->caMainErr->Name = L"area";
            this->chMainErr->ChartAreas->Add(this->caMainErr);
            this->tiMainErr = gcnew System::Windows::Forms::DataVisualization::Charting::Title();
            this->tiMainErr->Name = L"Title1";
            this->tiMainErr->Text = L"Разность v − v₂";
            this->chMainErr->Titles->Add(this->tiMainErr);
            this->lgMainErr = gcnew System::Windows::Forms::DataVisualization::Charting::Legend();
            this->lgMainErr->Name = L"legend";
            this->chMainErr->Legends->Add(this->lgMainErr);
            this->chMainErr->Dock = System::Windows::Forms::DockStyle::Fill;
            this->chMainErr->Name = L"chMainErr";

            this->dgvMain->AllowUserToAddRows = false;
            this->dgvMain->AllowUserToResizeRows = false;
            this->dgvMain->ColumnHeadersHeightSizeMode =
                System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            this->dgvMain->Dock = System::Windows::Forms::DockStyle::Fill;
            this->dgvMain->Name = L"dgvMain";
            this->dgvMain->ReadOnly = true;
            this->dgvMain->RowHeadersVisible = false;

            // ====== Вкладка 3: Проверка порядка ======
            this->tabConv->Controls->Add(this->scConv);
            this->tabConv->Controls->Add(this->lblConvHelp);
            this->tabConv->Location = System::Drawing::Point(4, 24);
            this->tabConv->Name = L"tabConv";
            this->tabConv->Size = System::Drawing::Size(1272, 642);
            this->tabConv->Text = L"Проверка порядка";
            this->tabConv->UseVisualStyleBackColor = true;

            this->lblConvHelp->Dock = System::Windows::Forms::DockStyle::Top;
            this->lblConvHelp->Height = 40;
            this->lblConvHelp->Padding = System::Windows::Forms::Padding(8, 4, 4, 4);
            this->lblConvHelp->Text = L"Тестовая: ε₁ = max|u−v|.   Основная: ε₂ = max|v − v₂|.\r\n"
                                      L"При удвоении n отношение ε(n)/ε(2n) ≈ 4 (порядок 2).";

            this->scConv->Dock = System::Windows::Forms::DockStyle::Fill;
            this->scConv->Orientation = System::Windows::Forms::Orientation::Vertical;

            this->scConv->Panel1->Controls->Add(this->dgvConvTest);
            this->scConv->Panel1->Controls->Add(this->lblConvTest);
            this->lblConvTest->Dock = System::Windows::Forms::DockStyle::Top;
            this->lblConvTest->Height = 22;
            this->lblConvTest->Padding = System::Windows::Forms::Padding(4);
            this->lblConvTest->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9, System::Drawing::FontStyle::Bold));
            this->lblConvTest->Text = L"Тестовая задача (ε₁)";
            this->dgvConvTest->AllowUserToAddRows = false;
            this->dgvConvTest->Dock = System::Windows::Forms::DockStyle::Fill;
            this->dgvConvTest->Name = L"dgvConvTest";
            this->dgvConvTest->ReadOnly = true;
            this->dgvConvTest->RowHeadersVisible = false;

            this->scConv->Panel2->Controls->Add(this->dgvConvMain);
            this->scConv->Panel2->Controls->Add(this->lblConvMain);
            this->lblConvMain->Dock = System::Windows::Forms::DockStyle::Top;
            this->lblConvMain->Height = 22;
            this->lblConvMain->Padding = System::Windows::Forms::Padding(4);
            this->lblConvMain->Font = (gcnew System::Drawing::Font(L"Segoe UI", 9, System::Drawing::FontStyle::Bold));
            this->lblConvMain->Text = L"Основная задача (ε₂, удвоение шага)";
            this->dgvConvMain->AllowUserToAddRows = false;
            this->dgvConvMain->Dock = System::Windows::Forms::DockStyle::Fill;
            this->dgvConvMain->Name = L"dgvConvMain";
            this->dgvConvMain->ReadOnly = true;
            this->dgvConvMain->RowHeadersVisible = false;

            // ====== Вкладка 4: Широкий диапазон ======
            this->tabWide->Controls->Add(this->dgvWide);
            this->tabWide->Controls->Add(this->lblWideHelp);
            this->tabWide->Location = System::Drawing::Point(4, 24);
            this->tabWide->Name = L"tabWide";
            this->tabWide->Size = System::Drawing::Size(1272, 642);
            this->tabWide->Text = L"Широкий диапазон n";
            this->tabWide->UseVisualStyleBackColor = true;

            this->lblWideHelp->Dock = System::Windows::Forms::DockStyle::Top;
            this->lblWideHelp->Height = 40;
            this->lblWideHelp->Padding = System::Windows::Forms::Padding(8, 4, 4, 4);
            this->lblWideHelp->Text = L"Таблица 6: погрешности при n от 10 до 1 000 000.\r\n"
                                      L"Колонки −lg(ε₁), −lg(ε₂) для графиков сходимости.";

            this->dgvWide->AllowUserToAddRows = false;
            this->dgvWide->Dock = System::Windows::Forms::DockStyle::Fill;
            this->dgvWide->Name = L"dgvWide";
            this->dgvWide->ReadOnly = true;
            this->dgvWide->RowHeadersVisible = false;

            // ====== Форма ======
            this->AutoScaleDimensions = System::Drawing::SizeF(7, 15);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(1280, 800);
            this->Controls->Add(this->tabs);
            this->Controls->Add(this->panelTop);
            this->MinimumSize = System::Drawing::Size(1024, 600);
            this->Name = L"MyForm";
            this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
            this->Text = L"Лабораторная 2 — Метод баланса (вариант 1)";

            this->panelTop->ResumeLayout(false);
            this->panelTop->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numN))->EndInit();
            this->grpL->ResumeLayout(false); this->grpL->PerformLayout();
            this->grpR->ResumeLayout(false); this->grpR->PerformLayout();
            this->tabs->ResumeLayout(false);

            this->scTestCharts->Panel1->ResumeLayout(false); this->scTestCharts->Panel2->ResumeLayout(false); this->scTestCharts->ResumeLayout(false);
            this->scTestInner->Panel1->ResumeLayout(false); this->scTestInner->Panel2->ResumeLayout(false); this->scTestInner->ResumeLayout(false);
            this->scTest->Panel1->ResumeLayout(false); this->scTest->Panel2->ResumeLayout(false); this->scTest->ResumeLayout(false);
            this->tabTest->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chTestSols))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chTestErr))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvTest))->EndInit();

            this->scMainCharts->Panel1->ResumeLayout(false); this->scMainCharts->Panel2->ResumeLayout(false); this->scMainCharts->ResumeLayout(false);
            this->scMainInner->Panel1->ResumeLayout(false); this->scMainInner->Panel2->ResumeLayout(false); this->scMainInner->ResumeLayout(false);
            this->scMain->Panel1->ResumeLayout(false); this->scMain->Panel2->ResumeLayout(false); this->scMain->ResumeLayout(false);
            this->tabMain->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chMainSols))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chMainErr))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvMain))->EndInit();

            this->scConv->Panel1->ResumeLayout(false); this->scConv->Panel2->ResumeLayout(false); this->scConv->ResumeLayout(false);
            this->tabConv->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvConvTest))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvConvMain))->EndInit();

            this->tabWide->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvWide))->EndInit();

            this->ResumeLayout(false);
        }
#pragma endregion

    // ===== Объявления обработчиков (реализация в MyForm.cpp) =====
    private:
        System::Void cmbBcL_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
        System::Void cmbBcR_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e);
        System::Void btnSolveTest_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnSolveMain_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnConv_Click(System::Object^ sender, System::EventArgs^ e);
        System::Void btnWide_Click(System::Object^ sender, System::EventArgs^ e);
    };
}
