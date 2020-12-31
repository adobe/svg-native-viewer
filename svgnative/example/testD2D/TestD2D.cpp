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

#include <tchar.h>
#include <windows.h>
#include <unknwn.h>
#include <D2d1.h>
#include <memory>

#include "svgnative/SVGDocument.h"
#include "svgnative/ports/d2d/D2DSVGRenderer.h"

/* We need to include the Gdiplus lib */
#pragma comment (lib, "D2d1.lib")

using namespace D2D1;
using namespace SVGNative;

static HWND hwndMain = NULL;

static const std::string gSVGString = "<svg viewBox=\"0 0 200 200\"><rect width=\"20\" height=\"20\" fill=\"yellow\"/><g transform=\"translate(20, 20) scale(2)\" opacity=\"0.5\"><rect transform=\"rotate(15)\" width=\"20\" height=\"20\" fill=\"green\"/></g></svg>";

static void
MainWinPaintToCanvas(HWND hwnd)
{
    // Init D2D and create factory
    constexpr D2D1_FACTORY_OPTIONS factoryOptions{ D2D1_DEBUG_LEVEL_NONE };

    ID2D1Factory* pD2DFactory{};
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);

    // Init render target
    RECT rc;
    GetClientRect(hwnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(
        rc.right - rc.left,
        rc.bottom - rc.top);

    // Create a Direct2D render target.
    ID2D1HwndRenderTarget* pD2DenderTarget{};
    pD2DFactory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hwnd, size),
        &pD2DenderTarget);

    pD2DenderTarget->BeginDraw();

    auto renderer = std::shared_ptr<D2DSVGRenderer>(new D2DSVGRenderer);
    renderer->SetGraphicsContext(pD2DFactory, pD2DenderTarget);

    auto svgDocument = SVGDocument::CreateSVGDocument(gSVGString.c_str(), renderer);
    svgDocument->Render();

    pD2DenderTarget->EndDraw();
}

/* Main window procedure */
static LRESULT CALLBACK
MainWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg) {
        case WM_PAINT:
        {
            MainWinPaintToCanvas(hwnd);
            return 0;
        }

        case WM_PRINTCLIENT:
            PostQuitMessage(0);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int APIENTRY
_tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc = { 0 };
    MSG msg;

    /* Register main window class */
    wc.lpfnWndProc = MainWinProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = _T("main_window");
    RegisterClass(&wc);

    /* Create main window */
    hwndMain = CreateWindow(
        _T("main_window"), _T("LibWinDraw Example: Simple Draw"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 550, 350,
        NULL, NULL, hInstance, NULL
    );
    SendMessage(hwndMain, WM_SETFONT, (WPARAM) GetStockObject(DEFAULT_GUI_FONT),
            MAKELPARAM(TRUE, 0));
    ShowWindow(hwndMain, nCmdShow);

    /* Message loop */
    while(GetMessage(&msg, NULL, 0, 0)) {
        if(IsDialogMessage(hwndMain, &msg))
            continue;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    /* Return exit code of WM_QUIT */
    return (int)msg.wParam;
}
