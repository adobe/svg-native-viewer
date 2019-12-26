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

#include <Windows.h>
#include <gdiplus.h>

#include "Config.h"
#include "GDIPlusSVGRenderer.h"
#include "base64.h"

namespace SVGNative
{

/******************************************************************************/

Gdiplus::Color ColorToGdiplusColor(const Color& inColor)
{
    return Gdiplus::Color(
        static_cast<BYTE>(inColor[3] * 255.0),
        static_cast<BYTE>(inColor[0] * 255.0),
        static_cast<BYTE>(inColor[1] * 255.0),
        static_cast<BYTE>(inColor[2] * 255.0));
}

/******************************************************************************/

GDIPlusSVGPath::GDIPlusSVGPath()
{
}

GDIPlusSVGPath::~GDIPlusSVGPath()
{
}

void GDIPlusSVGPath::Rect(float x, float y, float width, float height)
{
    mPath.AddRectangle(Gdiplus::RectF(x, y, width, height));
}

void GDIPlusSVGPath::RoundedRect(float x, float y, float w, float h, float r)
{
    const float d = r + r;

    mPath.AddLine(x + r, y, x + w - d, y);
    mPath.AddArc(x + w - d, y, d, d, 270, 90);
    mPath.AddLine(x + w, y + r, x + w, y + h - d);
    mPath.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    mPath.AddLine(x + w - d, y + h, x + r, y + h);
    mPath.AddArc(x, y + h - d, d, d, 90, 90);
    mPath.AddLine(x, y + h - d, x, y + r);
    mPath.AddArc(x, y, d, d, 180, 90);
    mPath.CloseFigure();
}

void GDIPlusSVGPath::RoundedRect(float x, float y, float w, float h, float rx, float ry)
{
    const float dx = rx + rx;
    const float dy = ry + ry;

    mPath.AddLine(x + rx, y, x + w - dx, y);
    mPath.AddArc(x + w - dx, y, dx, dy, 270, 90);
    mPath.AddLine(x + w, y + ry, x + w, y + h - dy);
    mPath.AddArc(x + w - dx, y + h - dy, dx, dy, 0, 90);
    mPath.AddLine(x + w - dx, y + h, x + rx, y + h);
    mPath.AddArc(x, y + h - dy, dx, dy, 90, 90);
    mPath.AddLine(x, y + h - dy, x, y + ry);
    mPath.AddArc(x, y, dx, dy, 180, 90);
    mPath.CloseFigure();
}

void GDIPlusSVGPath::Ellipse(float cx, float cy, float rx, float ry)
{
    mPath.AddEllipse(cx - rx, cy - ry, rx + rx, ry + ry);
}

void GDIPlusSVGPath::MoveTo(float x, float y)
{
    mCurrentX = x;
    mCurrentY = y;
}

void GDIPlusSVGPath::LineTo(float x, float y)
{
    mPath.AddLine(mCurrentX, mCurrentY, x, y);
    mCurrentX = x;
    mCurrentY = y;
}

void GDIPlusSVGPath::CurveTo(float x1, float y1, float x2, float y2, float x3, float y3)
{
    mPath.AddBezier(mCurrentX, mCurrentY, x1, y1, x2, y2, x3, y3);
    mCurrentX = x3;
    mCurrentY = y3;
}

void GDIPlusSVGPath::CurveToV(float x2, float y2, float x3, float y3)
{
    float cx1 = static_cast<float>(mCurrentX + 2.0 / 3.0 * (x2 - mCurrentX));
    float cy1 = static_cast<float>(mCurrentY + 2.0 / 3.0 * (y2 - mCurrentY));
    float cx2 = static_cast<float>(x3 + 2.0 / 3.0 * (x2 - x3));
    float cy2 = static_cast<float>(y3 + 2.0 / 3.0 * (y2 - y3));

    mPath.AddBezier(mCurrentX, mCurrentY, cx1, cy1, cx2, cy2, x3, y3);
    mCurrentX = x3;
    mCurrentY = y3;
}

void GDIPlusSVGPath::ClosePath()
{
    mPath.CloseFigure();
}

const Gdiplus::GraphicsPath& GDIPlusSVGPath::GetGraphicsPath() const
{
    return mPath;
}

/******************************************************************************/

GDIPlusSVGTransform::GDIPlusSVGTransform(float a, float b, float c, float d, float tx, float ty)
{
    mTransform.SetElements(a, b, c, d, tx, ty);
}

void GDIPlusSVGTransform::Set(float a, float b, float c, float d, float tx, float ty)
{
    mTransform.SetElements(a, b, c, d, tx, ty);
}

void GDIPlusSVGTransform::Rotate(float r)
{
    mTransform.Rotate(r);
}

void GDIPlusSVGTransform::Translate(float tx, float ty)
{
    mTransform.Translate(tx, ty);
}

void GDIPlusSVGTransform::Scale(float sx, float sy)
{
    mTransform.Scale(sx, sy);
}

void GDIPlusSVGTransform::Concat(const Transform& other)
{
    mTransform.Multiply(&dynamic_cast<const GDIPlusSVGTransform&>(other).mTransform);
}

const Gdiplus::Matrix& GDIPlusSVGTransform::GetMatrix() const
{
    return mTransform;
}

GDIPlusSVGImageData::GDIPlusSVGImageData(const std::string& base64, ImageEncoding encoding)
{
    std::string imageString = base64_decode(base64);
    HGLOBAL hImageData = ::GlobalAlloc(GMEM_MOVEABLE, imageString.size());
    if (hImageData)
    {
        void* imageData = ::GlobalLock(hImageData);
        if (imageData)
        {
            std::memcpy(imageData, imageString.c_str(), imageString.size());

            IStream* pImageStream{};
            if (::CreateStreamOnHGlobal(hImageData, FALSE, &pImageStream) == S_OK)
            {
                mImage = std::unique_ptr<Gdiplus::Image>(Gdiplus::Image::FromStream(pImageStream, false));
                pImageStream->Release();
            }
            ::GlobalUnlock(hImageData);
        }
        ::GlobalFree(hImageData);
    }
}

GDIPlusSVGImageData::~GDIPlusSVGImageData()
{
}

float GDIPlusSVGImageData::Width() const
{
    return static_cast<float>(mImage->GetWidth());
}

float GDIPlusSVGImageData::Height() const
{
    return static_cast<float>(mImage->GetHeight());
}

const std::unique_ptr<Gdiplus::Image>& GDIPlusSVGImageData::GetImage() const
{
    return mImage;
}

GDIPlusSVGRenderer::GDIPlusSVGRenderer()
{
}

void GDIPlusSVGRenderer::Save(const GraphicStyle& graphicStyle)
{
    mStateStack.push_back(mContext->Save());

    if (graphicStyle.transform)
    {
        const Gdiplus::Matrix* matrix = &dynamic_cast<GDIPlusSVGTransform*>(graphicStyle.transform.get())->GetMatrix();
        if (matrix != nullptr)
        {
            mContext->SetTransform(matrix);
        }
    }

    if (graphicStyle.clippingPath)
    {
        // TODO
        SVG_ASSERT_MSG(false, "Need to implement svg clipping path!");
    }

    if (graphicStyle.opacity < 1.0f)
    {
        // TODO
        SVG_ASSERT_MSG(false, "Need to implement svg opacity!");
    }
}

void GDIPlusSVGRenderer::Restore()
{
    mContext->Restore(mStateStack.back());
    mStateStack.pop_back();
}

std::unique_ptr<Gdiplus::Brush> GDIPlusSVGRenderer::CreateGradientBrush(const Gradient& gradient, float opacity)
{
    std::unique_ptr<Gdiplus::Brush> resultBrush;

    const size_t stopCount = gradient.colorStops.size();

    std::vector<Gdiplus::Color> presetColors;
    std::vector<float> blendPositions;
    for (const auto& colorStop : gradient.colorStops)
    {
        Color color = colorStop.second;
        color[3] *= opacity;

        if (gradient.type == GradientType::kLinearGradient)
        {
            presetColors.push_back(ColorToGdiplusColor(color));
        }
        else
        {
           presetColors.insert(presetColors.begin(), ColorToGdiplusColor(color));
        }

        blendPositions.push_back(colorStop.first);
    }

    const Gdiplus::Matrix* matrix{};
    GDIPlusSVGTransform* gdiplusSvgTransform = dynamic_cast<GDIPlusSVGTransform*>(gradient.transform.get());
    if (gdiplusSvgTransform)
    {
        matrix = &gdiplusSvgTransform->GetMatrix();
    }

    Gdiplus::WrapMode wrapMode = Gdiplus::WrapModeClamp;
    switch (gradient.method)
    {
        case SpreadMethod::kPad:
            wrapMode = Gdiplus::WrapModeClamp;
            break;
        case SpreadMethod::kReflect:
            wrapMode = Gdiplus::WrapModeTileFlipXY;
            break;
        case SpreadMethod::kRepeat:
            wrapMode = Gdiplus::WrapModeTile;
            break;
    }

    if (gradient.type == GradientType::kLinearGradient)
    {
        Gdiplus::LinearGradientBrush* linearGradientBrush = new Gdiplus::LinearGradientBrush(Gdiplus::PointF(gradient.x1, gradient.y1), Gdiplus::PointF(gradient.x2, gradient.y2),
            ColorToGdiplusColor(gradient.colorStops[0].second), ColorToGdiplusColor(gradient.colorStops[stopCount - 1].second));

        linearGradientBrush->SetInterpolationColors(presetColors.data(), blendPositions.data(), static_cast<int>(stopCount));

        if (matrix != nullptr)
        {
            linearGradientBrush->SetTransform(matrix);
        }

        linearGradientBrush->SetWrapMode(wrapMode);

        resultBrush = std::unique_ptr<Gdiplus::Brush>(linearGradientBrush);
    }
    else
    {
        SVG_ASSERT_MSG(gradient.type == GradientType::kRadialGradient, "SVG radial gradient expected.");
        Gdiplus::GraphicsPath ellipticalPath;
        ellipticalPath.AddEllipse(gradient.cx- gradient.r, gradient.cy- gradient.r, 2*gradient.r, 2*gradient.r);

        Gdiplus::PathGradientBrush* radialGradientBrush = new Gdiplus::PathGradientBrush(&ellipticalPath);
        radialGradientBrush->SetInterpolationColors(presetColors.data(), blendPositions.data(), static_cast<int>(stopCount));
        radialGradientBrush->SetCenterPoint(Gdiplus::PointF(gradient.fx, gradient.fy));

        if (matrix != nullptr)
        {
            radialGradientBrush->SetTransform(matrix);
        }

        radialGradientBrush->SetWrapMode(wrapMode);

        resultBrush = std::unique_ptr<Gdiplus::Brush>(radialGradientBrush);
    }

    return resultBrush;
}

void GDIPlusSVGRenderer::DrawPath(const Path& renderPath, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle)
{
    SVG_ASSERT(mContext);
    Gdiplus::GraphicsState savedState = mContext->Save();

    std::unique_ptr<Gdiplus::GraphicsPath> path(dynamic_cast<const GDIPlusSVGPath&>(renderPath).GetGraphicsPath().Clone());
    if (fillStyle.fillRule == WindingRule::kEvenOdd)
        path->SetFillMode(Gdiplus::FillModeAlternate);
    else
        path->SetFillMode(Gdiplus::FillModeWinding);

    mContext->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

    std::unique_ptr<Gdiplus::Brush> brush;
    if (fillStyle.hasFill)
    {
        if (fillStyle.paint.type() == typeid(Color))
        {
            auto color = boost::get<Color>(fillStyle.paint);
            color[3] *= fillStyle.fillOpacity;

            Gdiplus::Color brushColor = ColorToGdiplusColor(color);
            brush = std::unique_ptr<Gdiplus::Brush>(new Gdiplus::SolidBrush(brushColor));
        }
        else if (fillStyle.paint.type() == typeid(Gradient))
        {
            const auto& gradient = boost::get<Gradient>(fillStyle.paint);
            brush = CreateGradientBrush(gradient, fillStyle.fillOpacity);
        }

        mContext->FillPath(brush.get(), path.get());
    }

    if (strokeStyle.hasStroke)
    {
        std::unique_ptr<Gdiplus::Pen> pen;
        if (strokeStyle.paint.type() == typeid(Color))
        {
            auto color = boost::get<Color>(strokeStyle.paint);
            color[3] *= strokeStyle.strokeOpacity;

            Gdiplus::Color penColor = ColorToGdiplusColor(color);
            pen = std::unique_ptr<Gdiplus::Pen>(new Gdiplus::Pen(penColor, strokeStyle.lineWidth));
        }
        else if (strokeStyle.paint.type() == typeid(Gradient))
        {
            const auto& gradient = boost::get<Gradient>(fillStyle.paint);
            brush = CreateGradientBrush(gradient, fillStyle.fillOpacity);
            pen = std::unique_ptr<Gdiplus::Pen>(new Gdiplus::Pen(brush.get(), strokeStyle.lineWidth));
        }

        switch (strokeStyle.lineCap)
        {
            case LineCap::kButt:
                pen->SetLineCap(Gdiplus::LineCapFlat, Gdiplus::LineCapFlat, Gdiplus::DashCapFlat);
                break;
            case LineCap::kRound:
                pen->SetLineCap(Gdiplus::LineCapRound, Gdiplus::LineCapRound, Gdiplus::DashCapRound);
                break;
            case LineCap::kSquare:
                pen->SetLineCap(Gdiplus::LineCapSquare, Gdiplus::LineCapSquare, Gdiplus::DashCapFlat);
                break;
        }

        switch (strokeStyle.lineJoin)
        {
            case LineJoin::kMiter:
                pen->SetLineJoin(Gdiplus::LineJoinMiter);
                break;
            case LineJoin::kRound:
                pen->SetLineJoin(Gdiplus::LineJoinRound);
                break;
            case LineJoin::kBevel:
                pen->SetLineJoin(Gdiplus::LineJoinBevel);
                break;
        }

        if (!strokeStyle.dashArray.empty())
        {
            pen->SetDashStyle(Gdiplus::DashStyleDash);
            pen->SetDashOffset(strokeStyle.dashOffset);

            std::vector<float> dashArray = strokeStyle.dashArray;
            if (dashArray.size() % 2 == 1)
            {
                // Gdiplus doesn't do SVG's odd case which duplicates array to make it even.
                dashArray.insert(dashArray.end(), strokeStyle.dashArray.begin(), strokeStyle.dashArray.end());
            }

            pen->SetDashPattern(dashArray.data(), static_cast<int>(dashArray.size()));
        }

        pen->SetMiterLimit(strokeStyle.miterLimit);

        mContext->DrawPath(pen.get(), path.get());
    }

    mContext->Restore(savedState);
}

void GDIPlusSVGRenderer::DrawImage(const ImageData& image, const GraphicStyle& graphicStyle, const Rect& clipArea, const Rect& fillArea)
{
    const GDIPlusSVGImageData& gdiPlusImage(dynamic_cast<const GDIPlusSVGImageData&>(image));
    if (gdiPlusImage.GetImage())
    {
        Save(graphicStyle);

        if (clipArea.width < fillArea.width || clipArea.height < fillArea.height)
        {
            mContext->SetClip(Gdiplus::RectF(clipArea.x, clipArea.y, clipArea.width, clipArea.height));
        }

        mContext->DrawImage(gdiPlusImage.GetImage().get(), Gdiplus::RectF(fillArea.x, 0, fillArea.width, fillArea.height));

        Restore();
    }
}

} // namespace SVGNative
