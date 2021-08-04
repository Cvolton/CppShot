#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <wingdi.h>
#include <gdiplus.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <locale>
#include <codecvt>
#include <string>
#include <sys/stat.h>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("MainCreWindow");
TCHAR blackBackdropClassName[ ] = _T("BlackBackdropWindow");
TCHAR whiteBackdropClassName[ ] = _T("WhiteBackdropWindow");
TCHAR transparentBackdropClassName[ ] = _T("TransparentBackdropWindow");

inline bool FileExists (const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
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

/*char* statusString(const Gdiplus::Status status) {
    switch (status) {
        case Gdiplus::Ok: return "Ok";
        case Gdiplus::GenericError: return "GenericError";
        case Gdiplus::InvalidParameter: return "InvalidParameter";
        case Gdiplus::OutOfMemory: return "OutOfMemory";
        case Gdiplus::ObjectBusy: return "ObjectBusy";
        case Gdiplus::InsufficientBuffer: return "InsufficientBuffer";
        case Gdiplus::NotImplemented: return "NotImplemented";
        case Gdiplus::Win32Error: return "Win32Error";
        case Gdiplus::Aborted: return "Aborted";
        case Gdiplus::FileNotFound: return "FileNotFound";
        case Gdiplus::ValueOverflow: return "ValueOverflow";
        case Gdiplus::AccessDenied: return "AccessDenied";
        case Gdiplus::UnknownImageFormat: return "UnknownImageFormat";
        case Gdiplus::FontFamilyNotFound: return "FontFamilyNotFound";
        case Gdiplus::FontStyleNotFound: return "FontStyleNotFound";
        case Gdiplus::NotTrueTypeFont: return "NotTrueTypeFont";
        case Gdiplus::UnsupportedGdiplusVersion: return "UnsupportedGdiplusVersion";
        case Gdiplus::GdiplusNotInitialized: return "GdiplusNotInitialized";
        case Gdiplus::PropertyNotFound: return "PropertyNotFound";
        case Gdiplus::PropertyNotSupported: return "PropertyNotSupported";
        default: return "Status Type Not Found.";
    }
}*/

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
        MessageBox(NULL, "Unable to create backdrop window, the program may not work correctly.", "Error", 0);
        return NULL;
    }

    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
            &className,         /* Classname */
           _T("Backdrop Window"),       /* Title Text */
           WS_POPUP, /* default window */
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
    transparentBitmap->LockBits(&rect1, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &transparentBitmapData);
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
            transparentPixels[currentPixel+3] = ((blackPixels[currentPixel+2] - whitePixels[currentPixel+2] + 255 + blackPixels[currentPixel+1] - whitePixels[currentPixel+1] + 255 + blackPixels[currentPixel] - whitePixels[currentPixel] + 255) / 3);
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

