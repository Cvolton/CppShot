#pragma once

#include <windows.h>

class Application {
    HINSTANCE m_instance;
public:
	static Application& get();
    void setup(HINSTANCE instance);
    HINSTANCE getInstance();
private:
    Application() = default;
    ~Application() = default;
    Application(const Application&) = default;
    Application& operator=(const Application&) = default;
};