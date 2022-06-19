#pragma once

#include <windows.h>

class BackdropWindow {
	HWND m_window = nullptr;
	COLORREF m_color;
	RECT m_rect;
	void waitForResize(LONG left, LONG top) const;
public:
	BackdropWindow(HINSTANCE hThisInstance, COLORREF color, const TCHAR* className);
	void resize(HWND window);
	void show() const;
	void hide() const;
	static LRESULT CALLBACK windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};