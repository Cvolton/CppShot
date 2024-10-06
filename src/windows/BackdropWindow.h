#pragma once

#include <windows.h>
#include "Window.h"

class BackdropWindow : public Window {
	COLORREF m_color;
	RECT m_rect;
	void waitForResize(LONG left, LONG top) const;
public:
	BackdropWindow(HINSTANCE hThisInstance, COLORREF color, const TCHAR* className);
	Window& resize(HWND window);
	Window& show(int nCmdShow = SW_SHOWNORMAL) const;
};