#pragma once

#include <windows.h>
#include <functional>
#include "ui/Node.h"

class Button : public Node {
    std::function<void()> m_onClick = [](){};
public:
    Button(Window* parent);
    Button& setCallback(std::function<void()> callback);
    void onClick();
};