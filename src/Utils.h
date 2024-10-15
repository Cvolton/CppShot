#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>
#include <vector>

namespace CppShot {
    std::wstring getRegistry(LPCTSTR pszValueName, LPCTSTR defaultValue);
    std::wstring getSaveDirectory();
    const wchar_t* statusString(const Gdiplus::Status status);
    RECT getDesktopRect();
    RECT getCaptureRect(HWND window);
    std::vector<RECT> getMonitorRects();
}