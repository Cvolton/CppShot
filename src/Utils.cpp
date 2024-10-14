#include "Utils.h"

#include <iostream>
#include <tchar.h>

#define DEFAULT_SAVE_DIRECTORY L"C:\\test\\"

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

    _tprintf(_T("getregistry: %s\n"), szValue);

    return std::wstring(szValue);
}

std::wstring CppShot::getSaveDirectory(){
    return getRegistry(L"Path", DEFAULT_SAVE_DIRECTORY);
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

    rct.left = (rctDesktop.left < (rct.left-100)) ? (rct.left - 100) : rctDesktop.left;
    rct.right = (rctDesktop.right > (rct.right+100)) ? (rct.right + 100) : rctDesktop.right;
    rct.bottom = (rctDesktop.bottom > (rct.bottom+100)) ? (rct.bottom + 100) : rctDesktop.bottom;
    rct.top = (rctDesktop.top < (rct.top-100)) ? (rct.top - 100) : rctDesktop.top;

    return rct;
}