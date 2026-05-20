// App.cpp — точка входа приложения.
// Лабораторная работа 3, метод баланса, вариант 7.

#include "MyForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(array<String^>^) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    Lab3::MyForm form;
    Application::Run(% form);
    return 0;
}
