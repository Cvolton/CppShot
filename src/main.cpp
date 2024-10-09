#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <gdiplus.h>
#include <sstream>
#include <iostream>
#include <chrono>
#include <string>
#include <sys/stat.h>

#include "resources.h"
#include "images/Screenshot.h"
#include "images/CompositeScreenshot.h"
#include "windows/MainWindow.h"
#include "windows/BackdropWindow.h"

#define CPPSHOT_VERSION L"0.5 - build: " __DATE__ " " __TIME__

#define ERROR_TITLE L"CppShot Error"

#define DEFAULT_SAVE_DIRECTORY L"C:\\test\\"

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("MainCreWindow");
TCHAR blackBackdropClassName[ ] = _T("BlackBackdropWindow");
TCHAR whiteBackdropClassName[ ] = _T("WhiteBackdropWindow");

inline bool FileExists (const std::wstring& name) {
    std::string name_string(name.begin(), name.end());
    struct stat buffer;
    return (stat (name_string.c_str(), &buffer) == 0);
}

inline unsigned __int64 CurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

std::wstring GetRegistry(LPCTSTR pszValueName, LPCTSTR defaultValue)
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

std::wstring GetSaveDirectory(){
    return GetRegistry(L"Path", DEFAULT_SAVE_DIRECTORY);
}

