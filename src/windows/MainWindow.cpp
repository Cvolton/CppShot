#include "MainWindow.h"
#include "ui/Button.h"

#include <tchar.h>

MainWindow::MainWindow(HINSTANCE hThisInstance) : Window(hThisInstance, (HBRUSH) (COLOR_BTNFACE + 1), L"MainCreWindow", L"CppShot") {
    // Create a button
    Node* button = new Button(this);
    button->setPosition(10, 10)
          .setSize(500, 100)
          .setTitle(L"Press CTRL+B to take a screenshot");

    addChild(button);

    // Create a second button
    Node* buttonTwo = new Button(this);
    buttonTwo->setPosition(10, 120)
             .setSize(500, 100)
             .setTitle(L"Or you can press CTRL+SHIFT+B to take inactive and active screenshots");

    addChild(buttonTwo);
    setSize(544, 375);
}