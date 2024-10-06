#include "Screenshot.h"

Screenshot::Screenshot() {}

Screenshot::Screenshot(HWND window) {
	capture(window);
}

Screenshot::~Screenshot() {
	delete m_image;
}

void Screenshot::capture(HWND window) {
	delete m_image;

    m_window = window;
	RECT rct = createRect();

	HDC hdc = GetDC(HWND_DESKTOP);
    HDC memdc = CreateCompatibleDC(hdc);
    HBITMAP hbitmap = CreateCompatibleBitmap(hdc, rct.right - rct.left, rct.bottom - rct.top);

    SelectObject(memdc, hbitmap);
    BitBlt(memdc, 0, 0, rct.right - rct.left, rct.bottom - rct.top, hdc, rct.left, rct.top, SRCCOPY );

    DeleteDC(memdc);
    ReleaseDC(HWND_DESKTOP, hdc);

	m_image = new Gdiplus::Bitmap(hbitmap, NULL);
	//delete hbitmap;
}

RECT Screenshot::createRect() {
    RECT rctDesktop;
    RECT rct;

    HWND desktopWindow = GetDesktopWindow();

    GetWindowRect(m_window, &rct);
    GetWindowRect(desktopWindow, &rctDesktop);

	rct.left = (rctDesktop.left < (rct.left-100)) ? (rct.left - 100) : rctDesktop.left;
    rct.right = (rctDesktop.right > (rct.right+100)) ? (rct.right + 100) : rctDesktop.right;
    rct.bottom = (rctDesktop.bottom > (rct.bottom+100)) ? (rct.bottom + 100) : rctDesktop.bottom;
    rct.top = (rctDesktop.top < (rct.top-100)) ? (rct.top - 100) : rctDesktop.top;

    return rct;
}

void Screenshot::save(const std::wstring& path) {
	// This is supposed to be gathered from the OS but the encoder CLSID has never changed, so this is safe enough
	// The old version of cppshot had a bug that made it use the hardcoded one anyway
    CLSID pngEncoder = {0x557cf406, 0x1a04, 0x11d3, {0x9a, 0x73, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e} } ;
    
    m_image->Save(path.c_str(), &pngEncoder, NULL);
}

bool Screenshot::isCaptured() {
    return m_image != nullptr;
}

Gdiplus::Bitmap* Screenshot::getBitmap() const {
	return m_image;
}