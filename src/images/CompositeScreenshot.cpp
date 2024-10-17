#include "CompositeScreenshot.h"
#include "Utils.h"

#include <stdexcept>
#include <cstring>

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
	cropImage();
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
    BYTE* transparentPixels = (BYTE*) (void*) transparentBitmapData.Scan0;

    Gdiplus::BitmapData whiteBitmapData;
    whiteShot->LockBits(&rect1, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &whiteBitmapData);
    BYTE* whitePixels = (BYTE*) (void*) whiteBitmapData.Scan0;

    Gdiplus::BitmapData blackBitmapData;
    blackShot->LockBits(&rect1, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &blackBitmapData);
    BYTE* blackPixels = (BYTE*) (void*) blackBitmapData.Scan0;

    bool isOnlyOneMonitorConnected = monitorRects.size() == 1;

    auto width = whiteShot->GetWidth();
    auto height = whiteShot->GetHeight();

    for(int x = 0; x < width; x++){
        for(int y = 0; y < height; y++){
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

            //Setting alpha
            transparentPixels[currentPixel+3] = !isInsideMonitor ? 0 : toByte((blackPixels[currentPixel+2] - whitePixels[currentPixel+2] + 255 + blackPixels[currentPixel+1] - whitePixels[currentPixel+1] + 255 + blackPixels[currentPixel] - whitePixels[currentPixel] + 255) / 3);
            if(transparentPixels[currentPixel+3] > 0){
                transparentPixels[currentPixel+2] = toByte(255 * blackPixels[currentPixel+2] / transparentPixels[currentPixel+3]); //RED
                transparentPixels[currentPixel+1] = toByte(255 * blackPixels[currentPixel+1] / transparentPixels[currentPixel+3]); //GREEN
                transparentPixels[currentPixel] = toByte(255 * blackPixels[currentPixel] / transparentPixels[currentPixel+3]); //BLUE
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
	Gdiplus::Bitmap* croppedBitmap = m_image->Clone(crop, PixelFormatDontCare);
	delete m_image;
	m_image = croppedBitmap;
    
    ensureEvenDimensions();
}

void CompositeScreenshot::ensureEvenDimensions(){
    auto oldBitmap = m_image;

    auto oldHeight = oldBitmap->GetHeight();
    auto oldWidth = oldBitmap->GetWidth();

    if(oldWidth % 2 == 0 && oldHeight % 2 == 0) return;

    auto newWidth = m_image->GetWidth();
    auto newHeight = m_image->GetHeight();

    if(newWidth % 2 != 0) newWidth++;
    if(newHeight % 2 != 0) newHeight++;

    Gdiplus::Bitmap* newBitmap = new Gdiplus::Bitmap(newWidth, newHeight, PixelFormat32bppARGB);

    Gdiplus::Rect oldRect(0, 0, m_image->GetWidth(), m_image->GetHeight());
    Gdiplus::Rect newRect(0, 0, newBitmap->GetWidth(), newBitmap->GetHeight());

    Gdiplus::BitmapData newBitmapData;
    newBitmap->LockBits(&newRect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &newBitmapData);
    BYTE* newPixels = (BYTE*) (void*) newBitmapData.Scan0;

    Gdiplus::BitmapData oldBitmapData;
    oldBitmap->LockBits(&oldRect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &oldBitmapData);
    BYTE* oldPixels = (BYTE*) (void*) oldBitmapData.Scan0;

    for(size_t x = 0; x < oldHeight; x++){
        size_t rowStart = x*oldWidth;
        size_t newRowStart = x*newWidth;

        std::memcpy(newPixels + newRowStart*4, oldPixels + rowStart*4, oldWidth*4);
    }

    newBitmap->UnlockBits(&newBitmapData);
    oldBitmap->UnlockBits(&oldBitmapData);

    delete m_image;
    m_image = newBitmap;
}