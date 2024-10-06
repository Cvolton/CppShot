#pragma once

#include "Screenshot.h"

#include <windows.h>
#include <gdiplus.h>
#include <string>

class CompositeScreenshot : public Screenshot {
	Gdiplus::Rect m_crop;

	void init(const Screenshot& white, const Screenshot& black);
	void differentiateAlpha(Gdiplus::Bitmap* whiteShot, Gdiplus::Bitmap* blackShot);
	void cropImage();
	Gdiplus::Rect calculateCrop();
public:
	Gdiplus::Rect getCrop();
	CompositeScreenshot(const Screenshot& whiteShot, const Screenshot& blackShot);
	CompositeScreenshot(const Screenshot& whiteShot, const Screenshot& blackShot, Gdiplus::Rect crop);
};