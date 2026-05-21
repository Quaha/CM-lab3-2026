#pragma once
#pragma execution_character_set("utf-8")

// Примечание: нативный заголовок Solver.h сюда НЕ включается намеренно —
// это держит файл формы «чистым» CLI-кодом. Численное ядро подключается
// только в MyForm.cpp. Helper-методы ниже возвращают int-коды, которые
// в MyForm.cpp преобразуются в перечисления BalanceSolver.

namespace Lab3 {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Drawing;
    using namespace System::Windows::Forms::DataVisualization::Charting;

    /// <summary>
    /// Главная форма приложения. Лабораторная работа 3, метод баланса, вариант 7.
    ///
    /// Три селектора:
    ///   – Постановка задачи: 1-я краевая / 3-я краевая
    ///   – Тип задачи:        тестовая  / основная
    ///   – Схема ГУ:          классическая / улучшенная (только для 3-й краевой)
    ///
    /// Три действия:
    ///   – «Решить»                 — рассчитать выбранную конфигурацию,
    ///                                 показать справку, графики, таблицу.
    ///   – «Проверка порядка»       — рассчитать сетки n = 10..320 и оценить
    ///                                 порядок сходимости (таблицы 4 и 5
    ///                                 бланка отчёта).
    ///   – «Широкий диапазон n»     — заполнить таблицу 6 (n = 10..1 000 000)
    ///                                 для построения графиков сходимости.
    /// </summary>
    public ref class MyForm : public System::Windows::Forms::Form
    {
    public:
        MyForm(void)
        {
            InitializeComponent();

            // По умолчанию выбрана 1-я краевая — селектор схемы недоступен,
            // поскольку 1-я краевая задаёт u в граничных узлах точно.
            UpdateSchemeEnabled();
        }

    protected:
        ~MyForm() { if (components) delete components; }

    private:
        System::ComponentModel::IContainer^ components;

        // ---- Верхняя панель ----
        System::Windows::Forms::Panel^         panelTop;
        System::Windows::Forms::Label^         lblTitle;
        System::Windows::Forms::Label^         lblVariant;

        System::Windows::Forms::Label^         lblN;
        System::Windows::Forms::NumericUpDown^ numN;

        System::Windows::Forms::GroupBox^      grpKind;
        System::Windows::Forms::RadioButton^   rbKindFirst;
        System::Windows::Forms::RadioButton^   rbKindThird;

        System::Windows::Forms::GroupBox^      grpType;
        System::Windows::Forms::RadioButton^   rbTypeTest;
        System::Windows::Forms::RadioButton^   rbTypeMain;

        System::Windows::Forms::GroupBox^      grpScheme;
        System::Windows::Forms::RadioButton^   rbSchemeClassical;
        System::Windows::Forms::RadioButton^   rbSchemeImproved;

        System::Windows::Forms::Button^        btnSolve;
        System::Windows::Forms::Button^        btnConv;

        // ---- Вкладки ----
        System::Windows::Forms::TabControl^    tabs;

        // Вкладка «Решение»
        System::Windows::Forms::TabPage^       tabSolve;
        System::Windows::Forms::SplitContainer^ scSolve;
        System::Windows::Forms::Label^         lblInfo;
        System::Windows::Forms::SplitContainer^ scSolveBody;
        System::Windows::Forms::SplitContainer^ scCharts;
        System::Windows::Forms::DataVisualization::Charting::Chart^         chSols;
        System::Windows::Forms::DataVisualization::Charting::ChartArea^     caSols;
        System::Windows::Forms::DataVisualization::Charting::Title^         tiSols;
        System::Windows::Forms::DataVisualization::Charting::Legend^        lgSols;
        System::Windows::Forms::DataVisualization::Charting::Chart^         chErr;
        System::Windows::Forms::DataVisualization::Charting::ChartArea^     caErr;
        System::Windows::Forms::DataVisualization::Charting::Title^         tiErr;
        System::Windows::Forms::DataVisualization::Charting::Legend^        lgErr;
        System::Windows::Forms::DataGridView^  dgvSolve;

        // Вкладка «Проверка порядка»
        System::Windows::Forms::TabPage^       tabConv;
        System::Windows::Forms::Label^         lblConvHelp;
        System::Windows::Forms::DataGridView^  dgvConv;

        // Вкладка «Широкий диапазон n»

#pragma region Designer-generated layout

        void InitializeComponent(void)
        {
            this->components         = nullptr;

            this->panelTop           = gcnew System::Windows::Forms::Panel();
            this->lblTitle           = gcnew System::Windows::Forms::Label();
            this->lblVariant         = gcnew System::Windows::Forms::Label();
            this->lblN               = gcnew System::Windows::Forms::Label();
            this->numN               = gcnew System::Windows::Forms::NumericUpDown();
            this->grpKind            = gcnew System::Windows::Forms::GroupBox();
            this->rbKindFirst        = gcnew System::Windows::Forms::RadioButton();
            this->rbKindThird        = gcnew System::Windows::Forms::RadioButton();
            this->grpType            = gcnew System::Windows::Forms::GroupBox();
            this->rbTypeTest         = gcnew System::Windows::Forms::RadioButton();
            this->rbTypeMain         = gcnew System::Windows::Forms::RadioButton();
            this->grpScheme          = gcnew System::Windows::Forms::GroupBox();
            this->rbSchemeClassical  = gcnew System::Windows::Forms::RadioButton();
            this->rbSchemeImproved   = gcnew System::Windows::Forms::RadioButton();
            this->btnSolve           = gcnew System::Windows::Forms::Button();
            this->btnConv            = gcnew System::Windows::Forms::Button();

            this->tabs               = gcnew System::Windows::Forms::TabControl();

            this->tabSolve           = gcnew System::Windows::Forms::TabPage();
            this->scSolve            = gcnew System::Windows::Forms::SplitContainer();
            this->lblInfo            = gcnew System::Windows::Forms::Label();
            this->scSolveBody        = gcnew System::Windows::Forms::SplitContainer();
            this->scCharts           = gcnew System::Windows::Forms::SplitContainer();
            this->chSols             = gcnew System::Windows::Forms::DataVisualization::Charting::Chart();
            this->chErr              = gcnew System::Windows::Forms::DataVisualization::Charting::Chart();
            this->dgvSolve           = gcnew System::Windows::Forms::DataGridView();

            this->tabConv            = gcnew System::Windows::Forms::TabPage();
            this->lblConvHelp        = gcnew System::Windows::Forms::Label();
            this->dgvConv            = gcnew System::Windows::Forms::DataGridView();


            this->panelTop->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numN))->BeginInit();
            this->grpKind->SuspendLayout();
            this->grpType->SuspendLayout();
            this->grpScheme->SuspendLayout();
            this->tabs->SuspendLayout();
            this->tabSolve->SuspendLayout();
            this->scSolve->Panel1->SuspendLayout();  this->scSolve->Panel2->SuspendLayout();  this->scSolve->SuspendLayout();
            this->scSolveBody->Panel1->SuspendLayout(); this->scSolveBody->Panel2->SuspendLayout(); this->scSolveBody->SuspendLayout();
            this->scCharts->Panel1->SuspendLayout(); this->scCharts->Panel2->SuspendLayout(); this->scCharts->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chSols))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chErr))->BeginInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvSolve))->BeginInit();
            this->tabConv->SuspendLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvConv))->BeginInit();
            this->SuspendLayout();

            // ===== Верхняя панель =====
            this->panelTop->BackColor = System::Drawing::Color::FromArgb(245, 245, 248);
            this->panelTop->Controls->Add(this->btnConv);
            this->panelTop->Controls->Add(this->btnSolve);
            this->panelTop->Controls->Add(this->grpScheme);
            this->panelTop->Controls->Add(this->grpType);
            this->panelTop->Controls->Add(this->grpKind);
            this->panelTop->Controls->Add(this->numN);
            this->panelTop->Controls->Add(this->lblN);
            this->panelTop->Controls->Add(this->lblVariant);
            this->panelTop->Controls->Add(this->lblTitle);
            this->panelTop->Dock     = System::Windows::Forms::DockStyle::Top;
            this->panelTop->Location = System::Drawing::Point(0, 0);
            this->panelTop->Name     = L"panelTop";
            this->panelTop->Size     = System::Drawing::Size(1280, 170);
            this->panelTop->TabIndex = 0;

            // ---- lblTitle, lblVariant ----
            this->lblTitle->Name      = L"lblTitle";
            this->lblTitle->AutoSize  = true;
            this->lblTitle->Font      = gcnew System::Drawing::Font(L"Segoe UI Semibold", 10);
            this->lblTitle->ForeColor = System::Drawing::Color::FromArgb(20, 60, 130);
            this->lblTitle->Location  = System::Drawing::Point(10, 8);
            this->lblTitle->Text      = L"Лабораторная работа 3. Метод баланса. Вариант 7.";

            this->lblVariant->Name     = L"lblVariant";
            this->lblVariant->AutoSize = true;
            this->lblVariant->Location = System::Drawing::Point(10, 32);
            this->lblVariant->Text     =
                L"ξ = π/4,  k₁ = √2·cos x,  k₂ = 2,  q₁ = x,  q₂ = x²,  f₁ = sin 2x,  f₂ = sin x,  "
                L"μ₁ = μ₂ = 1,  γ₁ = 4, γ₂ = 3,  θ₁ = 2, θ₂ = 1.";

            // ---- lblN, numN ----
            this->lblN->Name     = L"lblN";
            this->lblN->AutoSize = true;
            this->lblN->Location = System::Drawing::Point(10, 60);
            this->lblN->Text     = L"Число разбиений n:";

            this->numN->Location = System::Drawing::Point(140, 56);
            this->numN->Name     = L"numN";
            this->numN->Size     = System::Drawing::Size(100, 23);
            this->numN->Minimum  = System::Decimal(2);
            this->numN->Maximum  = System::Decimal(1000000);
            this->numN->Value    = System::Decimal(100);

            // ---- grpKind: 1-я / 3-я краевая ----
            this->grpKind->Controls->Add(this->rbKindThird);
            this->grpKind->Controls->Add(this->rbKindFirst);
            this->grpKind->Location = System::Drawing::Point(10, 90);
            this->grpKind->Name     = L"grpKind";
            this->grpKind->Size     = System::Drawing::Size(280, 70);
            this->grpKind->TabStop  = false;
            this->grpKind->Text     = L"Постановка задачи";

            this->rbKindFirst->AutoSize = true;
            this->rbKindFirst->Checked  = true;
            this->rbKindFirst->Location = System::Drawing::Point(15, 22);
            this->rbKindFirst->Name     = L"rbKindFirst";
            this->rbKindFirst->Text     = L"1-я краевая";
            this->rbKindFirst->TabIndex = 0;
            this->rbKindFirst->CheckedChanged +=
                gcnew EventHandler(this, &MyForm::rbKind_CheckedChanged);

            this->rbKindThird->AutoSize = true;
            this->rbKindThird->Location = System::Drawing::Point(15, 44);
            this->rbKindThird->Name     = L"rbKindThird";
            this->rbKindThird->Text     = L"3-я краевая";
            this->rbKindThird->TabIndex = 1;
            this->rbKindThird->CheckedChanged +=
                gcnew EventHandler(this, &MyForm::rbKind_CheckedChanged);

            // ---- grpType: тестовая / основная ----
            this->grpType->Controls->Add(this->rbTypeMain);
            this->grpType->Controls->Add(this->rbTypeTest);
            this->grpType->Location = System::Drawing::Point(300, 90);
            this->grpType->Name     = L"grpType";
            this->grpType->Size     = System::Drawing::Size(280, 70);
            this->grpType->TabStop  = false;
            this->grpType->Text     = L"Тип задачи";

            this->rbTypeTest->AutoSize = true;
            this->rbTypeTest->Checked  = true;
            this->rbTypeTest->Location = System::Drawing::Point(15, 22);
            this->rbTypeTest->Name     = L"rbTypeTest";
            this->rbTypeTest->Text     = L"Тестовая";
            this->rbTypeTest->TabIndex = 0;

            this->rbTypeMain->AutoSize = true;
            this->rbTypeMain->Location = System::Drawing::Point(15, 44);
            this->rbTypeMain->Name     = L"rbTypeMain";
            this->rbTypeMain->Text     = L"Основная";
            this->rbTypeMain->TabIndex = 1;

            // ---- grpScheme: классическая / улучшенная ----
            this->grpScheme->Controls->Add(this->rbSchemeImproved);
            this->grpScheme->Controls->Add(this->rbSchemeClassical);
            this->grpScheme->Location = System::Drawing::Point(590, 90);
            this->grpScheme->Name     = L"grpScheme";
            this->grpScheme->Size     = System::Drawing::Size(320, 70);
            this->grpScheme->TabStop  = false;
            this->grpScheme->Text     = L"Схема аппроксимации ГУ (для 3-й краевой)";

            this->rbSchemeClassical->AutoSize = true;
            this->rbSchemeClassical->Checked  = true;
            this->rbSchemeClassical->Location = System::Drawing::Point(15, 22);
            this->rbSchemeClassical->Name     = L"rbSchemeClassical";
            this->rbSchemeClassical->Text     = L"Классическая";

            this->rbSchemeImproved->AutoSize  = true;
            this->rbSchemeImproved->Location  = System::Drawing::Point(15, 44);
            this->rbSchemeImproved->Name      = L"rbSchemeImproved";
            this->rbSchemeImproved->Text      = L"Улучшенная";

            // ---- Кнопки ----
            this->btnSolve->Location = System::Drawing::Point(930, 92);
            this->btnSolve->Name     = L"btnSolve";
            this->btnSolve->Size     = System::Drawing::Size(160, 28);
            this->btnSolve->Text     = L"Решить";
            this->btnSolve->UseVisualStyleBackColor = true;
            this->btnSolve->Click   += gcnew EventHandler(this, &MyForm::btnSolve_Click);

            this->btnConv->Location  = System::Drawing::Point(1100, 92);
            this->btnConv->Name      = L"btnConv";
            this->btnConv->Size      = System::Drawing::Size(160, 28);
            this->btnConv->Text      = L"Проверка порядка";
            this->btnConv->UseVisualStyleBackColor = true;
            this->btnConv->Click    += gcnew EventHandler(this, &MyForm::btnConv_Click);

            // ===== TabControl =====
            this->tabs->Controls->Add(this->tabSolve);
            this->tabs->Controls->Add(this->tabConv);
            this->tabs->Dock          = System::Windows::Forms::DockStyle::Fill;
            this->tabs->Location      = System::Drawing::Point(0, 170);
            this->tabs->Name          = L"tabs";
            this->tabs->SelectedIndex = 0;
            this->tabs->Size          = System::Drawing::Size(1280, 630);

            // ===== Вкладка «Решение» =====
            this->tabSolve->Controls->Add(this->scSolve);
            this->tabSolve->Location  = System::Drawing::Point(4, 24);
            this->tabSolve->Name      = L"tabSolve";
            this->tabSolve->Padding   = System::Windows::Forms::Padding(3);
            this->tabSolve->Size      = System::Drawing::Size(1272, 602);
            this->tabSolve->Text      = L"Решение";
            this->tabSolve->UseVisualStyleBackColor = true;

            this->scSolve->Dock        = System::Windows::Forms::DockStyle::Fill;
            this->scSolve->Orientation = System::Windows::Forms::Orientation::Horizontal;
            this->scSolve->Panel1->Controls->Add(this->lblInfo);
            this->scSolve->Panel2->Controls->Add(this->scSolveBody);
            this->scSolve->Size        = System::Drawing::Size(1266, 596);
            this->scSolve->SplitterDistance = 110;

            this->lblInfo->Name        = L"lblInfo";
            this->lblInfo->Dock        = System::Windows::Forms::DockStyle::Fill;
            this->lblInfo->Font        = gcnew System::Drawing::Font(L"Consolas", 9);
            this->lblInfo->Text        = L"Выберите конфигурацию и нажмите «Решить».";
            this->lblInfo->TextAlign   = System::Drawing::ContentAlignment::TopLeft;

            this->scSolveBody->Dock        = System::Windows::Forms::DockStyle::Fill;
            this->scSolveBody->Orientation = System::Windows::Forms::Orientation::Vertical;
            this->scSolveBody->Panel1->Controls->Add(this->scCharts);
            this->scSolveBody->Panel2->Controls->Add(this->dgvSolve);

            this->scCharts->Dock        = System::Windows::Forms::DockStyle::Fill;
            this->scCharts->Orientation = System::Windows::Forms::Orientation::Horizontal;
            this->scCharts->Panel1->Controls->Add(this->chSols);
            this->scCharts->Panel2->Controls->Add(this->chErr);

            this->caSols = gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea(); this->caSols->Name = L"area";
            this->chSols->ChartAreas->Add(this->caSols);
            this->tiSols = gcnew System::Windows::Forms::DataVisualization::Charting::Title();     this->tiSols->Name = L"t";   this->tiSols->Text = L"Решения";
            this->chSols->Titles->Add(this->tiSols);
            this->lgSols = gcnew System::Windows::Forms::DataVisualization::Charting::Legend();    this->lgSols->Name = L"l";
            this->chSols->Legends->Add(this->lgSols);
            this->chSols->Dock = System::Windows::Forms::DockStyle::Fill;
            this->chSols->Name = L"chSols";

            this->caErr  = gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea(); this->caErr->Name = L"area";
            this->chErr->ChartAreas->Add(this->caErr);
            this->tiErr  = gcnew System::Windows::Forms::DataVisualization::Charting::Title();     this->tiErr->Name = L"t";   this->tiErr->Text = L"Разность";
            this->chErr->Titles->Add(this->tiErr);
            this->lgErr  = gcnew System::Windows::Forms::DataVisualization::Charting::Legend();    this->lgErr->Name = L"l";
            this->chErr->Legends->Add(this->lgErr);
            this->chErr->Dock = System::Windows::Forms::DockStyle::Fill;
            this->chErr->Name = L"chErr";

            this->dgvSolve->AllowUserToAddRows     = false;
            this->dgvSolve->AllowUserToResizeRows  = false;
            this->dgvSolve->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
            this->dgvSolve->Dock                   = System::Windows::Forms::DockStyle::Fill;
            this->dgvSolve->Name                   = L"dgvSolve";
            this->dgvSolve->ReadOnly               = true;
            this->dgvSolve->RowHeadersVisible      = false;

            // ===== Вкладка «Проверка порядка» =====
            this->tabConv->Controls->Add(this->dgvConv);
            this->tabConv->Controls->Add(this->lblConvHelp);
            this->tabConv->Location  = System::Drawing::Point(4, 24);
            this->tabConv->Name      = L"tabConv";
            this->tabConv->Size      = System::Drawing::Size(1272, 602);
            this->tabConv->Text      = L"Проверка порядка";
            this->tabConv->UseVisualStyleBackColor = true;

            this->lblConvHelp->Name    = L"lblConvHelp";
            this->lblConvHelp->Dock    = System::Windows::Forms::DockStyle::Top;
            this->lblConvHelp->Height  = 60;
            this->lblConvHelp->Padding = System::Windows::Forms::Padding(8, 6, 4, 4);
            this->lblConvHelp->Font    = gcnew System::Drawing::Font(L"Consolas", 9);
            this->lblConvHelp->Text    =
                L"Тестовая ε₁ = max|u(xᵢ) − v(xᵢ)| (точное и численное); основная ε₂ = max|v(xᵢ) − v₂(x₂ᵢ)|.\r\n"
                L"При удвоении n отношение ε(n)/ε(2n) ≈ 4 для схемы 2-го порядка и ≈ 2 для 1-го.\r\n"
                L"Ряд n удваивается от 10 до значения в поле «Число разбиений n» (минимум 320).";

            this->dgvConv->AllowUserToAddRows    = false;
            this->dgvConv->Dock                  = System::Windows::Forms::DockStyle::Fill;
            this->dgvConv->Name                  = L"dgvConv";
            this->dgvConv->ReadOnly              = true;
            this->dgvConv->RowHeadersVisible     = false;


            // ===== Форма =====
            this->AutoScaleDimensions = System::Drawing::SizeF(7, 15);
            this->AutoScaleMode       = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize          = System::Drawing::Size(1280, 800);
            this->Controls->Add(this->tabs);
            this->Controls->Add(this->panelTop);
            this->MinimumSize         = System::Drawing::Size(1024, 600);
            this->Name                = L"MyForm";
            this->StartPosition       = System::Windows::Forms::FormStartPosition::CenterScreen;
            this->Text                = L"Лабораторная 3 — Метод баланса (вариант 7)";

            this->panelTop->ResumeLayout(false); this->panelTop->PerformLayout();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numN))->EndInit();
            this->grpKind->ResumeLayout(false);   this->grpKind->PerformLayout();
            this->grpType->ResumeLayout(false);   this->grpType->PerformLayout();
            this->grpScheme->ResumeLayout(false); this->grpScheme->PerformLayout();
            this->tabs->ResumeLayout(false);
            this->scCharts->Panel1->ResumeLayout(false); this->scCharts->Panel2->ResumeLayout(false); this->scCharts->ResumeLayout(false);
            this->scSolveBody->Panel1->ResumeLayout(false); this->scSolveBody->Panel2->ResumeLayout(false); this->scSolveBody->ResumeLayout(false);
            this->scSolve->Panel1->ResumeLayout(false); this->scSolve->Panel2->ResumeLayout(false); this->scSolve->ResumeLayout(false);
            this->tabSolve->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chSols))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chErr))->EndInit();
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvSolve))->EndInit();
            this->tabConv->ResumeLayout(false);
            (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dgvConv))->EndInit();
            this->ResumeLayout(false);
        }

#pragma endregion

        // ---- Helpers (реализация — в MyForm.cpp) ----
        // Возвращают int-коды, чтобы не тянуть нативные типы в заголовок формы:
        //   Kind:   0 = 1-я краевая, 1 = 3-я краевая
        //   Type:   0 = тестовая,    1 = основная
        //   Scheme: 0 = классическая, 1 = улучшенная
        void UpdateSchemeEnabled();
        int  CurrentKind();
        int  CurrentType();
        int  CurrentScheme();
        System::String^ ConfigSuffix();

        // ---- Обработчики ----
        System::Void rbKind_CheckedChanged(System::Object^ sender, System::EventArgs^ e);
        System::Void btnSolve_Click       (System::Object^ sender, System::EventArgs^ e);
        System::Void btnConv_Click        (System::Object^ sender, System::EventArgs^ e);
    };
}
