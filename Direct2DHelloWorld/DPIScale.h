#pragma once

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

class DPIScale {
    static float scaleX;
    static float scaleY;

 public:
    static void Initialize(ID2D1Factory *pFactory);

    template <typename T>
    static D2D1_POINT_2F PixelsToDips(T x, T y) {
        return D2D1::Point2F(static_cast<float>(x) / scaleX, static_cast<float>(y) / scaleY);
    }
};
