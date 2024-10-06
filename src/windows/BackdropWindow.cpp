#include "BackdropWindow.h"

#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <iostream>

void BackdropWindow::waitForResize(LONG left, LONG top) const {

    for(int x = 0; x < 66; x++){ //capping out at 330 ms, which is already fairly slow
    
    	COLORREF currentColor = GetPixel(GetDC(HWND_DESKTOP), left, top);
        std::cout << std::hex << currentColor << " " << m_color << std::endl;
        if(m_color == currentColor)
            break;

        Sleep(5);
    }
}

BackdropWindow::BackdropWindow(HINSTANCE hThisInstance, COLORREF color, const TCHAR* className) : Window(hThisInstance, CreateSolidBrush(color), className, _T("Backdrop Window"), WS_EX_TOOLWINDOW, WS_POPUP) {
	m_color = color;
}

Window& BackdropWindow::resize(HWND window) {
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

	return *this;
}

Window& BackdropWindow::show(int nCmdShow) const {
	ShowWindow (m_window, SW_SHOWNOACTIVATE);
    waitForResize(m_rect.left, m_rect.top);
    return *const_cast<BackdropWindow*>(this);
}