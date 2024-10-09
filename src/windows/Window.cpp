#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif
#define _WIN32_IE 0x0300

#include "Window.h"
#include "resources.h"
#include <windows.h>
#include <commctrl.h>
#include <stdexcept>
#include <iostream>

#include <tchar.h>

Window::Window(HINSTANCE hThisInstance, HBRUSH brush, const TCHAR* className, const TCHAR* title, DWORD dwExStyle, DWORD dwStyle) {
    // Initialise common controls.
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);

	WNDCLASSEX wincl;        /* Data structure for the windowclass */
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = className;
	wincl.lpfnWndProc = Window::windowProcedure;      /* This function is called by windows */
	wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
	wincl.cbSize = sizeof (WNDCLASSEX);

	/* Use default icon and mouse-pointer */
	wincl.hIcon = (HICON) LoadImage(hThisInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
    wincl.hIconSm = wincl.hIcon;
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;                 /* No menu */
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
	wincl.cbWndExtra = 0;                      /* structure or the window instance */

	/* Use Windows's default colour as the background of the window */
	wincl.hbrBackground = brush;

	if(!RegisterClassEx (&wincl)) throw std::runtime_error("Unable to create window");

	m_window = CreateWindowEx (
        dwExStyle,                   /* Extended possibilites for variation */
        className,         /* Classname */
        title,       /* Title Text */
        dwStyle, /* default window */
        CW_USEDEFAULT,       /* Windows decides the position */
        CW_USEDEFAULT,       /* where the window ends up on the screen */
        544,                 /* The programs width */
        375,                 /* and height in pixels */
        HWND_DESKTOP,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hThisInstance,       /* Program Instance handler */
        NULL                 /* No Window Creation data */
    );

    SetWindowLongPtr(m_window, GWLP_USERDATA, (LONG_PTR)this);
}

LRESULT CALLBACK Window::windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if(auto ptr = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA))) return ptr->handleMessage(message, wParam, lParam);

    return DefWindowProc(hwnd, message, wParam, lParam);
}

HWND Window::getWindow() {
    return m_window;
}

Window& Window::show(int nCmdShow) const {
    ShowWindow (m_window, nCmdShow);
    return *const_cast<Window*>(this);
}

Window& Window::hide() const {
    ShowWindow (m_window, 0);
    return *const_cast<Window*>(this);
}

Window& Window::setSize(int width, int height) {
    auto scale = getScaleFactor();
    SetWindowPos(m_window, NULL, 0, 0, width * scale, height * scale, SWP_NOMOVE | SWP_NOZORDER);
    return *this;
}

LRESULT Window::handleMessage(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_FILE_OPEN:
                    //StartExplorer();
                    break;
                case ID_FILE_EXIT:
                    DestroyWindow(m_window);
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        case 0x02E0: //WM_DPICHANGED
            for(auto child : m_children) child->forceResize();
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (m_window, message, wParam, lParam);
    }

    return 0;
}

void Window::addChild(Node* child) {
    m_children.push_back(child);
}

unsigned int Window::getDPI() {
    //use GetDpiForWindow if available
    if (HMODULE hUser32 = GetModuleHandle(L"user32.dll")) {
        if (auto pGetDpiForWindow = reinterpret_cast<UINT(WINAPI*)(HWND)>(GetProcAddress(hUser32, "GetDpiForWindow"))) {
            return pGetDpiForWindow(m_window);
        }
    }

    //use GetDpiForSystem if available
    if (HMODULE hShcore = LoadLibrary(L"shcore.dll")) {
        if (auto pGetDpiForSystem = reinterpret_cast<UINT(WINAPI*)()>(GetProcAddress(hShcore, "GetDpiForSystem"))){
            return pGetDpiForSystem();
        }
    }

    //use GetDeviceCaps as a last resort
    return GetDeviceCaps(GetDC(m_window), LOGPIXELSX);
}

double Window::getScaleFactor() {
    return getDPI() / 96.0;
}

Window::~Window() {
    for(auto child : m_children) delete child;
}