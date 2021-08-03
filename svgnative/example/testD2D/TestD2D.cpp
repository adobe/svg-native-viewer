/*
Copyright 2020 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Wincodec.h> // Windows Imaging Component (WIC)
#include <atlbase.h> // CComPtr
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "Windowscodecs")

#include "basewin.h"
#include "svgnative/SVGDocument.h"
#include "svgnative/ports/d2d/D2DSVGRenderer.h"

namespace
{
const std::string gSVGString = R"SVG(<svg viewBox="0 0 200 200">
    <rect width="20" height="20" fill="yellow"/>
    <g transform="translate(20, 20) scale(2)" opacity="0.5">
    <rect transform="rotate(15)" width="20" height="20" fill="green"/>
    </g>
    <rect x="60" y="60" width="140" height="80" rx="40" ry="30" fill="blue"/>
    <ellipse cx="140" cy="100" rx="40" ry="20" fill="purple"/>
    <image x="60" y="40" width="129" height="24" transform="rotate(-10)" xlink:href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAa4AAAAwCAAAAABshcHBAAAABGdBTUEAAw1AShHhyQAAAHJJREFUeNrt0TERwCAABLAeQ4dOnVGADNTUF0qpgR9Y4RILKTMYyRf0pAU1eYMnuVeV411sRJcudKFLF7rQpQtd6NKFLnTpQhe6dKELXbrQhS5d6EKXLnShSxe60KULXejShS506UIXunShC1260IWuc/2e+TjGRf4i2gAAAABJRU5ErkJggg=="/>
    </svg>)SVG";
}

using namespace SVGNative;

class MainWindow : public BaseWindow<MainWindow>
{
    CComPtr<IWICImagingFactory> pWICFactory;
    CComPtr<ID2D1Factory> pFactory;
    CComPtr<ID2D1HwndRenderTarget> pRenderTarget;

    CComPtr<ID2D1SolidColorBrush> pBrush;

    std::shared_ptr<SVGNative::SVGDocument> pSVGDocument;

    void    CalculateLayout();
    HRESULT CreateGraphicsResources();
    void    DiscardGraphicsResources();
    void    OnPaint();
    void    Resize();

public:

    MainWindow() = default;

    PCWSTR  ClassName() const { return L"SVGRenderer Window Class"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

// Recalculate drawing layout when the size of the window changes.

void MainWindow::CalculateLayout()
{
    if (pRenderTarget)
    {
        // Layout changes (size) for the SVGRenderer are applied in OnPaint()
    }
}

HRESULT MainWindow::CreateGraphicsResources()
{
    HRESULT hr = S_OK;
    if (!pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = pFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &pRenderTarget);

        if (SUCCEEDED(hr))
        {
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &pBrush);
        }

        if (SUCCEEDED(hr))
        {
            if (pSVGDocument)
            {
                auto renderer = static_cast<D2DSVGRenderer*>(pSVGDocument->Renderer());
                renderer->SetGraphicsContext(pWICFactory, pFactory, pRenderTarget);
            }
            else
            {
                auto renderer = std::shared_ptr<D2DSVGRenderer>(new D2DSVGRenderer);
                renderer->SetGraphicsContext(pWICFactory, pFactory, pRenderTarget);
                pSVGDocument = SVGDocument::CreateSVGDocument(gSVGString.c_str(), renderer);
            }

            CalculateLayout();
        }
    }
    return hr;
}

void MainWindow::DiscardGraphicsResources()
{
    pBrush.Release();
    pRenderTarget.Release();
}

void MainWindow::OnPaint()
{
    HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr))
    {
        PAINTSTRUCT ps;
        BeginPaint(m_hwnd, &ps);

        pRenderTarget->BeginDraw();
        pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::LightGray, 0.0f));

        D2D1_SIZE_F size = pRenderTarget->GetSize();
        D2D1_RECT_F clientRect = D2D1::RectF(0.0f, 0.0f, size.width, size.height);
        pRenderTarget->DrawRectangle(clientRect, pBrush);

        D2D1_POINT_2F inset = D2D1::Point2F(15.0f, 15.0f);
        D2D1_RECT_F rect = D2D1::RectF(inset.x, inset.y, size.width - inset.x, size.height - inset.y);
        pRenderTarget->DrawRectangle(rect, pBrush);
        pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(inset.x, inset.y));
        pSVGDocument->Render(size.width - 2 * inset.x, size.height - 2 * inset.y);

        hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
    }
}

void MainWindow::Resize()
{
    if (pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        pRenderTarget->Resize(size);
        CalculateLayout();
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    MainWindow win;

    if (!win.Create(L"D2DSVGRenderer", WS_OVERLAPPEDWINDOW))
    {
        return 0;
    }

    ShowWindow(win.Window(), nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        {
            constexpr D2D1_FACTORY_OPTIONS factoryOptions{ D2D1_DEBUG_LEVEL_NONE };
            HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factoryOptions, &pFactory);
            if (SUCCEEDED(hr))
            {
                CoInitializeEx(NULL, COINIT_MULTITHREADED);
                hr = CoCreateInstance(
                    CLSID_WICImagingFactory1,
                    nullptr,
                    CLSCTX_INPROC_SERVER,
                    IID_IWICImagingFactory,
                    (void**)&pWICFactory);
            }
            return SUCCEEDED(hr) ? 0 : -1;  // Fail CreateWindowEx
        }
    case WM_DESTROY:
        DiscardGraphicsResources();
        pSVGDocument.reset();
        pWICFactory.Release();
        CoUninitialize();
        pFactory.Release();
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        OnPaint();
        return 0;

        // Other messages not shown...

    case WM_SIZE:
        Resize();
        return 0;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
