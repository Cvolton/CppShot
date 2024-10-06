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

    /**
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
     */
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
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (m_window, message, wParam, lParam);
    }
}