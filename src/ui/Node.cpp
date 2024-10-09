#include "Node.h"
#include "windows/Window.h"

#include <commctrl.h>
#include <tchar.h>

Node::Node(LPCTSTR className, DWORD dwStyle, Window& parent) {
    m_window = CreateWindow(
        className,
        L"Default",
        dwStyle,
        10,
        10,
        500,
        100,
        parent.getWindow(),
        NULL,
        (HINSTANCE) GetWindowLongPtr(parent.getWindow(), GWLP_HINSTANCE),
        NULL
    );

    SetWindowLongPtr(m_window, GWLP_USERDATA, (LONG_PTR) this);
}

Node& Node::setPosition(int x, int y) {
    SetWindowPos(m_window, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    return *this;
}


Node& Node::setSize(int width, int height) {
    SetWindowPos(m_window, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
    return *this;
}

Node& Node::setTitle(LPCTSTR title) {
    SetWindowText(m_window, title);
    return *this;
}

HWND Node::getWindow() {
    return m_window;
}