void CaptureCompositeScreenshot(HINSTANCE hThisInstance, HWND whiteHwnd, HWND blackHwnd, bool creMode){

    HWND desktopWindow = GetDesktopWindow();
    HWND foregoundWindow = GetForegroundWindow();
    HWND taskbar = FindWindow("Shell_TrayWnd", NULL);
    HWND startButton = FindWindow("Button", "Start");

    //hiding the taskbar in case it gets in the way
    //note that this may cause issues if the program crashes during capture
    if(foregoundWindow != taskbar && foregoundWindow != startButton){
        ShowWindow(taskbar, 0);
        ShowWindow(startButton, 0);
    }

    SetForegroundWindow(foregoundWindow);

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

    if(!SetWindowPos(blackHwnd, foregoundWindow, rct.left, rct.top, rct.right - rct.left, rct.bottom - rct.top, SWP_NOACTIVATE))
        SetWindowPos(blackHwnd, whiteHwnd, rct.left, rct.top, rct.right - rct.left, rct.bottom - rct.top, SWP_NOACTIVATE);
    SetWindowPos(whiteHwnd, blackHwnd, rct.left, rct.top, rct.right - rct.left, rct.bottom - rct.top, SWP_NOACTIVATE);

    ShowWindow (blackHwnd, SW_SHOWNOACTIVATE);

    Sleep(33);
    //taking the screenshot
    Gdiplus::Bitmap blackShot(CaptureScreenArea(rct), NULL);

    ShowWindow (blackHwnd, 0);
    ShowWindow (whiteHwnd, SW_SHOWNOACTIVATE);
    Sleep(33);

    Gdiplus::Bitmap whiteShot(CaptureScreenArea(rct), NULL);

    //inactive capture
    if(creMode){
        SetForegroundWindow(desktopWindow);
        Sleep(33);
    }
    Gdiplus::Bitmap whiteInactiveShot(CaptureScreenArea(rct), NULL);
    if(creMode){
        ShowWindow (blackHwnd, SW_SHOWNOACTIVATE);
        ShowWindow (whiteHwnd, 0);
        Sleep(33);
    }
    Gdiplus::Bitmap blackInactiveShot(CaptureScreenArea(rct), NULL);

    //activating taskbar
    ShowWindow(taskbar, 1);
    ShowWindow(startButton, 1);

    //calculating crop
    int leftcrop = (rct.right - rct.left);
    int rightcrop = -1;
    int topcrop = (rct.bottom - rct.top);
    int bottomcrop = -1;

    Gdiplus::Bitmap transparentBitmap(whiteShot.GetWidth(), whiteShot.GetHeight(), PixelFormat32bppARGB);
    DifferentiateAlpha(&whiteShot, &blackShot, &transparentBitmap);

    Gdiplus::Bitmap transparentInactiveBitmap(whiteShot.GetWidth(), whiteShot.GetHeight(), PixelFormat32bppARGB);
    if(creMode)
        DifferentiateAlpha(&whiteInactiveShot, &blackInactiveShot, &transparentInactiveBitmap);

    Gdiplus::Rect rect1(0, 0, (rct.right - rct.left), (rct.bottom - rct.top));

    Gdiplus::BitmapData transparentBitmapData;
    transparentBitmap.LockBits(&rect1, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &transparentBitmapData);
    BYTE* transparentPixels = (BYTE*) (void*) transparentBitmapData.Scan0;

    for(int x = 0; x < (rct.right - rct.left); x++){
        for(int y = 0; y < (rct.bottom - rct.top); y++){
            int currentPixel = (y*(rct.right - rct.left) + x)*4;
            if(transparentPixels[currentPixel+3] > 0){
                leftcrop = (leftcrop > x) ? x : leftcrop;
                topcrop = (topcrop > y) ? y : topcrop;
                rightcrop = (x > rightcrop) ? x : rightcrop;
                bottomcrop = (y > bottomcrop) ? y : bottomcrop;
            }
        }
    }

    transparentBitmap.UnlockBits(&transparentBitmapData);

    if(leftcrop >= rightcrop || topcrop >= bottomcrop){
        ShowWindow (whiteHwnd, 0);
        ShowWindow (blackHwnd, 0);
        MessageBox(whiteHwnd, "Screenshot is empty, aborting capture.", "Error", MB_OK | MB_ICONSTOP);
        return;
    }

    bottomcrop -= topcrop;
    rightcrop -= leftcrop;

    if(creMode){
        if((rightcrop % 2 == 1) && rightcrop != rct.right)
            rightcrop++;

        if((bottomcrop % 2 == 1) && bottomcrop != rct.bottom)
            bottomcrop++;
    }

    printf("%i ; %i ; %i ; %i", leftcrop, topcrop, rightcrop, bottomcrop);

    Gdiplus::Bitmap* croppedBitmap = transparentBitmap.Clone(Gdiplus::Rect(leftcrop, topcrop, rightcrop, bottomcrop), PixelFormatDontCare);
    Gdiplus::Bitmap* croppedInactive = transparentInactiveBitmap.Clone(Gdiplus::Rect(leftcrop, topcrop, rightcrop, bottomcrop), PixelFormatDontCare);

    //Saving the image
    CLSID pngEncoder = {0x557cf406, 0x1a04, 0x11d3, {0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e} } ;
    GetEncoderClsid(L"image/png", &pngEncoder);

    char h[2048];
    GetWindowText(foregoundWindow, h, 2048);
    std::string windowTextStr(h);

    std::string::iterator it;
    std::string illegalChars = "\\/:?\"<>|*";
    for (it = windowTextStr.begin() ; it < windowTextStr.end() ; ++it){
        bool found = illegalChars.find(*it) != std::string::npos;
        if(found){
            *it = ' ';
        }
    }

    std::cout << windowTextStr << std::endl;
    //std::cout << std::endl << len;

    std::string path = "c:\\test\\";
    std::ostringstream pathbuild;
    std::ostringstream pathbuildInactive;
    pathbuild << path << windowTextStr << "_b1.png";
    pathbuildInactive << path << windowTextStr << "_b2.png";

    std::string fileName = pathbuild.str();
    std::string fileNameInactive = pathbuildInactive.str();

    unsigned int i = 0;
    while(FileExists(fileName) | FileExists(fileNameInactive)){
        pathbuild.str("");
        pathbuildInactive.str("");
        pathbuild << path << windowTextStr << "_" << i << "_b1.png";
        pathbuildInactive << path << windowTextStr << "_" << i << "_b2.png";
        fileName = pathbuild.str();
        fileNameInactive = pathbuildInactive.str();
        i++;
    }

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring fileNameUtf16 = converter.from_bytes(fileName);
    std::wstring fileNameInactiveUtf16 = converter.from_bytes(fileNameInactive);

    std::wcout << fileNameUtf16 << std::endl << fileNameInactiveUtf16 << std::endl;
    croppedBitmap->Save(fileNameUtf16.c_str(), &pngEncoder, NULL);
    if(creMode)
        croppedInactive->Save(fileNameInactiveUtf16.c_str(), &pngEncoder, NULL);

    //hiding backdrop
    ShowWindow (blackHwnd, 0);
    ShowWindow (whiteHwnd, 0);

    //Cleaning memory
    delete croppedBitmap;
    delete croppedInactive;
}

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
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
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("CppShot"),       /* Title Text */
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
            "BUTTON",  // Predefined class; Unicode assumed
            "This button doesn't do anything, press CTRL+B to take a screenshot",      // Button text
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
            "BUTTON",  // Predefined class; Unicode assumed
            "Or you can press CTRL+SHIFT+B to take inactive and active screenshots",      // Button text
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
        }

    if (RegisterHotKey(
            NULL,
            2,
            0x6,
            0x42))  //0x42 is 'b'
        {
            _tprintf(_T("CTRL+SHIFT+b\n"));
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


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
