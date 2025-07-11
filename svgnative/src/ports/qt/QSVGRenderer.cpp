/*
Copyright 2025 Adobe. All rights reserved.
Copyright 2025 KATO Kanryu All rights reserved.
Developed and contributed by KATO Kanryu.

This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/
#include <algorithm>
#include <QtGui>
#include "svgnative/Config.h"
#include "svgnative/ports/qt/QSVGRenderer.h"

namespace SVGNative
{

/******************************************************************************/

QColor ColorToQtColor(const Color& inColor)
{
    return QColor(
        static_cast<int>(inColor[0] * 255.0),
        static_cast<int>(inColor[1] * 255.0),
        static_cast<int>(inColor[2] * 255.0),
        static_cast<int>(inColor[3] * 255.0));
}

/******************************************************************************/

QSVGPath::QSVGPath()
{
}

QSVGPath::~QSVGPath()
{
}

void QSVGPath::Rect(float x, float y, float width, float height)
{
    mPath.addRect(QRectF(x, y, width, height));
}

void QSVGPath::RoundedRect(float x, float y, float w, float h, float rx, float ry)
{
    mPath.addRoundedRect(QRectF(x, y, w, h), rx, ry, Qt::AbsoluteSize);
}

void QSVGPath::Ellipse(float cx, float cy, float rx, float ry)
{
    mPath.addEllipse(cx-rx, cy - ry, rx + rx, ry + ry);
}

void QSVGPath::MoveTo(float x, float y)
{
    mPath.moveTo(x, y);
    mCurrentX = x;
    mCurrentY = y;
}

void QSVGPath::LineTo(float x, float y)
{
    mPath.lineTo(x, y);
    mCurrentX = x;
    mCurrentY = y;
}

void QSVGPath::CurveTo(float x1, float y1, float x2, float y2, float x3, float y3)
{
    mPath.cubicTo(x1, y1, x2, y2, x3, y3);
    mCurrentX = x3;
    mCurrentY = y3;
}

void QSVGPath::CurveToV(float x2, float y2, float x3, float y3)
{
    mPath.quadTo(x2, y2, x3, y3);
    mCurrentX = x3;
    mCurrentY = y3;
}

void QSVGPath::ClosePath()
{
    mPath.closeSubpath();
}

/******************************************************************************/

QSVGTransform::QSVGTransform(float a, float b, float c, float d, float tx, float ty)
{
    mTransform = QTransform(a, b, c, d, tx, ty);
}

void QSVGTransform::Set(float a, float b, float c, float d, float tx, float ty)
{
    mTransform = QTransform(a, b, c, d, tx, ty);
}

void QSVGTransform::Rotate(float r)
{
    mTransform.rotate(r);
}

void QSVGTransform::Translate(float tx, float ty)
{
    mTransform.translate(tx, ty);
}

void QSVGTransform::Scale(float sx, float sy)
{
    mTransform.scale(sx, sy);
}

void QSVGTransform::Concat(float a, float b, float c, float d, float tx, float ty)
{
    QTransform other(a, b, c, d, tx, ty);
    mTransform *= other;
}

QSVGImageData::QSVGImageData(const std::string& base64, ImageEncoding encoding)
{
    QByteArray bytes = QByteArray::fromBase64(QByteArray::fromStdString(base64));
    mImage.loadFromData(bytes);
}

QSVGImageData::~QSVGImageData()
{
}

float QSVGImageData::Width() const
{
    return static_cast<float>(mImage.width());
}

float QSVGImageData::Height() const
{
    return static_cast<float>(mImage.height());
}

QSVGRenderer::QSVGRenderer()
{
}

std::unique_ptr<ImageData> QSVGRenderer::CreateImageData(const std::string& base64, ImageEncoding encoding)
{
    return std::unique_ptr<QSVGImageData>(new QSVGImageData(base64, encoding));
}

std::unique_ptr<Path> QSVGRenderer::CreatePath()
{
    return std::unique_ptr<QSVGPath>(new QSVGPath);
}

std::unique_ptr<Transform> QSVGRenderer::CreateTransform(float a, float b, float c, float d, float tx, float ty)
{
    return std::unique_ptr<QSVGTransform>(new QSVGTransform(a, b, c, d, tx, ty));
}

void QSVGRenderer::Save(const GraphicStyle& graphicStyle)
{
    SVG_ASSERT(mPainter);
    mPainter->save();
    if (graphicStyle.transform)
    {
        mPainter->setTransform(static_cast<QSVGTransform*>(graphicStyle.transform.get())->mTransform, true); // combine

    }
    if (graphicStyle.clippingPath && graphicStyle.clippingPath->path)
    {
        QPainterPath clippingPath(static_cast<const QSVGPath*>(graphicStyle.clippingPath->path.get())->mPath);
        if (graphicStyle.clippingPath->transform)
        {
            const QTransform matrix = static_cast<const QSVGTransform*>(graphicStyle.clippingPath->transform.get())->mTransform;
            clippingPath = matrix.map(clippingPath);
        }
        clippingPath.setFillRule(graphicStyle.clippingPath->clipRule == WindingRule::kNonZero ? Qt::WindingFill : Qt::OddEvenFill);
        mPainter->setClipPath(clippingPath);
    }
}