const wchar_t* statusString(const Gdiplus::Status status) {
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

void DisplayGdiplusStatusError(const Gdiplus::Status status){
    if(status == Gdiplus::Ok)
        return;
    wchar_t errorText[2048];
    _stprintf(errorText, L"An error has occured while saving: %s", statusString(status));
    MessageBox(NULL, errorText, ERROR_TITLE, 0x40010);
}

void RemoveIllegalChars(std::wstring& str){
    std::wstring::iterator it;
    std::wstring illegalChars = L"\\/:?\"<>|*";
    for (it = str.begin() ; it < str.end() ; ++it){
        bool found = illegalChars.find(*it) != std::string::npos;
        if(found){
            *it = ' ';
        }
    }
}

std::wstring GetSafeFilenameBase(std::wstring windowTitle) {
    RemoveIllegalChars(windowTitle);

    std::wstring path = GetSaveDirectory();
    std::wcout << L"registrypath: " << path << std::endl;

    CreateDirectory(path.c_str(), NULL);

    std::wstringstream pathbuild;

    std::wstring fileNameBase;

    unsigned int i = 0;
    do {
        pathbuild.str(L"");

        pathbuild << path << L"\\" << windowTitle << L"_" << i;

        fileNameBase = pathbuild.str();

        i++;
    } while(FileExists(fileNameBase + L"_b1.png") | FileExists(fileNameBase + L"_b2.png"));

    return fileNameBase;
}

void CaptureCompositeScreenshot(HINSTANCE hThisInstance, BackdropWindow& whiteWindow, BackdropWindow& blackWindow, bool creMode){
    std::cout << "Screenshot capture start: " << CurrentTimestamp() << std::endl;

    HWND desktopWindow = GetDesktopWindow();
    HWND foregroundWindow = GetForegroundWindow();
    HWND taskbar = FindWindow(L"Shell_TrayWnd", NULL);
    HWND startButton = FindWindow(L"Button", L"Start");

    std::pair<Screenshot, Screenshot> shots;
    std::pair<Screenshot, Screenshot> creShots;

    //hiding the taskbar in case it gets in the way
    //note that this may cause issues if the program crashes during capture
    if(foregroundWindow != taskbar && foregroundWindow != startButton){
        ShowWindow(taskbar, 0);
        ShowWindow(startButton, 0);
    }

    whiteWindow.resize(foregroundWindow);
    blackWindow.resize(foregroundWindow);

    //spawning backdrop
    SetForegroundWindow(foregroundWindow);

    std::cout << "Additional white flash: " << CurrentTimestamp() << std::endl;
    
    //WaitForColor(rct, RGB(255,255,255));
    blackWindow.hide();
    whiteWindow.show();

    //taking the screenshot
    //WaitForColor(rct, RGB(0,0,0));

    std::cout << "Capturing black: " << CurrentTimestamp() << std::endl;


    whiteWindow.hide();
    blackWindow.show();
    
    shots.second.capture(foregroundWindow);

    //WaitForColor(rct, RGB(255,255,255));

    std::cout << "Capturing white: " << CurrentTimestamp() << std::endl;
    blackWindow.hide();
    whiteWindow.show();
    
    shots.first.capture(foregroundWindow);

    if(creMode){
        SetForegroundWindow(desktopWindow);
        Sleep(33); //Time for the foreground window to settle
        creShots.first.capture(foregroundWindow); //order swapped bc were starting with white now
        
        std::cout << "Capturing black inactive: " << CurrentTimestamp() << std::endl;
        whiteWindow.hide();
        blackWindow.show();

        creShots.second.capture(foregroundWindow);
    }

    //activating taskbar
    ShowWindow(taskbar, 1);
    ShowWindow(startButton, 1);

    //hiding backdrop
    blackWindow.hide();
    whiteWindow.hide();

    if(!shots.first.isCaptured() || !shots.second.isCaptured()){
        MessageBox(NULL, L"Screenshot is empty, aborting capture.", ERROR_TITLE, MB_OK | MB_ICONSTOP);
        return;
    }

    //differentiating alpha
    std::cout << "Starting image save: " << CurrentTimestamp() << std::endl;

    //Saving the image
    std::cout << "Saving: " << CurrentTimestamp() << std::endl;

    TCHAR h[2048];
    GetWindowText(foregroundWindow, h, 2048);
    std::wstring windowTextStr(h);
    //std::cout << std::endl << len;

    auto base = GetSafeFilenameBase(windowTextStr);

    std::cout << "Differentiating alpha: " << CurrentTimestamp() << std::endl;
    try {
        CompositeScreenshot transparentImage(shots.first, shots.second);
        transparentImage.save(base + L"_b1.png");

        if(creShots.first.isCaptured() && creShots.second.isCaptured()){
            CompositeScreenshot transparentInactiveImage(creShots.first, creShots.second, transparentImage.getCrop());
            std::cout << "Inactive image ptr: " << transparentInactiveImage.getBitmap() << std::endl;
            std::cout << transparentInactiveImage.getBitmap()->GetWidth() << "x" << transparentInactiveImage.getBitmap()->GetHeight() << std::endl;
            transparentInactiveImage.save(base + L"_b2.png");
        }
    } catch(std::runtime_error& e) {
        MessageBox(NULL, L"An error has occured while capturing the screenshot.", ERROR_TITLE, MB_OK | MB_ICONSTOP);
        return;
    }

    /*std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring fileNameUtf16 = converter.from_bytes(fileName);
    std::wstring fileNameInactiveUtf16 = converter.from_bytes(fileNameInactive);*/

    /*std::wcout << fileName << std::endl << fileNameInactive << std::endl;
    DisplayGdiplusStatusError(clonedBitmap->Save(fileName.c_str(), &pngEncoder, NULL));
    if(creMode)
        DisplayGdiplusStatusError(clonedInactive->Save(fileNameInactive.c_str(), &pngEncoder, NULL));

    std::cout << "Done: " << CurrentTimestamp() << std::endl;
    //Cleaning memory
    delete clonedBitmap;
    delete clonedInactive;*/

}

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    
    MainWindow window(hThisInstance);
    window.show(nCmdShow);

    if (RegisterHotKey(
        NULL,
        1,
        0x2,
        0x42))  //0x42 is 'b'
    {
        _tprintf(_T("CTRL+b\n"));
    }else{
        MessageBox(NULL, L"Unable to register the CTRL+B keyboard shortcut.", ERROR_TITLE, 0x10);
    }

    if (RegisterHotKey(
        NULL,
        2,
        0x6,
        0x42))  //0x42 is 'b'
    {
        _tprintf(_T("CTRL+SHIFT+b\n"));
    }else{
        MessageBox(NULL, L"Unable to register the CTRL+SHIFT+B keyboard shortcut.", ERROR_TITLE, 0x10);
    }

    /* Create backdrop windows */
    BackdropWindow whiteWindow(hThisInstance, RGB(255, 255, 255), whiteBackdropClassName);
    BackdropWindow blackWindow(hThisInstance, RGB(0, 0, 0), blackBackdropClassName);

    /* Start GDI+ */
    Gdiplus::GdiplusStartupInput gpStartupInput;
    ULONG_PTR gpToken;
    int val = Gdiplus::GdiplusStartup(&gpToken, &gpStartupInput, NULL);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    MSG messages;
    while (GetMessage (&messages, NULL, 0, 0))
    {
        if (messages.message == WM_HOTKEY)
        {
            _tprintf(_T("WM_HOTKEY received\n"));
            if (messages.wParam == 1)
                CaptureCompositeScreenshot(hThisInstance, whiteWindow, blackWindow, false);
            else if (messages.wParam == 2)
                CaptureCompositeScreenshot(hThisInstance, whiteWindow, blackWindow, true);
        }

        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

VOID StartExplorer()
{
    std::wstring path = GetSaveDirectory();
   // additional information
   STARTUPINFO si;
   PROCESS_INFORMATION pi;

   // set the size of the structures
   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

   TCHAR commandLine[2048];
   _stprintf(commandLine, L"explorer \"%s\"", path.c_str());

  // start the program up
  CreateProcess( NULL,   // the path
    commandLine,        // Command line
    NULL,           // Process handle not inheritable
    NULL,           // Thread handle not inheritable
    FALSE,          // Set handle inheritance to FALSE
    0,              // No creation flags
    NULL,           // Use parent's environment block
    NULL,           // Use parent's starting directory
    &si,            // Pointer to STARTUPINFO structure
    &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    );
    // Close process and thread handles.
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}