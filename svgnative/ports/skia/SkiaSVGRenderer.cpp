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

#include "SkiaSVGRenderer.h"
#include "base64.h"
#include "Config.h"
#include "SkCanvas.h"
#include "SkCodec.h"
#include "SkData.h"
#include "SkEncodedOrigin.h"
#include "SkGradientShader.h"
#include "SkImage.h"
#include "SkPoint.h"
#include "SkRect.h"
#include "SkRRect.h"
#include "SkShader.h"
#include "SkSurface.h"
#include "SkDashPathEffect.h"
#include <math.h>

namespace SVGNative
{
SkiaSVGPath::SkiaSVGPath() {}

void SkiaSVGPath::Rect(float x, float y, float width, float height) { mPath.addRect({x, y, x + width, y + height}); }

void SkiaSVGPath::RoundedRect(float x, float y, float width, float height, float cornerRadius)
{
    SkRRect rrect;
    rrect.setNinePatch({x, y, x + width, y + height}, cornerRadius, cornerRadius, cornerRadius, cornerRadius);
    mPath.addRRect(rrect);
}

void SkiaSVGPath::RoundedRect(float x, float y, float width, float height, float cornerRadiusX, float cornerRadiusY)
{
    SkRRect rrect;
    rrect.setRectXY({x, y, x + width, y + height}, cornerRadiusX, cornerRadiusY);
    mPath.addRRect(rrect);
}

void SkiaSVGPath::Ellipse(float cx, float cy, float rx, float ry) { mPath.addOval({cx - rx, cy - ry, cx + rx, cy + ry}); }

void SkiaSVGPath::MoveTo(float x, float y)
{
    mPath.moveTo(x, y);
    mCurrentX = x;
    mCurrentY = y;
}

void SkiaSVGPath::LineTo(float x, float y)
{
    mPath.lineTo(x, y);
    mCurrentX = x;
    mCurrentY = y;
}

void SkiaSVGPath::CurveTo(float x1, float y1, float x2, float y2, float x3, float y3)
{
    mPath.cubicTo(x1, y1, x2, y2, x3, y3);
    mCurrentX = x3;
    mCurrentY = y3;
}

void SkiaSVGPath::CurveToV(float x2, float y2, float x3, float y3)
{
    mPath.cubicTo(mCurrentX, mCurrentY, x2, y2, x3, y3);
    mCurrentX = x3;
    mCurrentY = y3;
}

void SkiaSVGPath::ClosePath() { mPath.close(); }

SkiaSVGTransform::SkiaSVGTransform(float a, float b, float c, float d, float tx, float ty) { Set(a, b, c, d, tx, ty); }

void SkiaSVGTransform::Set(float a, float b, float c, float d, float tx, float ty) { mMatrix.setAll(a, c, tx, b, d, ty, 0, 0, 1); }

void SkiaSVGTransform::Rotate(float degree) { mMatrix.preRotate(degree); }

void SkiaSVGTransform::Translate(float tx, float ty) { mMatrix.preTranslate(tx, ty); }

void SkiaSVGTransform::Scale(float sx, float sy) { mMatrix.preScale(sx, sy, 0.0, 0.0); }

void SkiaSVGTransform::Concat(const Transform& other) { mMatrix.preConcat(static_cast<const SkiaSVGTransform&>(other).mMatrix); }

inline sk_sp<SkImage> getOrientedImage(sk_sp<SkImage> srcImg, SkEncodedOrigin origin)
{
    int width = 0, height = 0;
    int offsetX = 0, offsetY = 0;
    int rot = 0;
    int centerX = 0, centerY = 0;

    switch (origin)
    {
    case SkEncodedOrigin::kBottomRight_SkEncodedOrigin:
        width = srcImg->width();
        height = srcImg->height();
        rot = 180;
        centerX = width / 2;
        centerY = height / 2;
        break;

    case SkEncodedOrigin::kLeftBottom_SkEncodedOrigin:
        width = srcImg->height();
        height = srcImg->width();
        offsetX = width;
        rot = 90;
        break;

    case SkEncodedOrigin::kRightTop_SkEncodedOrigin:
        width = srcImg->height();
        height = srcImg->width();
        offsetY = height;
        rot = 270;
        break;

    default:
        return nullptr;
    }

    auto skRasterSurface = SkSurface::MakeRasterN32Premul(width, height);
    auto skRasterCanvas = skRasterSurface->getCanvas();
    if (offsetX != 0 || offsetY != 0)
        skRasterCanvas->translate(offsetX, offsetY);
    if (rot != 0)
    {
        if (centerX != 0 || centerY != 0)
            skRasterCanvas->rotate(rot, centerX, centerY);
        else
            skRasterCanvas->rotate(rot);
    }
    skRasterCanvas->drawImage(srcImg, 0, 0);
    return skRasterSurface->makeImageSnapshot();
}

SkiaSVGImageData::SkiaSVGImageData(const std::string& base64, ImageEncoding /*encoding*/)
{
    std::string imageString = base64_decode(base64);
    auto skData = SkData::MakeWithCopy(imageString.data(), imageString.size());
    SkEncodedOrigin origin = SkCodec::MakeFromData(skData, nullptr)->getOrigin();
    if (origin == SkEncodedOrigin::kTopLeft_SkEncodedOrigin)
        mImageData = SkImage::MakeFromEncoded(skData);
    else
    {
        auto rawImg = SkImage::MakeFromEncoded(skData);
        mImageData = getOrientedImage(rawImg, origin);
    }
}

float SkiaSVGImageData::Width() const
{
    if (!mImageData)
        return 0;
    return static_cast<float>(mImageData->width());
}

float SkiaSVGImageData::Height() const
{
    if (!mImageData)
        return 0;
    return static_cast<float>(mImageData->height());
}

SkiaSVGRenderer::SkiaSVGRenderer() {}

void SkiaSVGRenderer::Save(const GraphicStyle& graphicStyle)
{
    SVG_ASSERT(mCanvas);
    if (graphicStyle.opacity != 1.0)
        mCanvas->saveLayerAlpha(nullptr, graphicStyle.opacity);
    else
        mCanvas->save();
    if (graphicStyle.transform)
        mCanvas->concat(static_cast<SkiaSVGTransform*>(graphicStyle.transform.get())->mMatrix);
    if (graphicStyle.clippingPath && graphicStyle.clippingPath->path)
    {
        SkPath clippingPath(static_cast<const SkiaSVGPath*>(graphicStyle.clippingPath->path.get())->mPath);
        if (graphicStyle.clippingPath->transform)
        {
            const auto& matrix = static_cast<const SkiaSVGTransform*>(graphicStyle.clippingPath->transform.get())->mMatrix;
            clippingPath.transform(matrix);
        }
		clippingPath.setFillType(graphicStyle.clippingPath->clipRule == WindingRule::kNonZero ? SkPath::kWinding_FillType : SkPath::kEvenOdd_FillType);
        mCanvas->clipPath(clippingPath);
    }
}

void SkiaSVGRenderer::Restore()
{
    SVG_ASSERT(mCanvas);
    mCanvas->restore();
}

inline void CreateSkPaint(const Paint& paint, float opacity, SkPaint& skPaint)
{
    if (paint.type() == typeid(Color))
    {
        const auto& color = boost::get<Color>(paint);
        skPaint.setColor(SkColorSetARGB(static_cast<uint8_t>(opacity * color[3] * 255), static_cast<uint8_t>(color[0] * 255),
            static_cast<uint8_t>(color[1] * 255), static_cast<uint8_t>(color[2] * 255)));
    }
    else if (paint.type() == typeid(Gradient))
    {
        const auto& gradient = boost::get<Gradient>(paint);
        std::vector<SkColor> colors;
        std::vector<SkScalar> pos;
        for (const auto& stop : gradient.colorStops)
        {
            pos.push_back(stop.first);
            const auto& stopColor = stop.second;
            colors.push_back(SkColorSetARGB(static_cast<uint8_t>(opacity * stopColor[3] * 255), static_cast<uint8_t>(stopColor[0] * 255),
                static_cast<uint8_t>(stopColor[1] * 255), static_cast<uint8_t>(stopColor[2] * 255)));
        }
        SkShader::TileMode mode;
        switch (gradient.method)
        {
        case SpreadMethod::kReflect:
            mode = SkShader::TileMode::kMirror_TileMode;
            break;
        case SpreadMethod::kRepeat:
            mode = SkShader::TileMode::kRepeat_TileMode;
            break;
        case SpreadMethod::kPad:
        default:
            mode = SkShader::TileMode::kClamp_TileMode;
            break;
        }
        SkMatrix* matrix{};
        if (gradient.transform)
            matrix = &(static_cast<SkiaSVGTransform*>(gradient.transform.get())->mMatrix);
        if (gradient.type == GradientType::kLinearGradient)
        {
            SkPoint points[2] = {SkPoint::Make(gradient.x1, gradient.y1), SkPoint::Make(gradient.x2, gradient.y2)};
            skPaint.setShader(
                SkGradientShader::MakeLinear(points, colors.data(), pos.data(), static_cast<int>(colors.size()), mode, 0, matrix));
        }
        else if (gradient.type == GradientType::kRadialGradient)
        {
            skPaint.setShader(
                SkGradientShader::MakeTwoPointConical(SkPoint::Make(gradient.fx, gradient.fy), 0, SkPoint::Make(gradient.cx, gradient.cy),
                    gradient.r, colors.data(), pos.data(), static_cast<int>(colors.size()), mode, 0, matrix));
        }
    }
}

void SkiaSVGRenderer::DrawPath(
    const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle)
{
    SVG_ASSERT(mCanvas);
    Save(graphicStyle);
    if (fillStyle.hasFill)
    {
        SkPaint fill;
        fill.setStyle(SkPaint::kFill_Style);
        CreateSkPaint(fillStyle.paint, fillStyle.fillOpacity, fill);
        SkPath mPath = (static_cast<const SkiaSVGPath&>(path).mPath);
        mPath.setFillType(fillStyle.fillRule == WindingRule::kNonZero ? SkPath::kWinding_FillType : SkPath::kEvenOdd_FillType);
        mCanvas->drawPath(mPath, fill);
    }
    if (strokeStyle.hasStroke)
    {
        SkPaint stroke;
        stroke.setStyle(SkPaint::kStroke_Style);
        stroke.setStrokeWidth(strokeStyle.lineWidth);
        if (!strokeStyle.dashArray.empty())
        {
            stroke.setPathEffect(SkDashPathEffect::Make((SkScalar*)(strokeStyle.dashArray.data()),
                                                        strokeStyle.dashArray.size(),
                                                        (SkScalar)strokeStyle.dashOffset));
        }
        CreateSkPaint(strokeStyle.paint, strokeStyle.strokeOpacity, stroke);
        mCanvas->drawPath(static_cast<const SkiaSVGPath&>(path).mPath, stroke);
    }
    Restore();
}

void SkiaSVGRenderer::DrawImage(
    const ImageData& image, const GraphicStyle& graphicStyle, const Rect& clipArea, const Rect& fillArea)
{
    SVG_ASSERT(mCanvas);
    Save(graphicStyle);
    mCanvas->clipRect({clipArea.x, clipArea.y, clipArea.x + clipArea.width, clipArea.y + clipArea.height}, SkClipOp::kIntersect);
    mCanvas->drawImageRect(static_cast<const SkiaSVGImageData&>(image).mImageData,
        {fillArea.x, fillArea.y, fillArea.x + fillArea.width, fillArea.y + fillArea.height}, nullptr);
    Restore();
}

void SkiaSVGRenderer::SetSkCanvas(SkCanvas* canvas)
{
    SVG_ASSERT(canvas);
    mCanvas = canvas;
}

} // namespace SVGNative
