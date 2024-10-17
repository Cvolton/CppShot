#include "CompositeScreenshot.h"
#include "Utils.h"

#include <stdexcept>
#include <cstring>
#include <sstream>

inline BYTE toByte(int value){
    return value > 255 ? 255 : value;
}

void CompositeScreenshot::init(const Screenshot& white, const Screenshot& black){
	Gdiplus::Bitmap* whiteShot = white.getBitmap(), *blackShot = black.getBitmap();
	if(whiteShot->GetWidth() != blackShot->GetWidth() || whiteShot->GetHeight() != blackShot->GetHeight()) throw std::runtime_error("Black/white screenshot size mismatch");
    if(whiteShot->GetWidth() == 0 || whiteShot->GetHeight() == 0) throw std::runtime_error("Zero width captured screenshot");

	m_image = new Gdiplus::Bitmap(whiteShot->GetWidth(), whiteShot->GetHeight(), PixelFormat32bppARGB);
    m_captureRect = white.getCaptureRect();

	differentiateAlpha(whiteShot, blackShot);

    auto beforeStamp = CppShot::currentTimestamp();
	cropImage();
    auto afterStamp = CppShot::currentTimestamp();
    std::wstringstream os;
    os << L"cropImage took: " << afterStamp - beforeStamp << L"ms" << std::endl;
    MessageBox(NULL, os.str().c_str(), L"Performance", MB_OK);
}

CompositeScreenshot::CompositeScreenshot(const Screenshot& white, const Screenshot& black) : Screenshot() {
    this->init(white, black);
}

CompositeScreenshot::CompositeScreenshot(const Screenshot& white, const Screenshot& black, Gdiplus::Rect crop) : Screenshot() {
    m_crop = crop;
    this->init(white, black);
}

void CompositeScreenshot::differentiateAlpha(Gdiplus::Bitmap* whiteShot, Gdiplus::Bitmap* blackShot){
    auto monitorRects = CppShot::getMonitorRects();

	Gdiplus::BitmapData transparentBitmapData;
    Gdiplus::Rect rect1(0, 0, m_image->GetWidth(), m_image->GetHeight());
    m_image->LockBits(&rect1, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &transparentBitmapData);
    BYTE* transparentPixels = (BYTE*) transparentBitmapData.Scan0;

    Gdiplus::BitmapData whiteBitmapData;
    whiteShot->LockBits(&rect1, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &whiteBitmapData);
    const BYTE* whitePixels = (BYTE*) whiteBitmapData.Scan0;

    Gdiplus::BitmapData blackBitmapData;
    blackShot->LockBits(&rect1, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &blackBitmapData);
    const BYTE* blackPixels = (BYTE*) blackBitmapData.Scan0;

    bool isOnlyOneMonitorConnected = monitorRects.size() == 1;

    auto width = whiteShot->GetWidth();
    auto height = whiteShot->GetHeight();

    BYTE* transparentFullBegin = nullptr;
    BYTE* whiteFullBegin = nullptr;

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            int currentPixel = (y*width + x)*4;

            bool isInsideMonitor = isOnlyOneMonitorConnected;
            if(!isInsideMonitor){
                for(auto monitorRect : monitorRects){
                    if(x + m_captureRect.left >= monitorRect.left && x+ m_captureRect.left < monitorRect.right && y + m_captureRect.top >= monitorRect.top && y + m_captureRect.top < monitorRect.bottom){
                        isInsideMonitor = true;
                        break;
                    }
                }
            }

            // Oddly enough this makes the code both faster and more readable
            // compared to direct array accesses in the calculation itself
            BYTE blackR = blackPixels[currentPixel + 2];
            BYTE blackG = blackPixels[currentPixel + 1];
            BYTE blackB = blackPixels[currentPixel];
            BYTE whiteR = whitePixels[currentPixel + 2];
            BYTE whiteG = whitePixels[currentPixel + 1];
            BYTE whiteB = whitePixels[currentPixel];

            // Calculate alpha
            BYTE alpha = isInsideMonitor
                ? toByte((blackR - whiteR + 255 + blackG - whiteG + 255 + blackB - whiteB + 255) / 3)
                : 0;

            if(alpha == 255) {
                if(transparentFullBegin == nullptr) transparentFullBegin = transparentPixels + currentPixel;
                if(whiteFullBegin == nullptr) whiteFullBegin = (BYTE*) whitePixels + currentPixel;
            } else {
                if(transparentFullBegin != nullptr) {
                    std::memcpy(transparentFullBegin, whiteFullBegin, (transparentPixels + currentPixel) - transparentFullBegin);
                    transparentFullBegin = nullptr;
                    whiteFullBegin = nullptr;
                }

                if (alpha > 0) {
                    transparentPixels[currentPixel + 3] = alpha;
                    transparentPixels[currentPixel + 2] = toByte(255 * blackR / alpha); // RED
                    transparentPixels[currentPixel + 1] = toByte(255 * blackG / alpha); // GREEN
                    transparentPixels[currentPixel] = toByte(255 * blackB / alpha); // BLUE
                }
            }
        }
    }

    m_image->UnlockBits(&transparentBitmapData);
    whiteShot->UnlockBits(&whiteBitmapData);
    blackShot->UnlockBits(&blackBitmapData);
}

