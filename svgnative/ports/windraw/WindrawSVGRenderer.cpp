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

#include "WindrawSVGRenderer.h"
#include "Config.h"

#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace SVGNative
{
WindrawSVGPath::WindrawSVGPath()
{
    mPath = wdCreatePath(nullptr);
    SVG_ASSERT(mPath);
}

WindrawSVGPath::~WindrawSVGPath()
{
    SVG_ASSERT(mPath);
    ClosePathSink();
    wdDestroyPath(mPath);
}

void WindrawSVGPath::Rect(float x, float y, float width, float height)
{
    RoundedRect(x, y, width, height, 0);
}

void WindrawSVGPath::RoundedRect(float x, float y, float width, float height, float cornerRadius)
{
    SVG_ASSERT(!mHasOpenFigure);
    SVG_ASSERT(!mHasOpenPathSink);
    if (mPath)
    {
        wdDestroyPath(mPath);
        mPath = nullptr;
        mHasOpenFigure = false;
    }
    WD_RECT rect{};
    rect.x0 = x;
    rect.y0 = y;
    rect.x1 = x + width;
    rect.y1 = y + height;
    mPath = wdCreateRoundedRectPath(nullptr, &rect, cornerRadius);
}

void WindrawSVGPath::Ellipse(float cx, float cy, float rx, float ry)
{
    SVG_ASSERT(!mHasOpenFigure);
    SVG_ASSERT(!mHasOpenPathSink);
    wdOpenPathSink(&mSink, mPath);
    wdBeginFigure(&mSink, mCurrentX, mCurrentY);
    wdAddArc(&mSink, cx, cx, 360);
    wdEndFigure(&mSink, TRUE);
    wdClosePathSink(&mSink);
}

void WindrawSVGPath::MoveTo(float x, float y)
{
    if (mHasOpenFigure)
    {
        SVG_ASSERT(mHasOpenPathSink);
        wdEndFigure(&mSink, FALSE);
        mHasOpenFigure = false;
    }
    if (!mHasOpenPathSink)
    {
        wdOpenPathSink(&mSink, mPath);
        mHasOpenPathSink = true;
    }
    wdBeginFigure(&mSink, x, y);
    mCurrentX = x;
    mCurrentY = y;
}

void WindrawSVGPath::LineTo(float x, float y)
{
    if (!mHasOpenFigure)
        MoveTo(x, y);
    wdAddLine(&mSink, x, y);
}

void WindrawSVGPath::CurveTo(float x1, float y1, float x2, float y2, float x3, float y3)
{
    if (!mHasOpenFigure)
        MoveTo(x1, y1);
    wdAddBezier(&mSink, x1, y1, x2, y2, x3, y3);
    mCurrentX = x3;
    mCurrentY = y3;
}

void WindrawSVGPath::CurveToV(float x2, float y2, float x3, float y3)
{
    CurveTo(mCurrentX, mCurrentY, x2, y2, x3, y3);
}

void WindrawSVGPath::ClosePath()
{
    if (!mHasOpenFigure)
        MoveTo(mCurrentX, mCurrentY);

    wdEndFigure(&mSink, TRUE);
    wdClosePathSink(&mSink);
    mHasOpenFigure = false;
}

void WindrawSVGPath::ClosePathSink()
{
    if (mHasOpenFigure)
    {
        SVG_ASSERT(mHasOpenPathSink);
        wdEndFigure(&mSink, FALSE);
        mHasOpenFigure = false;
    }
    if (mHasOpenPathSink)
    {
        wdClosePathSink(&mSink);
        mHasOpenPathSink = false;        
    }
}

const WD_HPATH WindrawSVGPath::Path()
{
    SVG_ASSERT(mPath);
    ClosePathSink();
    return mPath;
}

WindrawSVGTransform::WindrawSVGTransform(float a, float b, float c, float d, float tx, float ty) { Set(a, b, c, d, tx, ty); }

void WindrawSVGTransform::Set(float a, float b, float c, float d, float tx, float ty)
{
    mTransform.m11 = a;
    mTransform.m12 = b;
    mTransform.m21 = c;
    mTransform.m22 = d;
    mTransform.dx = tx;
    mTransform.dy = ty;
}

float deg2rad(float angle);
float deg2rad(float angle) { return static_cast<float>(M_PI / 180.0 * angle); }

void WindrawSVGTransform::Rotate(float r)
{
    r = deg2rad(r);
    float cosAngle = cos(r);
    float sinAngle = sin(r);

    auto rot = WD_MATRIX{cosAngle, sinAngle, -sinAngle, cosAngle, 0, 0};
    Multiply(rot);
}

void WindrawSVGTransform::Translate(float tx, float ty)
{
    mTransform.dx += tx * mTransform.m11 + ty * mTransform.m21;
    mTransform.dy += tx * mTransform.m12 + ty * mTransform.m22;
}

void WindrawSVGTransform::Scale(float sx, float sy)
{
    mTransform.m11 *= sx;
    mTransform.m12 *= sx;
    mTransform.m21 *= sy;
    mTransform.m22 *= sy;
}

void WindrawSVGTransform::Concat(const Transform& other) { Multiply(static_cast<const WindrawSVGTransform&>(other).mTransform); }

void WindrawSVGTransform::Multiply(const WD_MATRIX& o)
{
    WD_MATRIX newT;
    newT.m11 = o.m11 * mTransform.m11 + o.m12 * mTransform.m21;
    newT.m12 = o.m11 * mTransform.m12 + o.m12 * mTransform.m22;
    newT.m21 = o.m21 * mTransform.m11 + o.m22 * mTransform.m21;
    newT.m22 = o.m21 * mTransform.m12 + o.m22 * mTransform.m22;
    newT.dx = o.dx * mTransform.m11 + o.dy * mTransform.m21 + mTransform.dx;
    newT.dy = o.dx * mTransform.m12 + o.dy * mTransform.m22 + mTransform.dy;
    mTransform = newT;
}

WindrawSVGImageData::WindrawSVGImageData(const std::string& base64, ImageEncoding /*encoding*/)
{
}

float WindrawSVGImageData::Width() const
{
	return 0;
}

float WindrawSVGImageData::Height() const
{
	return 0;
}

WindrawSVGRenderer::WindrawSVGRenderer() {}

void WindrawSVGRenderer::Save(const GraphicStyle& graphicStyle)
{
}

void WindrawSVGRenderer::Restore()
{
}

void WindrawSVGRenderer::DrawPath(
    const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle)
{
    SVG_ASSERT(mCanvas);
    const auto& constPPath = static_cast<const WindrawSVGPath&>(path);
    auto pPath = const_cast<WindrawSVGPath&>(constPPath);
    Save(graphicStyle);
    if (fillStyle.hasFill)
    {
        auto hBrush = CreateBrush(fillStyle.paint);
        wdFillPath(mCanvas, hBrush, pPath.Path());
        wdDestroyBrush(hBrush);
    }
    if (strokeStyle.hasStroke)
    {
        auto hBrush = CreateBrush(strokeStyle.paint);
        UINT lineCap{};
        switch (strokeStyle.lineCap)
        {
        case LineCap::kButt:
            lineCap = WD_LINECAP_FLAT;
            break;
        case LineCap::kRound:
            lineCap = WD_LINECAP_ROUND;
            break;
        case LineCap::kSquare:
            lineCap = WD_LINECAP_SQUARE;
            break;
        }

        UINT lineJoin{};
        switch (strokeStyle.lineJoin)
        {
        case LineJoin::kBevel:
            lineJoin = WD_LINEJOIN_BEVEL;
            break;
        case LineJoin::kMiter:
            lineJoin = WD_LINEJOIN_MITER;
            break;
        case LineJoin::kRound:
            lineJoin = WD_LINEJOIN_ROUND;
            break;
        }

        WD_HSTROKESTYLE hStrokeStyle{};
        if (!strokeStyle.dashArray.empty())
            hStrokeStyle = wdCreateStrokeStyleCustom(strokeStyle.dashArray.data(), static_cast<INT>(strokeStyle.dashArray.size()), lineCap, lineJoin);
        else
            hStrokeStyle = wdCreateStrokeStyle(WD_DASHSTYLE_SOLID, lineCap, lineJoin);
        wdDrawPath(mCanvas, hBrush, pPath.Path(), strokeStyle.lineWidth);
        wdDestroyStrokeStyle(hStrokeStyle);
        wdDestroyBrush(hBrush);
    }

    Restore();
}

WD_HBRUSH WindrawSVGRenderer::CreateBrush(const Paint& paint)
{
    SVG_ASSERT(mCanvas);
    if (paint.type() == typeid(Color))
    {
        const auto& color = boost::get<Color>(paint);
        auto argb = WD_ARGB(color[3] * 255.0f, color[0] * 255.0f, color[1] * 255.0f, color[2] * 255.0f);
        return wdCreateSolidBrush(mCanvas, argb);
    }
    else if (paint.type() == typeid(Gradient))
    {
        const auto& gradient = boost::get<Gradient>(paint);
        auto size = gradient.colorStops.size();
        std::vector<WD_COLOR> colors;
        std::vector<float> offsets;
        for (const auto& stop : gradient.colorStops)
        {
            offsets.push_back(stop.first);
            const auto& color = stop.second;
            colors.push_back(WD_ARGB(color[3] * 255.0f, color[0] * 255.0f, color[1] * 255.0f, color[2] * 255.0f));
        }
        if (gradient.type == GradientType::kLinearGradient)
        {
            return wdCreateLinearGradientBrushEx(mCanvas, gradient.x1, gradient.y1, gradient.x2, gradient.y2,
                colors.data(), offsets.data(), size);
        }
        else
        {
            return wdCreateRadialGradientBrushEx(mCanvas, gradient.cx, gradient.cy, gradient.r, gradient.fx, gradient.fy,
                colors.data(), offsets.data(), size);
        }
    }
    else
    {
        SVG_ASSERT_MSG(false, "Unknown paint type");
    }
    return nullptr;
}

void WindrawSVGRenderer::DrawImage(
    const ImageData& image, const GraphicStyle& graphicStyle, const Rect& clipArea, const Rect& fillArea)
{
    Save(graphicStyle);
    Restore();
}

} // namespace SVGNative
