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
#include "SkGradientShader.h"
#include "SkPoint.h"
#include "SkShader.h"
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

SkiaSVGShape::SkiaSVGShape(const Path&, WindingRule) {}

void SkiaSVGShape::Transform(const class Transform&) {}

void SkiaSVGShape::Union(const Shape&) {}

SkiaSVGRenderer::SkiaSVGRenderer() {}

void SkiaSVGRenderer::Save(const GraphicStyle& graphicStyle)
{
    assert(mCanvas);
    if (graphicStyle.opacity != 1.0)
        mCanvas->saveLayerAlpha(nullptr, graphicStyle.opacity);
    else
        mCanvas->save();
    if (graphicStyle.transform)
        mCanvas->concat(static_cast<SkiaSVGTransform*>(graphicStyle.transform.get())->mMatrix);
}

void SkiaSVGRenderer::Restore()
{
    assert(mCanvas);
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
    assert(mCanvas);
    Save(graphicStyle);
    if (fillStyle.hasFill)
    {
        // FIXME: Handle winding rules.
        SkPaint fill;
        fill.setStyle(SkPaint::kFill_Style);
        CreateSkPaint(fillStyle.paint, fillStyle.fillOpacity, fill);
        mCanvas->drawPath(static_cast<const SkiaSVGPath&>(path).mPath, fill);
    }
    if (strokeStyle.hasStroke)
    {
        SkPaint stroke;
        stroke.setStyle(SkPaint::kStroke_Style);
        CreateSkPaint(strokeStyle.paint, strokeStyle.strokeOpacity, stroke);
        mCanvas->drawPath(static_cast<const SkiaSVGPath&>(path).mPath, stroke);
    }
    Restore();
}

void SkiaSVGRenderer::DrawImage(
    const ImageData& /*image*/, const GraphicStyle& /*graphicStyle*/, const Rect& /*clipArea*/, const Rect& /*fillArea*/)
{
}

void SkiaSVGRenderer::SetSkCanvas(SkCanvas* canvas)
{
    assert(canvas);
    mCanvas = canvas;
}

} // namespace SVGNative
