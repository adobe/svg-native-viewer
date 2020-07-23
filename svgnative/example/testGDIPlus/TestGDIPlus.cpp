/*
Copyright 2019 Adobe. All rights reserved.
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
#include <Gdiplus.h>
#include <memory>

#include "svgnative/SVGDocument.h"
#include "svgnative/ports/gdiplus/GDIPlusSVGRenderer.h"

/* We need to include the Gdiplus lib */
#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;
using namespace SVGNative;

static HWND hwndMain = NULL;

static const std::string gSVGString = "<svg viewBox=\"0 0 200 200\"><circle cx=\"100\" cy=\"100\" r=\"100\" fill=\"yellow\"/></svg>";

static void
MainWinPaintToCanvas(HDC hdc)
{
    Graphics graphics(hdc);

    auto renderer = std::shared_ptr<GDIPlusSVGRenderer>(new GDIPlusSVGRenderer);
    renderer->SetGraphicsContext(&graphics);

    auto svgDocument = SVGDocument::CreateSVGDocument(gSVGString.c_str(), renderer);
    svgDocument->Render();
}

/* Main window procedure */
static LRESULT CALLBACK
MainWinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg) {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            HDC hdc;
            hdc = BeginPaint(hwndMain, &ps);
            MainWinPaintToCanvas(hdc);
            EndPaint(hwndMain, &ps);
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

    GdiplusStartupInput gdiplusStartupInput;

    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

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

    GdiplusShutdown(gdiplusToken);

    /* Return exit code of WM_QUIT */
    return (int)msg.wParam;
}
