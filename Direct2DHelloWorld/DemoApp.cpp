#include "DemoApp.h"

DemoApp::DemoApp() {}

DemoApp::~DemoApp() {
    SafeRelease(&direct2dFactory);
    SafeRelease(&renderTarget);
    SafeRelease(&greenBrush);
    SafeRelease(&blueBrush);
    SafeRelease(&strokeStyle);
}

void DemoApp::RunMessageLoop() {
    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

HRESULT DemoApp::Initialize() {
    HRESULT hr;

    hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr)) {
        WNDCLASSEX wcex = {sizeof(WNDCLASSEX)};
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = DemoApp::WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = HINST_THISCOMPONENT;
        wcex.hbrBackground = NULL;
        wcex.lpszMenuName = NULL;
        wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
        wcex.lpszClassName = L"D2DDemoApp";

        RegisterClassEx(&wcex);

        FLOAT dpiX = 0.0f;
        FLOAT dpiY = 0.0f;

        HDC hdc = GetDC(NULL);
        if (hdc) {
            dpiX = static_cast<float>(GetDeviceCaps(hdc, LOGPIXELSX));
            dpiY = static_cast<float>(GetDeviceCaps(hdc, LOGPIXELSY));
            ReleaseDC(NULL, hdc);
        }

        hwnd = CreateWindow(L"D2DDemoApp", L"Direct2D Demo App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                              static_cast<UINT>(ceil(640.f * dpiX / 96.f)), static_cast<UINT>(ceil(480.f * dpiY / 96.f)), NULL, NULL,
                              HINST_THISCOMPONENT, this);
        hr = hwnd ? S_OK : E_FAIL;
        if (SUCCEEDED(hr)) {
            ShowWindow(hwnd, SW_SHOWNORMAL);
            UpdateWindow(hwnd);
        }
    }

    return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources() {
    HRESULT hr = S_OK;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &direct2dFactory);

    if (SUCCEEDED(hr)) {
        DPIScale::Initialize(direct2dFactory);

        hr = direct2dFactory->CreateStrokeStyle(
            D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND, 0.0f,
                                        D2D1_DASH_STYLE_SOLID, 0.0f),
            nullptr, 0, &strokeStyle);
    }

    return hr;
}

HRESULT DemoApp::CreateDeviceResources() {
    HRESULT hr = S_OK;

    if (!renderTarget) {
        RECT rc;
        GetClientRect(hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

        hr = direct2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size),
                                                        &renderTarget);

        if (SUCCEEDED(hr)) hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGreen), &greenBrush);
        if (SUCCEEDED(hr)) hr = renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &blueBrush);
    }

    return hr;
}

void DemoApp::DiscardDeviceResources() {
    SafeRelease(&renderTarget);
    SafeRelease(&greenBrush);
    SafeRelease(&blueBrush);
}

ID2D1PathGeometry *DemoApp::GenTriangleGeometry(D2D1_POINT_2F pt1, D2D1_POINT_2F pt2, D2D1_POINT_2F pt3) {
    ID2D1GeometrySink *pSink = nullptr;
    HRESULT hr;
    ID2D1PathGeometry *m_pPathGeometry = nullptr;

    hr = direct2dFactory->CreatePathGeometry(&m_pPathGeometry);

    if (SUCCEEDED(hr)) {
        hr = m_pPathGeometry->Open(&pSink);

        if (SUCCEEDED(hr)) {
            pSink->BeginFigure(pt1, D2D1_FIGURE_BEGIN_FILLED);
            pSink->AddLine(pt2);
            pSink->AddLine(pt3);
            pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

            hr = pSink->Close();
        }
        SafeRelease(&pSink);
    }

    return m_pPathGeometry;
}

void DemoApp::OnLButtonDown(int pixelX, int pixelY, DWORD flags) {
    SetCapture(hwnd);
    mouseCoords = DPIScale::PixelsToDips(pixelX, pixelY);
    mousePressed = true;

    InvalidateRect(hwnd, NULL, FALSE);
}

void DemoApp::OnMouseMove(int pixelX, int pixelY, DWORD flags) {
    if (flags & MK_LBUTTON) {
        mouseCoords = DPIScale::PixelsToDips(pixelX, pixelY);

        InvalidateRect(hwnd, NULL, FALSE);
    }
}

void DemoApp::OnLButtonUp() {
    mousePressed = false;
    ReleaseCapture();

    InvalidateRect(hwnd, NULL, FALSE);
}

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;

    if (message == WM_CREATE) {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        DemoApp *pDemoApp = (DemoApp *)pcs->lpCreateParams;

        ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pDemoApp));

        result = 1;
    } else {
        DemoApp *pDemoApp = reinterpret_cast<DemoApp *>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

        bool wasHandled = false;

        if (pDemoApp) {
            result = 0;
            wasHandled = true;
            switch (message) {
                case WM_SIZE:
                    pDemoApp->OnResize(LOWORD(lParam), HIWORD(lParam));
                    break;

                case WM_DISPLAYCHANGE:
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;

                case WM_PAINT:
                    pDemoApp->OnRender();
                    ValidateRect(hwnd, NULL);
                    break;

                case WM_LBUTTONDOWN:
                    pDemoApp->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
                    break;

                case WM_LBUTTONUP:
                    pDemoApp->OnLButtonUp();
                    break;

                case WM_MOUSEMOVE:
                    pDemoApp->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
                    break;

                case WM_DESTROY:
                    PostQuitMessage(0);

                    result = 1;
                    wasHandled = true;
                    break;

                default:
                    wasHandled = false;
                    result = 1;
                    break;
            }
        }

        if (!wasHandled) {
            result = DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return result;
}

HRESULT DemoApp::OnRender() {
    HRESULT hr = S_OK;

    hr = CreateDeviceResources();
    if (SUCCEEDED(hr)) {
        renderTarget->BeginDraw();
        renderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        renderTarget->Clear(D2D1::ColorF(0x25854b));

        if (mousePressed) {
            ID2D1PathGeometry *geometry =
                GenTriangleGeometry(D2D1::Point2F(mouseCoords.x, mouseCoords.y), D2D1::Point2F(mouseCoords.x + 100, mouseCoords.y + 100),
                                    D2D1::Point2F(mouseCoords.x - 100, mouseCoords.y + 100));

            renderTarget->DrawGeometry(geometry, greenBrush, 12.0f, strokeStyle);
            renderTarget->FillGeometry(geometry, blueBrush);
            SafeRelease(&geometry);
        }

        hr = renderTarget->EndDraw();
    }

    if (hr == D2DERR_RECREATE_TARGET) {
        hr = S_OK;
        DiscardDeviceResources();
    }

    return hr;
}

void DemoApp::OnResize(UINT width, UINT height) {
    if (renderTarget) {
        renderTarget->Resize(D2D1::SizeU(width, height));
    }
}
