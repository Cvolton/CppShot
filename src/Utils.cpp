#include "Utils.h"
#include "managers/Application.h"

#include <iostream>
#include <tchar.h>

std::wstring CppShot::getRegistry(LPCTSTR pszValueName, LPCTSTR defaultValue)
{
    // Try open registry key
    HKEY hKey = NULL;
    LPCTSTR pszSubkey = _T("SOFTWARE\\CppShot");
    if ( RegOpenKey(HKEY_CURRENT_USER, pszSubkey, &hKey) != ERROR_SUCCESS )
    {
        std::cout << "Unable to open registry key" << std::endl;
    }

    // Buffer to store string read from registry
    TCHAR szValue[1024];
    DWORD cbValueLength = sizeof(szValue);

    // Query string value
    if ( RegQueryValueEx(
            hKey,
            pszValueName,
            NULL,
            NULL,
            reinterpret_cast<LPBYTE>(&szValue),
            &cbValueLength)
         != ERROR_SUCCESS )
    {
        std::cout << "Unable to read registry value" << std::endl;
        return std::wstring(defaultValue);
    }

    return std::wstring(szValue);
}

std::wstring CppShot::getSaveDirectory(){
    return Application::get().getSaveDirectory();
}

const wchar_t* CppShot::statusString(const Gdiplus::Status status) {
    switch (status) {
        case Gdiplus::Ok: return L"Ok";
        case Gdiplus::GenericError: return L"GenericError";
        case Gdiplus::InvalidParameter: return L"InvalidParameter";
        case Gdiplus::OutOfMemory: return L"OutOfMemory";
        case Gdiplus::ObjectBusy: return L"ObjectBusy";
        case Gdiplus::InsufficientBuffer: return L"InsufficientBuffer";
        case Gdiplus::NotImplemented: return L"NotImplemented";
        case Gdiplus::Win32Error: return L"Win32Error";
        case Gdiplus::Aborted: return L"Aborted";
        case Gdiplus::FileNotFound: return L"FileNotFound";
        case Gdiplus::ValueOverflow: return L"ValueOverflow";
        case Gdiplus::AccessDenied: return L"AccessDenied";
        case Gdiplus::UnknownImageFormat: return L"UnknownImageFormat";
        case Gdiplus::FontFamilyNotFound: return L"FontFamilyNotFound";
        case Gdiplus::FontStyleNotFound: return L"FontStyleNotFound";
        case Gdiplus::NotTrueTypeFont: return L"NotTrueTypeFont";
        case Gdiplus::UnsupportedGdiplusVersion: return L"UnsupportedGdiplusVersion";
        case Gdiplus::GdiplusNotInitialized: return L"GdiplusNotInitialized";
        case Gdiplus::PropertyNotFound: return L"PropertyNotFound";
        case Gdiplus::PropertyNotSupported: return L"PropertyNotSupported";
        default: return L"Status Type Not Found.";
    }
}

RECT CppShot::getDesktopRect() {
    RECT rctDesktop;
    rctDesktop.left = GetSystemMetrics(76);
    rctDesktop.top = GetSystemMetrics(77);
    rctDesktop.right = GetSystemMetrics(78) + rctDesktop.left;
    rctDesktop.bottom = GetSystemMetrics(79) + rctDesktop.top;
    return rctDesktop;
}

RECT CppShot::getCaptureRect(HWND window) {
    RECT rct;
    auto rctDesktop = CppShot::getDesktopRect();

    GetWindowRect(window, &rct);
    int dpi = CppShot::getDPIForWindow(window);
    int offset = 100 * dpi / 96;

    rct.left = (rctDesktop.left < (rct.left-offset)) ? (rct.left - offset) : rctDesktop.left;
    rct.right = (rctDesktop.right > (rct.right+offset)) ? (rct.right + offset) : rctDesktop.right;
    rct.bottom = (rctDesktop.bottom > (rct.bottom+offset)) ? (rct.bottom + offset) : rctDesktop.bottom;
    rct.top = (rctDesktop.top < (rct.top-offset)) ? (rct.top - offset) : rctDesktop.top;

    return rct;
}

BOOL CALLBACK CppShot::getMonitorRectsCallback(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    std::vector<RECT>* monitors = reinterpret_cast<std::vector<RECT>*>(dwData);
    monitors->push_back(*lprcMonitor);
    return TRUE;
}

std::vector<RECT> CppShot::getMonitorRects() {
    std::vector<RECT> monitors;
    EnumDisplayMonitors(NULL, NULL, &CppShot::getMonitorRectsCallback, reinterpret_cast<LPARAM>(&monitors));
    return monitors;
}

unsigned int CppShot::getDPIForWindow(HWND window) {
    //use GetDpiForWindow if available
    if (HMODULE hUser32 = GetModuleHandle(L"user32.dll")) {
        if (auto pGetDpiForWindow = reinterpret_cast<UINT(WINAPI*)(HWND)>(GetProcAddress(hUser32, "GetDpiForWindow"))) {
            return pGetDpiForWindow(window);
        }
    }

    //use GetDpiForSystem if available
    if (HMODULE hShcore = LoadLibrary(L"shcore.dll")) {
        if (auto pGetDpiForSystem = reinterpret_cast<UINT(WINAPI*)()>(GetProcAddress(hShcore, "GetDpiForSystem"))){
            return pGetDpiForSystem();
        }
    }

    //use GetDeviceCaps as a last resort
    return GetDeviceCaps(GetDC(window), LOGPIXELSX);
}