#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>

namespace CppShot {
    std::wstring GetRegistry(LPCTSTR pszValueName, LPCTSTR defaultValue);
    std::wstring GetSaveDirectory();
    const wchar_t* statusString(const Gdiplus::Status status);
    RECT getDesktopRect();
    RECT getCaptureRect(HWND window);
}