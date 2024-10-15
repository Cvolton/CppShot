#pragma once

#include <windows.h>
#include <string>

class Application {
    std::wstring m_saveDirectory;
public:
	static Application& get();
    std::wstring getSaveDirectory() const;
private:
    Application();
    ~Application() = default;
    Application(const Application&) = default;
    Application& operator=(const Application&) = default;
};