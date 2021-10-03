#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#define _WIN32_IE 0x0300
#undef __STRICT_ANSI__ 

#include <windows.h>
#include <commctrl.h>
//#include <wchar.h>
#include <tchar.h>
//#include <winuser.h>
//#include <stdio.h>
//#include <wingdi.h>
#include <gdiplus.h>
//#include <stdlib.h>
#include <sstream>
//#include <string>
#include <iostream>
//#include <vector>
//#include <locale>
//#include <codecvt>
#include <chrono>
#include <string>
//#include <cstring>
#include <sys/stat.h>

#include "resources.h"

#define CPPSHOT_VERSION L"0.4 - build: " __DATE__ " " __TIME__

#define ERROR_TITLE L"CppShot Error"

#define DEFAULT_SAVE_DIRECTORY L"C:\\test\\"

#define SAVE_INTERMEDIARY_IMAGES 0

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("MainCreWindow");
TCHAR blackBackdropClassName[ ] = _T("BlackBackdropWindow");
TCHAR whiteBackdropClassName[ ] = _T("WhiteBackdropWindow");
TCHAR transparentBackdropClassName[ ] = _T("TransparentBackdropWindow");

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

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

    Gdiplus::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

wchar_t* statusString(const Gdiplus::Status status) {
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

HWND createBackdropWindow(HINSTANCE hThisInstance, TCHAR className, HBRUSH backgroundBrush){
    HWND hwnd;               /* This is the handle for our window */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = &className;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = backgroundBrush;

    if (!RegisterClassEx (&wincl)){
        MessageBox(NULL, L"Unable to create backdrop window, the program may not work correctly.", ERROR_TITLE, 0x30);
        return NULL;
    }

    hwnd = CreateWindowEx (
           WS_EX_TOOLWINDOW,                   /* Extended possibilites for variation */
            &className,         /* Classname */
           _T("Backdrop Window"),       /* Title Text */
           WS_POPUP , /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    //SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);

    return hwnd;
}

void DifferentiateAlpha(Gdiplus::Bitmap* whiteShot, Gdiplus::Bitmap* blackShot, Gdiplus::Bitmap* transparentBitmap){

    Gdiplus::BitmapData transparentBitmapData;
    Gdiplus::Rect rect1(0, 0, transparentBitmap->GetWidth(), transparentBitmap->GetHeight());
    transparentBitmap->LockBits(&rect1, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &transparentBitmapData);
    BYTE* transparentPixels = (BYTE*) (void*) transparentBitmapData.Scan0;

    Gdiplus::BitmapData whiteBitmapData;
    whiteShot->LockBits(&rect1, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &whiteBitmapData);
    BYTE* whitePixels = (BYTE*) (void*) whiteBitmapData.Scan0;

    Gdiplus::BitmapData blackBitmapData;
    blackShot->LockBits(&rect1, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &blackBitmapData);
    BYTE* blackPixels = (BYTE*) (void*) blackBitmapData.Scan0;

    for(int x = 0; x < whiteShot->GetWidth(); x++){
        for(int y = 0; y < whiteShot->GetHeight(); y++){
            int currentPixel = (y*(whiteShot->GetWidth()) + x)*4;
            //Setting alpha
            int alphaPixel = ((blackPixels[currentPixel+2] - whitePixels[currentPixel+2] + 255 + blackPixels[currentPixel+1] - whitePixels[currentPixel+1] + 255 + blackPixels[currentPixel] - whitePixels[currentPixel] + 255) / 3);
            transparentPixels[currentPixel+3] = alphaPixel > 255 ? 255 : alphaPixel;
            //Setting fully transparent pixels to 0
            transparentPixels[currentPixel+2] = 0;
            transparentPixels[currentPixel+1] = 0;
            transparentPixels[currentPixel] = 0;
            if(transparentPixels[currentPixel+3] > 0){
                transparentPixels[currentPixel+2] = (255 * blackPixels[currentPixel+2] / transparentPixels[currentPixel+3]); //RED
                transparentPixels[currentPixel+1] = (255 * blackPixels[currentPixel+1] / transparentPixels[currentPixel+3]); //GREEN
                transparentPixels[currentPixel] = (255 * blackPixels[currentPixel] / transparentPixels[currentPixel+3]); //BLUE
            }
        }
    }

    transparentBitmap->UnlockBits(&transparentBitmapData);
    whiteShot->UnlockBits(&whiteBitmapData);
    blackShot->UnlockBits(&blackBitmapData);
}

HBITMAP CaptureScreenArea(RECT rct){
    HDC hdc = GetDC(HWND_DESKTOP);
    HDC memdc = CreateCompatibleDC(hdc);
    HBITMAP hbitmap = CreateCompatibleBitmap(hdc, rct.right - rct.left, rct.bottom - rct.top);
    HBITMAP oldbmp = (HBITMAP)SelectObject(memdc, hbitmap);
    BitBlt(memdc, 0, 0, rct.right - rct.left, rct.bottom - rct.top, hdc, rct.left, rct.top, SRCCOPY | CAPTUREBLT);
    SelectObject(memdc, oldbmp);

    HBITMAP hbitmapCopy = (HBITMAP) CopyImage(hbitmap, IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE);

    DeleteObject(hbitmap);
    DeleteDC(memdc);
    ReleaseDC(HWND_DESKTOP, hdc);

    return hbitmapCopy;
}

void WaitForColor(RECT rct, unsigned long color){
    for(int x = 0; x < 66; x++){ //capping out at 330 ms, which is already fairly slow

        RECT rctOnePx;
        rctOnePx.left = rct.left;
        rctOnePx.top = rct.top;
        rctOnePx.right = rct.left + 1;
        rctOnePx.bottom = rct.top + 1;
        HBITMAP pixelBmp = CaptureScreenArea(rctOnePx);

        //code adapted from https://stackoverflow.com/questions/26233848/c-read-pixels-with-getdibits
        HDC hdc = GetDC(0);

        BITMAPINFO MyBMInfo = {0};
        MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);

        // Get the BITMAPINFO structure from the bitmap
        if(0 == GetDIBits(hdc, pixelBmp, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS)) {
            std::cout << "error" << std::endl;
        }

        // create the bitmap buffer
        BYTE* lpPixels = new BYTE[MyBMInfo.bmiHeader.biSizeImage];

        // Better do this here - the original bitmap might have BI_BITFILEDS, which makes it
        // necessary to read the color table - you might not want this.
        MyBMInfo.bmiHeader.biCompression = BI_RGB;

        // get the actual bitmap buffer
        if(0 == GetDIBits(hdc, pixelBmp, 0, MyBMInfo.bmiHeader.biHeight, (LPVOID)lpPixels, &MyBMInfo, DIB_RGB_COLORS)) {
            std::cout << "error2" << std::endl;
        }

        //end of stackoverflow code
        unsigned long currentColor = (((unsigned long)lpPixels[0]) << 16) | (((unsigned long)lpPixels[1]) << 8) | (((unsigned long)lpPixels[2]));

        std::cout << currentColor << std::endl;
        if(color == currentColor)
            break;

        Sleep(5);
    }
}

void RemoveIllegalChars(std::wstring* str){
    std::wstring::iterator it;
    std::wstring illegalChars = L"\\/:?\"<>|*";
    for (it = str->begin() ; it < str->end() ; ++it){
        bool found = illegalChars.find(*it) != std::string::npos;
        if(found){
            *it = ' ';
        }
    }
}

Gdiplus::Rect CalculateCrop(Gdiplus::Bitmap* transparentBitmap){
    int imageWidth = transparentBitmap->GetWidth();
    int imageHeight = transparentBitmap->GetHeight();

    int leftcrop = imageWidth;
    int rightcrop = -1;
    int topcrop = imageHeight;
    int bottomcrop = -1;

    Gdiplus::Rect rect1(0, 0, imageWidth, imageHeight);

    Gdiplus::BitmapData transparentBitmapData;
    transparentBitmap->LockBits(&rect1, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &transparentBitmapData);
    BYTE* transparentPixels = (BYTE*) (void*) transparentBitmapData.Scan0;

    for(int x = 0; x < imageWidth; x++){
        for(int y = 0; y < imageHeight; y++){
            int currentPixel = (y*imageWidth + x)*4;
            if(transparentPixels[currentPixel+3] > 0){
                leftcrop = (leftcrop > x) ? x : leftcrop;
                topcrop = (topcrop > y) ? y : topcrop;
                rightcrop = (x > rightcrop) ? x : rightcrop;
                bottomcrop = (y > bottomcrop) ? y : bottomcrop;
            }
        }
    }

    transparentBitmap->UnlockBits(&transparentBitmapData);

    if(leftcrop >= rightcrop || topcrop >= bottomcrop){
        return Gdiplus::Rect(0, 0, 0, 0);
    }

    bottomcrop -= topcrop;
    rightcrop -= leftcrop;

    /*if(creMode){
        //if((rightcrop % 2 == 1) && rightcrop != rct.right)
            rightcrop++;

        //if((bottomcrop % 2 == 1) && bottomcrop != rct.bottom)
            bottomcrop++;
    }*/

    printf("%i ; %i ; %i ; %i", leftcrop, topcrop, rightcrop, bottomcrop);
    return Gdiplus::Rect(leftcrop, topcrop, rightcrop, bottomcrop);
}

void CloneImage(Gdiplus::Bitmap* oldBitmap, Gdiplus::Bitmap* newBitmap){
    //TODO: see if there's an equivalent of C#'s DrawImage
    Gdiplus::Rect oldRect(0, 0, oldBitmap->GetWidth(), oldBitmap->GetHeight());
    Gdiplus::Rect newRect(0, 0, newBitmap->GetWidth(), newBitmap->GetHeight());

    Gdiplus::BitmapData newBitmapData;
    newBitmap->LockBits(&newRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &newBitmapData);
    BYTE* newPixels = (BYTE*) (void*) newBitmapData.Scan0;

    Gdiplus::BitmapData oldBitmapData;
    oldBitmap->LockBits(&oldRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &oldBitmapData);
    BYTE* oldPixels = (BYTE*) (void*) oldBitmapData.Scan0;

    for(int x = 0; x < oldBitmap->GetWidth(); x++){
        for(int y = 0; y < oldBitmap->GetHeight(); y++){
            int oldPixel = (y*(oldBitmap->GetWidth()) + x)*4;
            int newPixel = (y*(newBitmap->GetWidth()) + x)*4;

            for(int z = 0; z < 4; z++)
                newPixels[newPixel + z] = oldPixels[oldPixel + z];
        }
    }

    newBitmap->UnlockBits(&newBitmapData);
    oldBitmap->UnlockBits(&oldBitmapData);

    //delete bitmap;
    //&bitmap = &bmp;
}

void CaptureCompositeScreenshot(HINSTANCE hThisInstance, HWND whiteHwnd, HWND blackHwnd, bool creMode){

    std::cout << "Screenshot capture start: " << CurrentTimestamp() << std::endl;

    HWND desktopWindow = GetDesktopWindow();
    HWND foregoundWindow = GetForegroundWindow();
    HWND taskbar = FindWindow(L"Shell_TrayWnd", NULL);
    HWND startButton = FindWindow(L"Button", L"Start");

    //hiding the taskbar in case it gets in the way
    //note that this may cause issues if the program crashes during capture
    if(foregoundWindow != taskbar && foregoundWindow != startButton){
        ShowWindow(taskbar, 0);
        ShowWindow(startButton, 0);
    }

    SetForegroundWindow(foregoundWindow);

    //calculating screenshot area
    RECT rct;
    RECT rctDesktop;

    GetWindowRect(foregoundWindow, &rct);
    GetWindowRect(desktopWindow, &rctDesktop);

    std::cout << rct.left << ";" << rct.right << ";" << rct.top << ";" << rct.bottom << std::endl;
    std::cout << rctDesktop.left << ";" << rctDesktop.right << ";" << rctDesktop.top << ";" << rctDesktop.bottom << std::endl;

    rct.left = (rctDesktop.left < (rct.left-100)) ? (rct.left - 100) : rctDesktop.left;
    rct.right = (rctDesktop.right > (rct.right+100)) ? (rct.right + 100) : rctDesktop.right;
    rct.bottom = (rctDesktop.bottom > (rct.bottom+100)) ? (rct.bottom + 100) : rctDesktop.bottom;
    rct.top = (rctDesktop.top < (rct.top-100)) ? (rct.top - 100) : rctDesktop.top;

    //spawning backdrop
    if(!SetWindowPos(blackHwnd, foregoundWindow, rct.left, rct.top, rct.right - rct.left, rct.bottom - rct.top, SWP_NOACTIVATE)){
        SetWindowPos(blackHwnd, NULL, rct.left, rct.top, rct.right - rct.left, rct.bottom - rct.top, SWP_NOACTIVATE);
        SetForegroundWindow(foregoundWindow);
    }
    SetWindowPos(whiteHwnd, blackHwnd, rct.left, rct.top, rct.right - rct.left, rct.bottom - rct.top, SWP_NOACTIVATE);

    ShowWindow (blackHwnd, SW_SHOWNOACTIVATE);

    //taking the screenshot
    WaitForColor(rct, RGB(0,0,0));

    std::cout << "Capturing black: " << CurrentTimestamp() << std::endl;

    Gdiplus::Bitmap blackShot(CaptureScreenArea(rct), NULL);

    ShowWindow (blackHwnd, 0);
    ShowWindow (whiteHwnd, SW_SHOWNOACTIVATE);

    WaitForColor(rct, RGB(255,255,255));

    /*while(true){
        unsigned __int64 whiteTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::cout << whiteTime << std::endl;
        if((whiteTime-blackTime) % 2000 == 0)
            break;
    }*/
    //Sleep(2000);

    std::cout << "Capturing white: " << CurrentTimestamp() << std::endl;
    Gdiplus::Bitmap whiteShot(CaptureScreenArea(rct), NULL);

    #if(SAVE_INTERMEDIARY_IMAGES)
        CLSID pngEncoderTemp = {0x557cf406, 0x1a04, 0x11d3, {0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e} } ;
        GetEncoderClsid(L"image/png", &pngEncoderTemp);
        whiteShot.Save(L"c:\\test\\0_whiteShot.png", &pngEncoderTemp, NULL);
        blackShot.Save(L"c:\\test\\0_blackShot.png", &pngEncoderTemp, NULL);
    #endif

    //inactive capture
    if(creMode){
        SetForegroundWindow(desktopWindow);
        Sleep(33); //Time for the foreground window to settle
        WaitForColor(rct, RGB(255,255,255));
    }
    std::cout << "Capturing black inactive: " << CurrentTimestamp() << std::endl;
    Gdiplus::Bitmap whiteInactiveShot(CaptureScreenArea(rct), NULL);
    if(creMode){
        ShowWindow (blackHwnd, SW_SHOWNOACTIVATE);
        ShowWindow (whiteHwnd, 0);
        WaitForColor(rct, RGB(0,0,0));
    }
    std::cout << "Capturing white inactive: " << CurrentTimestamp() << std::endl;
    Gdiplus::Bitmap blackInactiveShot(CaptureScreenArea(rct), NULL);

    //activating taskbar
    ShowWindow(taskbar, 1);
    ShowWindow(startButton, 1);

    //hiding backdrop
    ShowWindow (blackHwnd, 0);
    ShowWindow (whiteHwnd, 0);

    //differentiating alpha
    std::cout << "Differentiating alpha: " << CurrentTimestamp() << std::endl;
    Gdiplus::Bitmap transparentBitmap(whiteShot.GetWidth(), whiteShot.GetHeight(), PixelFormat32bppARGB);
    DifferentiateAlpha(&whiteShot, &blackShot, &transparentBitmap);

    Gdiplus::Bitmap transparentInactiveBitmap(whiteShot.GetWidth(), whiteShot.GetHeight(), PixelFormat32bppARGB);

    if(creMode)
        DifferentiateAlpha(&whiteInactiveShot, &blackInactiveShot, &transparentInactiveBitmap);

    //calculating crop
    std::cout << "Capturing crop: " << CurrentTimestamp() << std::endl;
    Gdiplus::Rect rect1(0, 0, (rct.right - rct.left), (rct.bottom - rct.top));
    Gdiplus::Rect crop = CalculateCrop(&transparentBitmap);
    if(crop.GetLeft() == crop.GetRight() || crop.GetTop() == crop.GetBottom()){
        ShowWindow (whiteHwnd, 0);
        ShowWindow (blackHwnd, 0);
        MessageBox(whiteHwnd, L"Screenshot is empty, aborting capture.", L"Error", MB_OK | MB_ICONSTOP);
        return;
    }

    std::cout << "Creating bitmaps: " << CurrentTimestamp() << std::endl;
    Gdiplus::Bitmap* croppedBitmap = transparentBitmap.Clone(crop, PixelFormatDontCare);
    Gdiplus::Bitmap* croppedInactive = transparentInactiveBitmap.Clone(crop, PixelFormatDontCare);

    int imageWidth = croppedBitmap->GetWidth();
    int imageHeight = croppedBitmap->GetHeight();
    if(creMode){
        if((imageWidth % 2 == 1))
            imageWidth++;

        if((imageHeight % 2 == 1))
            imageHeight++;
    }

    Gdiplus::Bitmap* clonedBitmap = new Gdiplus::Bitmap(imageWidth, imageHeight, PixelFormat32bppARGB);
    Gdiplus::Bitmap* clonedInactive = new Gdiplus::Bitmap(imageWidth, imageHeight, PixelFormat32bppARGB);
    CloneImage(croppedBitmap, clonedBitmap);
    CloneImage(croppedInactive, clonedInactive);

    //Saving memory
    delete croppedBitmap;
    delete croppedInactive;

    //Saving the image
    std::cout << "Saving: " << CurrentTimestamp() << std::endl;
    CLSID pngEncoder = {0x557cf406, 0x1a04, 0x11d3, {0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e} } ;
    GetEncoderClsid(L"image/png", &pngEncoder);

    TCHAR h[2048];
    GetWindowText(foregoundWindow, h, 2048);
    std::wstring windowTextStr(h);

    RemoveIllegalChars(&windowTextStr);

    std::wcout << windowTextStr << std::endl;
    //std::cout << std::endl << len;

    std::wstring path = GetSaveDirectory();
    std::wcout << L"registrypath: " << path << std::endl;

    CreateDirectory(path.c_str(), NULL);
    std::wstringstream pathbuild;
    std::wstringstream pathbuildInactive;
    pathbuild << path << L"\\" << windowTextStr << L"_b1.png";
    pathbuildInactive << path << L"\\" << windowTextStr << L"_b2.png";

    std::wstring fileName = pathbuild.str();
    std::wstring fileNameInactive = pathbuildInactive.str();

    unsigned int i = 0;
    while(FileExists(fileName) | FileExists(fileNameInactive)){
        pathbuild.str(L"");
        pathbuildInactive.str(L"");
        pathbuild << path << windowTextStr << L"_" << i << L"_b1.png";
        pathbuildInactive << path << windowTextStr << L"_" << i << L"_b2.png";
        fileName = pathbuild.str();
        fileNameInactive = pathbuildInactive.str();
        i++;
    }

    /*std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring fileNameUtf16 = converter.from_bytes(fileName);
    std::wstring fileNameInactiveUtf16 = converter.from_bytes(fileNameInactive);*/

    std::wcout << fileName << std::endl << fileNameInactive << std::endl;
    DisplayGdiplusStatusError(clonedBitmap->Save(fileName.c_str(), &pngEncoder, NULL));
    if(creMode)
        DisplayGdiplusStatusError(clonedInactive->Save(fileNameInactive.c_str(), &pngEncoder, NULL));

    std::cout << "Done: " << CurrentTimestamp() << std::endl;
    //Cleaning memory
    delete clonedBitmap;
    delete clonedInactive;
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    INITCOMMONCONTROLSEX icc;

    // Initialise common controls.
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = (HICON) LoadImage(hThisInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
    wincl.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("CppShot " CPPSHOT_VERSION),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    HWND hwndButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed
            L"This button doesn't do anything, press CTRL+B to take a screenshot",      // Button text
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
            10,         // x position
            10,         // y position
            500,        // Button width
            100,        // Button height
            hwnd,     // Parent window
            NULL,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.

     HWND hwndButtonTwo = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed
            L"Or you can press CTRL+SHIFT+B to take inactive and active screenshots",      // Button text
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
            10,         // x position
            120,         // y position
            500,        // Button width
            100,        // Button height
            hwnd,     // Parent window
            NULL,       // No menu.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.

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

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Create backdrop windows */
    HWND whiteHwnd = createBackdropWindow(hThisInstance, *whiteBackdropClassName, (HBRUSH) CreateSolidBrush(RGB(255,255,255)));
    HWND blackHwnd = createBackdropWindow(hThisInstance, *blackBackdropClassName, (HBRUSH) CreateSolidBrush(RGB(0,0,0)));

    /* Start GDI+ */
    Gdiplus::GdiplusStartupInput gpStartupInput;
    ULONG_PTR gpToken;
    int val = Gdiplus::GdiplusStartup(&gpToken, &gpStartupInput, NULL);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        if (messages.message == WM_HOTKEY)
        {
            _tprintf(_T("WM_HOTKEY received\n"));
            if (messages.wParam == 1)
                CaptureCompositeScreenshot(hThisInstance, whiteHwnd, blackHwnd, false);
            else if (messages.wParam == 2)
                CaptureCompositeScreenshot(hThisInstance, whiteHwnd, blackHwnd, true);
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

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;

        case WM_COMMAND:               /* menu items */
          switch (LOWORD(wParam))
          {
            case ID_FILE_OPEN:
            {
              StartExplorer();
              return 0;
            }

            case ID_FILE_EXIT:
            {
              DestroyWindow(hwnd);
              return 0;
            }
          }
          break;

        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
