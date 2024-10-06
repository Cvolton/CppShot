#include "Application.h"

Application::Application() {}
Application::~Application() {}
Application::Application(const Application&) {}
Application& Application::operator=(const Application&) {}

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