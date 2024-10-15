#include "Application.h"
#include "Utils.h"

Application& Application::get() {
    static Application instance;
    return instance;
}

Application::Application() {
    m_saveDirectory = CppShot::getRegistry(L"Path", L"C:\\test\\");
}

std::wstring Application::getSaveDirectory() const {
    return m_saveDirectory;
}