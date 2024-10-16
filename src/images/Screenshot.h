#pragma once

#include <windows.h>
#include <gdiplus.h>
#include <string>

class Screenshot {
protected:
	RECT m_captureRect;
	Gdiplus::Bitmap* m_image = nullptr;
	HWND m_window = nullptr;

	RECT createRect();
	void encoderClsid();
public:
	Screenshot();
	Screenshot(HWND window);
	~Screenshot();
	void capture(HWND window);
	void save(const std::wstring& path);
	bool isCaptured() const;
	Gdiplus::Bitmap* getBitmap() const;
	RECT getCaptureRect() const;
};