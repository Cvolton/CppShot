#include "BackdropWindow.h"

#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <stdexcept>
#include <iostream>

void BackdropWindow::waitForResize(LONG left, LONG top) const {

    for(int x = 0; x < 66; x++){ //capping out at 330 ms, which is already fairly slow

		/*HDC hdc = GetDC(HWND_DESKTOP);
		HDC memdc = CreateCompatibleDC(hdc);
		HBITMAP pixelBmp = CreateCompatibleBitmap(hdc, 1, 1);
		SelectObject(memdc, pixelBmp);
		BitBlt(memdc, 0, 0, 1, 1, hdc, left, top, SRCCOPY );
		DeleteDC(memdc);
		ReleaseDC(HWND_DESKTOP, hdc);

        //code adapted from https://stackoverflow.com/questions/26233848/c-read-pixels-with-getdibits
        hdc = GetDC(0);

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

        DeleteObject(pixelBmp);*/

    	COLORREF currentColor = GetPixel(GetDC(HWND_DESKTOP), left, top);
        std::cout << std::hex << currentColor << " " << m_color << std::endl;
        if(m_color == currentColor)
            break;

        Sleep(5);
    }
}

BackdropWindow::BackdropWindow(HINSTANCE hThisInstance, COLORREF color, const TCHAR* className) : m_color(color) {
	HBRUSH brush = (HBRUSH) CreateSolidBrush(color);

	WNDCLASSEX wincl;        /* Data structure for the windowclass */
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = className;
	wincl.lpfnWndProc = BackdropWindow::windowProcedure;      /* This function is called by windows */
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
	wincl.hbrBackground = brush;

	if(!RegisterClassEx (&wincl)) throw std::runtime_error("Unable to create backdrop window");

	m_window = CreateWindowEx(
		WS_EX_TOOLWINDOW,                   /* Extended possibilites for variation */
		className,         /* Classname */
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

	std::cout << "gg";
}

void BackdropWindow::resize(HWND window) {
	HWND desktopWindow = GetDesktopWindow();

	RECT rctDesktop;

	GetWindowRect(window, &m_rect);
    GetWindowRect(desktopWindow, &rctDesktop);

    std::cout << m_rect.left << ";" << m_rect.right << ";" << m_rect.top << ";" << m_rect.bottom << std::endl;
    std::cout << rctDesktop.left << ";" << rctDesktop.right << ";" << rctDesktop.top << ";" << rctDesktop.bottom << std::endl;

    m_rect.left = (rctDesktop.left < (m_rect.left-100)) ? (m_rect.left - 100) : rctDesktop.left;
    m_rect.right = (rctDesktop.right > (m_rect.right+100)) ? (m_rect.right + 100) : rctDesktop.right;
    m_rect.bottom = (rctDesktop.bottom > (m_rect.bottom+100)) ? (m_rect.bottom + 100) : rctDesktop.bottom;
    m_rect.top = (rctDesktop.top < (m_rect.top-100)) ? (m_rect.top - 100) : rctDesktop.top;

    if(!SetWindowPos(m_window, window, m_rect.left, m_rect.top, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, SWP_NOACTIVATE)){
        SetWindowPos(m_window, NULL, m_rect.left, m_rect.top, m_rect.right - m_rect.left, m_rect.bottom - m_rect.top, SWP_NOACTIVATE);
    }
}

void BackdropWindow::show() const {
	std::cout << "show";
	ShowWindow (m_window, SW_SHOWNOACTIVATE);
    waitForResize(m_rect.left, m_rect.top);
}

void BackdropWindow::hide() const {
	std::cout << "hide";
	ShowWindow (m_window, 0);
}

LRESULT CALLBACK BackdropWindow::windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
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
