#pragma once

#include <windows.h>
#include <vector>
#include "ui/Button.h"

class Window {
protected:
	HWND m_window = nullptr;

    std::vector<Node*> m_children;
    
    virtual LRESULT handleMessage(UINT message, WPARAM wParam, LPARAM lParam);
public:
	Window(HINSTANCE hThisInstance, HBRUSH brush, const TCHAR* className, const TCHAR* title, DWORD dwExStyle = 0, DWORD dwStyle = WS_OVERLAPPEDWINDOW);
	static LRESULT CALLBACK windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    HWND getWindow();

    virtual Window& show(int nCmdShow = SW_SHOWNORMAL) const;
    virtual Window& hide() const;
    Window& setSize(int width, int height);

    void addChild(Node* child);
    Button& addButton();

    unsigned int getDPI();
    double getScaleFactor();

    virtual ~Window();
};