#include "DPIScale.h"

float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;

void DPIScale::Initialize(ID2D1Factory* pFactory) {
    FLOAT dpiX = 0.0f, dpiY = 0.0f;
    HDC hdc = GetDC(NULL);
    if (hdc) {
        dpiX = static_cast<float>(GetDeviceCaps(hdc, LOGPIXELSX));
        dpiY = static_cast<float>(GetDeviceCaps(hdc, LOGPIXELSY));
        ReleaseDC(NULL, hdc);
    }

    scaleX = dpiX / 96.0f;
    scaleY = dpiY / 96.0f;
}
