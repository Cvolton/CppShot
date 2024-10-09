#pragma once

#include <windows.h>

class Window;

class Node {
protected:
	HWND m_window = nullptr;
    Window* m_parent;
public:
    Node(LPCTSTR className, DWORD dwStyle, Window* parent);
    Node& setPosition(int x, int y);
    Node& setSize(int width, int height);
    Node& setTitle(LPCTSTR title);
    HWND getWindow();
};