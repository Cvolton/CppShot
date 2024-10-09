#pragma once

#include <windows.h>
#include <vector>
#include "ui/Node.h"

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

    void addChild(Node* child);

    virtual ~Window();
};