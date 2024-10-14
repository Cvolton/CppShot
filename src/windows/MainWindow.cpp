#include "MainWindow.h"
#include "ui/Button.h"

#include <tchar.h>
#include <shellapi.h>
#include "Utils.h"

MainWindow::MainWindow(HINSTANCE hThisInstance) : Window(hThisInstance, (HBRUSH) (COLOR_BTNFACE + 1), L"MainCreWindow", L"CppShot") {
    setSize(544, 375);
    
    this->addButton()
        .setCallback([](){
            MessageBox(NULL, L"Hi", L"CppShot", 0);
        })
        .setPosition(10, 10)
        .setSize(500, 100)
        .setTitle(L"Press CTRL+B to take a screenshot");

    this->addButton()
        .setPosition(10, 120)
        .setSize(500, 100)
        .setTitle(L"Or you can press CTRL+SHIFT+B to take inactive and active screenshots");

    this->addButton()
        .setCallback([this](){
            onOpenExplorer();
        })
        .setPosition(10, 300)
        .setSize(200, 30)
        .setTitle(L"Open Screenshots Folder");
}

void MainWindow::onOpenExplorer() {
    ShellExecute(NULL, L"open", L"explorer", CppShot::getSaveDirectory().c_str(), NULL, SW_SHOWNORMAL);
    
}