void QSVGRenderer::Restore()
{
    SVG_ASSERT(mPainter);
    mPainter->restore();
}

inline void CreateGradient(const Gradient& gradient, float opacity, QGradient* grad)
{
    QList<QGradientStop> stops;
    for (const auto& stop : gradient.colorStops)
    {
        QGradientStop stp;
        stp.first = (qreal)stop.first;
        const Color& stopColor = stop.second;
        stp.second = ColorToQtColor(stopColor);
        stops.append(stp);
    }
    if (!stops.empty())
    {
        grad->setStops(stops);
    }
    switch (gradient.method)
    {
    case SpreadMethod::kReflect:
        grad->setSpread(QGradient::ReflectSpread);
        break;
    case SpreadMethod::kRepeat:
        grad->setSpread(QGradient::RepeatSpread);
        break;
    case SpreadMethod::kPad:
    default:
        grad->setSpread(QGradient::PadSpread);
        break;
    }
}

inline void CreateBrush(const Paint& paint, float opacity, QPainter* mPainter)
{
    QBrush fill(Qt::SolidPattern);
    if (SVGNative::holds_alternative<Gradient>(paint))
    {
        const auto& gradient = SVGNative::get<Gradient>(paint);
        QTransform transform;
        if (gradient.transform)
        {
            transform = static_cast<QSVGTransform*>(gradient.transform.get())->mTransform;
        }
        if (gradient.type == GradientType::kLinearGradient)
        {
            QPointF p1(gradient.x1, gradient.y1), p2(gradient.x2, gradient.y2);

            // QGradient does not have a transform, so you need to transform the initial value.
            if (!transform.isIdentity())
            {
                p1 = transform.map(p1);
                p2 = transform.map(p2);
            }
            QLinearGradient grad(p1, p2);
            CreateGradient(gradient, opacity, &grad);
            fill = QBrush(grad);
        }
        else if (gradient.type == GradientType::kRadialGradient)
        {
            QPointF pc(gradient.cx, gradient.cy), pf(gradient.fx, gradient.fy), pr(gradient.cx + gradient.r, gradient.cy);

            // For some reason, the radius of Qt and the radius of SVG differ by exactly sqrt(2)/2.
            qreal radius = gradient.r*0.7071;

            // QGradient does not have a transform, so you need to transform the initial value.
            if (!transform.isIdentity())
            {
                pc = transform.map(pc);
                pf = transform.map(pf);
                pr = transform.map(pr);
                radius = std::sqrt((pc.x()-pr.x())*(pc.x()-pr.x()) + (pc.y()-pr.y())*(pc.y()-pr.y()));
            }
            QRadialGradient grad(pc, radius, pf);
            CreateGradient(gradient, opacity, &grad);

            // In Qt, QGradient is an attribute of QBrush.
            fill = QBrush(grad);
        }
    }
    if (SVGNative::holds_alternative<Color>(paint))
    {
        const auto& color = SVGNative::get<Color>(paint);
        fill.setColor(ColorToQtColor(color));
    }
    mPainter->setBrush(fill);
}

inline void CreatePen(const Paint& paint, float opacity, QPainter* mPainter)
{
    QPen pen;
    if (SVGNative::holds_alternative<Color>(paint))
    {
        const auto& color = SVGNative::get<Color>(paint);
        pen.setColor(ColorToQtColor(color));
    }
    mPainter->setPen(pen);
}

