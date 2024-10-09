#include "Node.h"
#include "windows/Window.h"

#include <commctrl.h>
#include <tchar.h>

Node::Node(LPCTSTR className, DWORD dwStyle, Window* parent) {
    m_window = CreateWindow(
        className,
        L"Default",
        dwStyle,
        10,
        10,
        500,
        100,
        parent->getWindow(),
        NULL,
        (HINSTANCE) GetWindowLongPtr(parent->getWindow(), GWLP_HINSTANCE),
        NULL
    );

    SetWindowLongPtr(m_window, GWLP_USERDATA, (LONG_PTR) this);
    m_parent = parent;
}

Node& Node::setPosition(int x, int y) {
    m_position = {x, y};
    auto scale = m_parent->getScaleFactor();
    SetWindowPos(m_window, NULL, x * scale, y * scale, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    return *this;
}

Node& Node::setSize(int width, int height) {
    m_size = {width, height};
    auto scale = m_parent->getScaleFactor();
    SetWindowPos(m_window, NULL, 0, 0, width * scale, height * scale, SWP_NOMOVE | SWP_NOZORDER);
    return *this;
}

Node& Node::setTitle(LPCTSTR title) {
    SetWindowText(m_window, title);
    return *this;
}

Node& Node::forceResize() {
    setPosition(m_position.first, m_position.second);
    setSize(m_size.first, m_size.second);
}

HWND Node::getWindow() {
    return m_window;
}