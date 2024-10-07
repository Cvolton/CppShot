#include "Application.h"

Application& Application::get() {
    static Application instance;
    return instance;
}

void Application::setup(HINSTANCE instance) {
    m_instance = instance;
}

HINSTANCE Application::getInstance() {
    return m_instance;
}