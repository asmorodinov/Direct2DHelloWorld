#pragma once

#include <windows.h>
#include <windowsx.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include "DPIScale.h"

template <class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease) {
    if (*ppInterfaceToRelease != nullptr) {
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = nullptr;
    }
}

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

class DemoApp {
 public:
    DemoApp();
    ~DemoApp();

    HRESULT Initialize();
    void RunMessageLoop();

 private:
    HRESULT CreateDeviceIndependentResources();
    HRESULT CreateDeviceResources();

    void DiscardDeviceResources();

    ID2D1PathGeometry* GenTriangleGeometry(D2D1_POINT_2F pt1, D2D1_POINT_2F pt2, D2D1_POINT_2F pt3);

    void OnLButtonDown(int pixelX, int pixelY, DWORD flags);
    void OnMouseMove(int pixelX, int pixelY, DWORD flags);
    void OnLButtonUp();
    HRESULT OnRender();
    void OnResize(UINT width, UINT height);

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

 private:
    HWND hwnd = nullptr;
    ID2D1Factory* direct2dFactory = nullptr;
    ID2D1HwndRenderTarget* renderTarget = nullptr;

    ID2D1SolidColorBrush* greenBrush = nullptr;
    ID2D1SolidColorBrush* blueBrush = nullptr;
    ID2D1StrokeStyle* strokeStyle = nullptr;

    D2D1_POINT_2F mouseCoords = D2D1::Point2F();
    bool mousePressed = false;
};
