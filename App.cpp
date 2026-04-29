// App.cpp — точка входа
#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main() {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew BalanceMethodApp::MainForm());
    return 0;
}
