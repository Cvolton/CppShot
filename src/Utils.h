#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <vector>
#include <chrono>

namespace CppShot {
    std::wstring getRegistry(LPCTSTR pszValueName, LPCTSTR defaultValue);
    std::wstring getSaveDirectory();
    const wchar_t* statusString(const Gdiplus::Status status);
    RECT getDesktopRect();
    RECT getCaptureRect(HWND window);
    BOOL CALLBACK getMonitorRectsCallback(HMONITOR unnamedParam1, HDC unnamedParam2, LPRECT unnamedParam3, LPARAM unnamedParam4);
    std::vector<RECT> getMonitorRects();
    unsigned int getDPIForWindow(HWND window);

    inline unsigned __int64 currentTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
}