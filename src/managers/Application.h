#pragma once

#include <windows.h>

class Application {
    HINSTANCE m_instance;
public:
	static Application& get();
    void setup(HINSTANCE instance);
    HINSTANCE getInstance();
private:
    Application();
    ~Application();
    Application(const Application&);
    Application& operator=(const Application&);
};