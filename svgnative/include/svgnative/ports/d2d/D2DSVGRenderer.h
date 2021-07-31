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

#ifndef SVGViewer_D2DSVGRenderer_h
#define SVGViewer_D2DSVGRenderer_h

#include "svgnative/SVGRenderer.h"
#include <d2d1.h>
#include <Wincodec.h> // Windows Imaging Component (WIC)
#include <stack>
#include <atlbase.h> // CComPtr

namespace SVGNative
{
class D2DSVGPath final : public Path
{
public:
    D2DSVGPath(CComPtr<ID2D1Factory>);
    ~D2DSVGPath();

    void Rect(float x, float y, float width, float height) override;
    void RoundedRect(float x, float y, float width, float height, float rx, float ry) override;
    void Ellipse(float cx, float cy, float rx, float ry) override;

    void MoveTo(float x, float y) override;
    void LineTo(float x, float y) override;
    void CurveTo(float x1, float y1, float x2, float y2, float x3, float y3) override;
    void CurveToV(float x2, float y2, float x3, float y3) override;
    void ClosePath() override;

    CComPtr<ID2D1PathGeometry> GetGraphicsPath();

private:
    void AddArc(float x, float y, float dx, float dy);
    void ClosePathSink();

private:
    CComPtr<ID2D1PathGeometry> mPath;
    CComPtr<ID2D1GeometrySink> mSink;
    bool mHasOpenFigure{ false };
    float mCurrentX{};
    float mCurrentY{};
};

class D2DSVGTransform final : public Transform
{
public:
    D2DSVGTransform(float a, float b, float c, float d, float tx, float ty);

    void Set(float a, float b, float c, float d, float tx, float ty) override;
    void Rotate(float r) override;
    void Translate(float tx, float ty) override;
    void Scale(float sx, float sy) override;
    void Concat(float a, float b, float c, float d, float tx, float ty) override;

    const D2D1::Matrix3x2F& GetMatrix() const;

private:
    D2D1::Matrix3x2F mTransform;
};

class D2DSVGImageData final : public ImageData
{
public:
    D2DSVGImageData(CComPtr<IWICBitmapSource> bitmapSource);
    ~D2DSVGImageData();

    float Width() const override;
    float Height() const override;

    CComPtr<IWICBitmapSource> GetBitmapSource() const;

private:
    float mWidth{};
    float mHeight{};
    CComPtr<IWICBitmapSource> mBitmapSource;
};

class SVG_IMP_EXP D2DSVGRenderer final : public SVGRenderer
{
public:
    D2DSVGRenderer();

    virtual ~D2DSVGRenderer()
    { 
    }

    std::unique_ptr<ImageData> CreateImageData(const std::string& base64, ImageEncoding encoding) override;

    std::unique_ptr<Path> CreatePath() override;

    std::unique_ptr<Transform> CreateTransform(
        float a = 1.0, float b = 0.0, float c = 0.0, float d = 1.0, float tx = 0.0, float ty = 0.0) override;

    void Save(const GraphicStyle& graphicStyle) override;
    void Restore() override;

    void DrawPath(const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle) override;
    void DrawImage(const ImageData& image, const GraphicStyle& graphicStyle, const Rect& clipArea, const Rect& fillArea) override;

    void SetGraphicsContext(IWICImagingFactory* pWICFactory, ID2D1Factory* pD2DFactory, ID2D1RenderTarget* renderTarget)
    {
        mWICFactory = pWICFactory;
        mD2DFactory = pD2DFactory;
        mContext = renderTarget;
    }

    void ReleaseGraphicsContext()
    {
        mD2DFactory.Release();
        mWICFactory.Release();
    }

private:
    CComPtr<ID2D1Brush> D2DSVGRenderer::CreateBrush(const Paint& paint);

    CComPtr<ID2D1RenderTarget> mContext;
    CComPtr<ID2D1Factory> mD2DFactory;
    CComPtr<IWICImagingFactory> mWICFactory;

    std::stack<D2D1_MATRIX_3X2_F> mContextTransform;
};

} // namespace SVGNative

#endif // SVGViewer_D2DSVGRenderer_h