void QSVGRenderer::DrawPath(const Path& renderPath, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle)
{
    SVG_ASSERT(mPainter);
    Save(graphicStyle);

    // If you want anti-aliasing in the drawing results,
    // you should add this flag, but it is still not enough.
    mPainter->setRenderHints(QPainter::Antialiasing, true);

    // SVG does not have a default outline for paths
    mPainter->setPen(QPen(Qt::NoPen));

    QPainterPath mPath = (static_cast<const QSVGPath&>(renderPath).mPath);
    if (fillStyle.hasFill)
    {
        CreateBrush(fillStyle.paint, fillStyle.fillOpacity, mPainter);
        mPath.setFillRule(fillStyle.fillRule == WindingRule::kNonZero ? Qt::WindingFill : Qt::OddEvenFill );
        mPainter->drawPath(mPath);
    }
    if (strokeStyle.hasStroke)
    {
        QPainterPathStroker stroke;
        stroke.setWidth(strokeStyle.lineWidth);
        //stroke.setMiterLimit(strokeStyle.miterLimit);
        stroke.setMiterLimit(0.1);
        switch (strokeStyle.lineCap)
        {
        case LineCap::kRound:
            stroke.setCapStyle(Qt::RoundCap);
            break;
        case LineCap::kSquare:
            stroke.setCapStyle(Qt::SquareCap);
            break;
        case LineCap::kButt:
        default:
            stroke.setCapStyle(Qt::FlatCap);
        }
        switch (strokeStyle.lineJoin)
        {
        case LineJoin::kRound:
            stroke.setJoinStyle(Qt::RoundJoin);
            break;
        case LineJoin::kBevel:
            stroke.setJoinStyle(Qt::BevelJoin);
            break;
        case LineJoin::kMiter:
        default:
            stroke.setJoinStyle(Qt::MiterJoin);
        }
        if (!strokeStyle.dashArray.empty())
        {
            QList<qreal> dash;
            bool hasZero = false;
            for(float d : strokeStyle.dashArray) {
                if (d <= 0)
                {
                    hasZero = true;
                    continue;
                }
                if (!hasZero)
                {
                    // Qt's dashPattern needs to be removed because it is compensated for by Width
                    dash << d/strokeStyle.lineWidth;
                    continue;
                }
                // Although it is not stated explicitly in the SVG specification,
                // it appears that if dashPattern contains 0,
                // the values ​​before and after it are added together.
                dash.last() += d/strokeStyle.lineWidth;
                hasZero = false;
            }
            stroke.setDashPattern(dash);
            stroke.setDashOffset(strokeStyle.dashOffset);
        }
        CreatePen(strokeStyle.paint, strokeStyle.strokeOpacity, mPainter);
        QPainterPath strokedPath = stroke.createStroke(mPath);
        mPainter->drawPath(strokedPath);
    }

    Restore();
}

void QSVGRenderer::DrawImage(const ImageData& image, const GraphicStyle& graphicStyle, const Rect& clipArea, const Rect& fillArea)
{
    SVG_ASSERT(mPainter);
    const QSVGImageData& imd(dynamic_cast<const QSVGImageData&>(image));
    if (!imd.mImage.isNull())
    {
        Save(graphicStyle);

        if (clipArea.width < fillArea.width || clipArea.height < fillArea.height)
        {
            mPainter->setClipRect(QRectF(clipArea.x, clipArea.y, clipArea.width, clipArea.height));
        }

        mPainter->drawImage(QRectF(fillArea.x, 0, fillArea.width, fillArea.height), imd.mImage);

        Restore();
    }
}

Rect QSVGRenderer::GetBounds(const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle)
{
    SVG_ASSERT(mPainter);
    // we save the state while applying transforms and clippings if needed
    Save(graphicStyle);
    // get the internal SkPath
    QPainterPath mPath = (static_cast<const QSVGPath&>(path).mPath);
    mPath.setFillRule(fillStyle.fillRule == WindingRule::kNonZero ? Qt::WindingFill : Qt::OddEvenFill );
    // compute the tight fill bounds for the path
    QRectF bounds = mPath.controlPointRect();
    if (strokeStyle.hasStroke)
    {
        // create the stroke paint and then find the stroke bounds
        QPainterPathStroker stroke;
        stroke.setWidth(strokeStyle.lineWidth);
        if (!strokeStyle.dashArray.empty())
        {
            QList<qreal> dash;
            for(float d : strokeStyle.dashArray) {
                dash << d;
            }
            stroke.setDashPattern(dash);
        }
        //CreateSkPaint(strokeStyle.paint, strokeStyle.strokeOpacity, stroke);
        QPainterPath mPath = (static_cast<const QSVGPath&>(path).mPath);
        mPath = stroke.createStroke(mPath);
        bounds = mPath.controlPointRect();
    }

    // if there is clipping, take the clip bounds and intersect them with
    // the bound calculated so far, then return that intersection
    if (graphicStyle.clippingPath && graphicStyle.clippingPath->path)
    {
        Rect old_bounds{(float)bounds.x(), (float)bounds.y(), (float)bounds.width(), (float)bounds.height()};
        QRectF clip = mPainter->clipBoundingRect();
        Rect clip_bounds{(float)clip.x(), (float)clip.y(), (float)clip.width(), (float)clip.height()};
        Rect new_bounds = clip_bounds;
        bounds = QRectF(new_bounds.x, new_bounds.y, new_bounds.width, new_bounds.height);
    }
    Restore();
    Rect snv_bounds = Rect{(float)bounds.x(), (float)bounds.y(), (float)bounds.width(), (float)bounds.height()};
    if (!snv_bounds.IsEmpty())
        return snv_bounds;
    else
        return Rect{0, 0, 0, 0};
}

} // namespace SVGNative
