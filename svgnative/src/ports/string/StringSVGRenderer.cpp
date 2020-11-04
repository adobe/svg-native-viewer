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

#include "svgnative/ports/string/StringSVGRenderer.h"

#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace SVGNative
{
StringSVGPath::StringSVGPath() { mStringStream.precision(3); }

void StringSVGPath::Rect(float x, float y, float width, float height)
{
    mStringStream << " Rect(" << x << ',' << y << ',' << width << ',' << height << ')';
}

void StringSVGPath::RoundedRect(float x, float y, float width, float height, float rx, float ry)
{
    mStringStream << " RoundedRect(" << x << ',' << y << ',' << width << ',' << height << ',' << rx << ',' << ry << ')';
}

void StringSVGPath::Ellipse(float cx, float cy, float rx, float ry)
{
    mStringStream << " Ellipse(" << cx << ',' << cy << ',' << rx << ',' << ry << ')';
}

void StringSVGPath::MoveTo(float x, float y) { mStringStream << " M" << x << ',' << y; }

void StringSVGPath::LineTo(float x, float y) { mStringStream << " L" << x << ',' << y; }

void StringSVGPath::CurveTo(float x1, float y1, float x2, float y2, float x3, float y3)
{
    mStringStream << " C" << x1 << ',' << y1 << ',' << x2 << ',' << y2 << ',' << x3 << ',' << y3;
}

void StringSVGPath::CurveToV(float x2, float y2, float x3, float y3) { mStringStream << " Q" << x2 << ',' << y2 << ',' << x3 << ',' << y3; }

void StringSVGPath::ClosePath() { mStringStream << " Z"; }

std::string StringSVGPath::String() const { return mStringStream.str(); }

float deg2rad(float angle);
float deg2rad(float angle) { return static_cast<float>(M_PI / 180.0 * angle); }

StringSVGTransform::StringSVGTransform(float a, float b, float c, float d, float tx, float ty) { Set(a, b, c, d, tx, ty); }

void StringSVGTransform::Set(float a, float b, float c, float d, float tx, float ty) { mTransform = {a, b, c, d, tx, ty}; }

void StringSVGTransform::Rotate(float r)
{
    r = deg2rad(r);
    float cosAngle = cos(r);
    float sinAngle = sin(r);

    auto rot = AffineTransform{cosAngle, sinAngle, -sinAngle, cosAngle, 0, 0};
    Multiply(rot);
}

void StringSVGTransform::Translate(float tx, float ty)
{
    mTransform.e += tx * mTransform.a + ty * mTransform.c;
    mTransform.f += tx * mTransform.b + ty * mTransform.d;
}

void StringSVGTransform::Scale(float sx, float sy)
{
    mTransform.a *= sx;
    mTransform.b *= sx;
    mTransform.c *= sy;
    mTransform.d *= sy;
}

void StringSVGTransform::Concat(float a, float b, float c, float d, float tx, float ty)
{
    AffineTransform other{a, b, c, d, tx, ty};
    Multiply(other);
}

std::string StringSVGTransform::String() const
{
    std::ostringstream stringStream;
    stringStream.precision(3);
    stringStream << "matrix(" << mTransform.a << ',' << mTransform.b << ',' << mTransform.c << ',' << mTransform.d << ',' << mTransform.e
                 << ',' << mTransform.f << ')';
    return stringStream.str();
}

void StringSVGTransform::Multiply(const AffineTransform& o)
{
    AffineTransform newT;
    newT.a = o.a * mTransform.a + o.b * mTransform.c;
    newT.b = o.a * mTransform.b + o.b * mTransform.d;
    newT.c = o.c * mTransform.a + o.d * mTransform.c;
    newT.d = o.c * mTransform.b + o.d * mTransform.d;
    newT.e = o.e * mTransform.a + o.f * mTransform.c + mTransform.e;
    newT.f = o.e * mTransform.b + o.f * mTransform.d + mTransform.f;
    mTransform = newT;
}

StringSVGRenderer::StringSVGRenderer() { mStringStream.precision(3); }

std::unique_ptr<Path> StringSVGRenderer::CreatePath()
{
    return std::unique_ptr<StringSVGPath>(new StringSVGPath);
}

std::unique_ptr<Transform> StringSVGRenderer::CreateTransform(
    float a, float b, float c, float d, float tx, float ty)
{
    return std::unique_ptr<StringSVGTransform>(new StringSVGTransform(a, b, c, d, tx, ty));
}

void StringSVGRenderer::Save(const GraphicStyle& graphicStyle)
{
    WriteIndent();
    mStringStream << "[group";
    WriteGraphic(graphicStyle);
    IncIndent();
    WriteNewline();
}

void StringSVGRenderer::Restore()
{
    DecIndent();
    WriteIndent();
    mStringStream << ']';
    WriteNewline();
}

void StringSVGRenderer::DrawPath(
    const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle)
{
    WriteIndent();
    mStringStream << "[path" << static_cast<const StringSVGPath&>(path).String();
    WriteGraphic(graphicStyle);

    IncIndent();
    WriteNewline();
    WriteIndent();

    WriteFill(fillStyle);
    WriteNewline();
    WriteIndent();

    WriteStroke(strokeStyle);
    mStringStream << ']';
    DecIndent();
    WriteNewline();
}

void StringSVGRenderer::DrawImage(const ImageData& image, const GraphicStyle& graphicStyle, const Rect& clipArea, const Rect& fillArea)
{
    WriteIndent();
    mStringStream << "[image ";
    mStringStream << "clip(" << clipArea.x << ", " << clipArea.y << ", " << clipArea.width << ", " << clipArea.height << ") ";
    mStringStream << "fill(" << fillArea.x << ", " << fillArea.y << ", " << fillArea.width << ", " << fillArea.height << ") ";
    WriteGraphic(graphicStyle);
    mStringStream << ' ' << static_cast<const StringSVGImageData&>(image).String() << ']';
    WriteNewline();
}

std::string StringSVGRenderer::String() const { return mStringStream.str(); }

void StringSVGRenderer::IncIndent() { mIndent += 4; }

void StringSVGRenderer::DecIndent() { mIndent -= 4; }

void StringSVGRenderer::WriteNewline() { mStringStream << '\n'; }

void StringSVGRenderer::WriteIndent()
{
    for (size_t i = 0; i < mIndent; ++i)
        mStringStream << ' ';
}

void StringSVGRenderer::WriteFill(const FillStyle& fillStyle)
{
    mStringStream << "fill: {";
    mStringStream << "hasFill: " << (fillStyle.hasFill ? "true" : "false");
    mStringStream << " winding: " << (fillStyle.fillRule == WindingRule::kNonZero ? "nonzero" : "evenodd");
    if (fillStyle.fillOpacity != 1.0)
        mStringStream << " opacity: " << fillStyle.fillOpacity;
    WritePaint(fillStyle.paint);
    mStringStream << '}';
}

void StringSVGRenderer::WriteStroke(const StrokeStyle& strokeStyle)
{
    mStringStream << "stroke: {";
    mStringStream << "hasStroke: " << (strokeStyle.hasStroke ? "true" : "false");
    mStringStream << " width: " << strokeStyle.lineWidth;
    if (strokeStyle.strokeOpacity != 1.0)
        mStringStream << " opacity: " << strokeStyle.strokeOpacity;
    mStringStream << " cap: ";
    if (strokeStyle.lineCap == LineCap::kButt)
        mStringStream << "butt";
    else if (strokeStyle.lineCap == LineCap::kRound)
        mStringStream << "round";
    else if (strokeStyle.lineCap == LineCap::kSquare)
        mStringStream << "square";
    mStringStream << " join: ";
    if (strokeStyle.lineJoin == LineJoin::kMiter)
        mStringStream << "miter";
    else if (strokeStyle.lineJoin == LineJoin::kRound)
        mStringStream << "round";
    else if (strokeStyle.lineJoin == LineJoin::kBevel)
        mStringStream << "bevel";
    mStringStream << " miter: " << strokeStyle.miterLimit;
    if (!strokeStyle.dashArray.empty())
    {
        mStringStream << " dash:";
        for (auto dash : strokeStyle.dashArray)
            mStringStream << ' ' << dash;
    }
    mStringStream << " dashOffset: " << strokeStyle.dashOffset;
    WritePaint(strokeStyle.paint);
    mStringStream << '}';
}

void StringSVGRenderer::WriteGraphic(const GraphicStyle& graphicStyle)
{
    if (graphicStyle.opacity != 1.0)
        mStringStream << " opacity: " << graphicStyle.opacity;
    if (graphicStyle.transform)
        mStringStream << " transform: " << static_cast<StringSVGTransform*>(graphicStyle.transform.get())->String();
    if (graphicStyle.clippingPath && graphicStyle.clippingPath->path)
    {
        mStringStream << " clipping: {";
        mStringStream << " winding: " << (graphicStyle.clippingPath->clipRule == WindingRule::kNonZero ? "nonzero" : "evenodd");
        if (graphicStyle.clippingPath->transform)
            mStringStream << " transform: " << static_cast<StringSVGTransform*>(graphicStyle.clippingPath->transform.get())->String();
        mStringStream <<  " [path" << static_cast<const StringSVGPath*>(graphicStyle.clippingPath->path.get())->String();
        mStringStream << "]}";
    }
}

void StringSVGRenderer::WritePaint(const Paint& paint)
{
    // FIXME: use static_visitor
    if (paint.type() == typeid(Gradient))
    {
        auto gradient = boost::get<Gradient>(paint);
        mStringStream << " paint: {";
        WriteNewline();
        IncIndent();
        WriteIndent();
        mStringStream << (gradient.type == GradientType::kLinearGradient ? "linearGradient:" : "radialGradient:");
        if (gradient.transform)
            mStringStream << " transform: " << static_cast<StringSVGTransform*>(gradient.transform.get())->String();
        if (gradient.type == GradientType::kLinearGradient)
        {
            if (std::isfinite(gradient.x1))
                mStringStream << " x1: " << gradient.x1;
            if (std::isfinite(gradient.y1))
                mStringStream << " y1: " << gradient.y1;
            if (std::isfinite(gradient.x2))
                mStringStream << " x2: " << gradient.x2;
            if (std::isfinite(gradient.y2))
                mStringStream << " y2: " << gradient.y2;
        }
        else // GradientType::kRadialGradient
        {
            if (std::isfinite(gradient.cx))
                mStringStream << " cx: " << gradient.cx;
            if (std::isfinite(gradient.cy))
                mStringStream << " cy: " << gradient.cy;
            if (std::isfinite(gradient.fx))
                mStringStream << " fx: " << gradient.fx;
            if (std::isfinite(gradient.fy))
                mStringStream << " fy: " << gradient.fy;
            if (std::isfinite(gradient.r))
                mStringStream << " r: " << gradient.r;
        }
        mStringStream << " method: ";
        if (gradient.method == SpreadMethod::kPad)
            mStringStream << "pad";
        else if (gradient.method == SpreadMethod::kReflect)
            mStringStream << "reflect";
        else if (gradient.method == SpreadMethod::kRepeat)
            mStringStream << "repeat";
        mStringStream << " stops: {";
        WriteNewline();
        IncIndent();
        for (const auto& colorStop : gradient.colorStops)
        {
            WriteIndent();
            const auto& stopColor = colorStop.second;
            mStringStream << "offset: " << colorStop.first;
            mStringStream << " rgba(" << stopColor[0] << ',' << stopColor[1] << ',' << stopColor[2] << ',' << stopColor[3] << ')';
            WriteNewline();
        }
        DecIndent();
        WriteIndent();
        mStringStream << '}';
        mStringStream << '}';
        DecIndent();
    }
    else if (paint.type() == typeid(Color))
    {
        auto color = boost::get<Color>(paint);
        mStringStream << " paint: rgba(" << color[0] << ',' << color[1] << ',' << color[2] << ',' << color[3] << ')';
    }
}
} // namespace SVGNative
