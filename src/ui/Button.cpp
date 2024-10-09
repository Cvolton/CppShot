#include "Button.h"

Button::Button(Window* parent) : Node(L"BUTTON", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, parent) {}