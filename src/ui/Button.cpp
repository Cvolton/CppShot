#include "Button.h"

Button::Button(Window* parent) : Node(L"BUTTON", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, parent) {}

Button& Button::setCallback(std::function<void()> callback) {
    m_onClick = callback;
    return *this;
}

void Button::onClick() {
    m_onClick();
}