Gdiplus::Rect CompositeScreenshot::calculateCrop(){
    int imageWidth = m_image->GetWidth();
    int imageHeight = m_image->GetHeight();

    int leftcrop = imageWidth;
    int rightcrop = -1;
    int topcrop = imageHeight;
    int bottomcrop = -1;

    Gdiplus::Rect rect1(0, 0, imageWidth, imageHeight);

    Gdiplus::BitmapData transparentBitmapData;
    m_image->LockBits(&rect1, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &transparentBitmapData);
    BYTE* transparentPixels = (BYTE*) (void*) transparentBitmapData.Scan0;

    for(int x = 0; x < imageWidth; x++){
        for(int y = 0; y < imageHeight; y++){
            int currentPixel = (y*imageWidth + x)*4;
            if(transparentPixels[currentPixel+3] > 0){
                leftcrop = (leftcrop > x) ? x : leftcrop;
                topcrop = (topcrop > y) ? y : topcrop;
                rightcrop = (x > rightcrop) ? x : rightcrop;
                bottomcrop = (y > bottomcrop) ? y : bottomcrop;
            }
        }
    }

    //"temporary" workaround until I have time to analyze why the actual algo cuts the image one pixel short
    rightcrop++;
    bottomcrop++;

    m_image->UnlockBits(&transparentBitmapData);

    if(leftcrop >= rightcrop || topcrop >= bottomcrop){
        return Gdiplus::Rect(0, 0, 0, 0);
    }

    bottomcrop -= topcrop;
    rightcrop -= leftcrop;

    m_crop = Gdiplus::Rect(leftcrop, topcrop, rightcrop, bottomcrop);

    return m_crop;
}

Gdiplus::Rect CompositeScreenshot::getCrop() {
	if(m_crop.GetLeft() == 0 && m_crop.GetRight() == 0) return calculateCrop();
	return m_crop;
}

void CompositeScreenshot::cropImage() {
	Gdiplus::Rect crop = getCrop();
	if(crop.GetLeft() == crop.GetRight() || crop.GetTop() == crop.GetBottom()) throw std::runtime_error("The captured screenshot is empty");

    auto newWidth = crop.GetRight() - crop.GetLeft();
    auto newHeight = crop.GetBottom() - crop.GetTop();
    auto copyWidth = newWidth;

    if(newWidth % 2 != 0) newWidth++;
    if(newHeight % 2 != 0) newHeight++;

    Gdiplus::Bitmap* newBitmap = new Gdiplus::Bitmap(newWidth, newHeight, PixelFormat32bppARGB);
    Gdiplus::Bitmap* oldBitmap = m_image;

    Gdiplus::Rect oldRect(0, 0, oldBitmap->GetWidth(), oldBitmap->GetHeight());
    Gdiplus::Rect newRect(0, 0, newWidth, newHeight);

    Gdiplus::BitmapData newBitmapData;
    newBitmap->LockBits(&newRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &newBitmapData);
    BYTE* newPixels = (BYTE*) (void*) newBitmapData.Scan0;

    Gdiplus::BitmapData oldBitmapData;
    oldBitmap->LockBits(&oldRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &oldBitmapData);
    BYTE* oldPixels = (BYTE*) (void*) oldBitmapData.Scan0;

    auto top = crop.GetTop();
    auto leftMult = crop.GetLeft() * 4;
    auto oldWidth = oldBitmap->GetWidth();
    auto oldWidthBytes = oldWidth * 4;
    auto copyBytes = copyWidth * 4;
    auto newWidthMult = newWidth * 4;
    auto newHeightMult = newHeight * 4;

    BYTE* oldRowPtr = oldPixels + leftMult + top * oldWidthBytes;

    for(int x = 0; x < newHeightMult; x += 4){
        BYTE* srcRow = oldRowPtr + x;
        BYTE* dstRow = newPixels + x;

        std::memcpy(dstRow, srcRow, copyBytes);
    }

	delete m_image;
	m_image = newBitmap;
}