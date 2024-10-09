#pragma once

#include <windows.h>
#include <utility>

class Window;

class Node {
protected:
	HWND m_window = nullptr;
    Window* m_parent;
    std::pair<int, int> m_position;
    std::pair<int, int> m_size;
public:
    Node(LPCTSTR className, DWORD dwStyle, Window* parent);
    Node& setPosition(int x, int y);
    Node& setSize(int width, int height);
    Node& setTitle(LPCTSTR title);
    Node& forceResize();
    HWND getWindow();
};