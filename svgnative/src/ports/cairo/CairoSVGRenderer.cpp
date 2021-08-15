/*
Copyright 2019 suzuki toshiya <mpsuzuki@hiroshima-u.ac.jp>. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#include "svgnative/ports/cairo/CairoSVGRenderer.h"
#include "base64.h"
#include "svgnative/Config.h"
#include "cairo.h"
#include <math.h>
#include "CairoImageInfo.h"

namespace SVGNative
{
CairoSVGPath::CairoSVGPath()
{
#if CAIRO_HAS_IMAGE_SURFACE
    cairo_surface_t* sf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
#elif CAIRO_HAS_RECORDING_SURFACE
    cairo_surface_t* sf = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, NULL);
#endif
    mPathCtx = cairo_create(sf);
}

CairoSVGPath::~CairoSVGPath()
{
    cairo_surface_t* sf = cairo_get_target(mPathCtx);

    cairo_destroy(mPathCtx);
    cairo_surface_finish(sf);
    cairo_surface_destroy(sf);
}

void CairoSVGPath::Rect(float x, float y, float width, float height)
{
    cairo_rectangle(mPathCtx, x, y, width, height);
    cairo_close_path(mPathCtx);
}

inline double deg2rad(double deg)
{
    return (deg * M_PI / 180.0);
}

inline void cairo_arc_rx_ry(cairo_t* cr, float cx, float cy, float rx, float ry, float rad1, float rad2)
{
    // cairo_scale() with too small rx (or ry) breaks Cairo context.
    // they are tested and refused by the caller, SVGDocumentImpl::ParseShape().

    if (rx == ry)
    {
        // cairo_scale(), cairo_restore() are heavy calculation for some CPUs.
        // for the cases we can draw without them, draw like this.
        cairo_arc(cr, cx, cy, rx, rad1, rad2);
    }
    else
    {
        cairo_save(cr);
        cairo_translate(cr, cx, cy);
        cairo_scale(cr, rx, ry);
        cairo_arc(cr, 0, 0, 1, rad1, rad2);
        cairo_restore(cr);
    }
}

void CairoSVGPath::RoundedRect(float x, float y, float width, float height, float rx, float ry)
{
    // Cairo does not provide single API to draw "rounded rect". See
    // https://www.cairographics.org/samples/rounded_rectangle/

    cairo_new_sub_path(mPathCtx);
    cairo_arc_rx_ry(mPathCtx, x - rx + width, y + ry,          rx, ry, deg2rad(-90), deg2rad(  0));
    cairo_arc_rx_ry(mPathCtx, x - rx + width, y - ry + height, rx, ry, deg2rad(  0), deg2rad( 90));
    cairo_arc_rx_ry(mPathCtx, x + rx,         y - ry + height, rx, ry, deg2rad( 90), deg2rad(180));
    cairo_arc_rx_ry(mPathCtx, x + rx,         y + ry,          rx, ry, deg2rad(180), deg2rad(270));
    cairo_close_path(mPathCtx);
}

void CairoSVGPath::Ellipse(float cx, float cy, float rx, float ry)
{
    // Cairo does not provide single API to draw "ellipse". See
    // https://cairographics.org/cookbook/ellipses/

    cairo_matrix_t  save_matrix;
    cairo_get_matrix(mPathCtx, &save_matrix);

    cairo_translate(mPathCtx, cx, cy);
    cairo_scale(mPathCtx, rx, ry);
    cairo_new_sub_path(mPathCtx);
    cairo_arc(mPathCtx, 0, 0, 1, 0, 2 * M_PI);

    cairo_set_matrix(mPathCtx, &save_matrix);
}

void CairoSVGPath::MoveTo(float x, float y)
{
    cairo_move_to(mPathCtx, x, y);
    mCurrentX = x;
    mCurrentY = y;
}

void CairoSVGPath::LineTo(float x, float y)
{
    cairo_line_to(mPathCtx, x, y);
    mCurrentX = x;
    mCurrentY = y;
}

void CairoSVGPath::CurveTo(float x1, float y1, float x2, float y2, float x3, float y3)
{
    cairo_curve_to(mPathCtx, x1, y1, x2, y2, x3, y3);
    mCurrentX = x3;
    mCurrentY = y3;
}

void CairoSVGPath::CurveToV(float x2, float y2, float x3, float y3)
{
    float cx1 = mCurrentX + 2.0f / 3.0f * (x2 - mCurrentX);
    float cy1 = mCurrentY + 2.0f / 3.0f * (y2 - mCurrentY);
    float cx2 = x3 + 2.0f / 3.0f * (x2 - x3);
    float cy2 = y3 + 2.0f / 3.0f * (y2 - y3);

    cairo_curve_to(mPathCtx, cx1, cy1, cx2, cy2, x3, y3);
    mCurrentX = x3;
    mCurrentY = y3;
}

void CairoSVGPath::ClosePath()
{
    cairo_close_path(mPathCtx);
}

CairoSVGTransform::CairoSVGTransform(float a, float b, float c, float d, float tx, float ty)
{
    cairo_matrix_init(&mMatrix, a, b, c, d, tx, ty);
}

void CairoSVGTransform::Set(float a, float b, float c, float d, float tx, float ty)
{
    cairo_matrix_init(&mMatrix, a, b, c, d, tx, ty);
}

void CairoSVGTransform::Rotate(float degree)
{
    cairo_matrix_rotate(&mMatrix, deg2rad(degree) );
}

void CairoSVGTransform::Translate(float tx, float ty)
{
    cairo_matrix_translate(&mMatrix, tx, ty);
}

void CairoSVGTransform::Scale(float sx, float sy)
{
    cairo_matrix_scale(&mMatrix, sx, sy);
}

void CairoSVGTransform::Concat(float a, float b, float c, float d, float tx, float ty)
{
    cairo_matrix_t other{a, b, c, d, tx, ty};
    cairo_matrix_multiply(&mMatrix, &other, &mMatrix);
}

CairoSVGImageData::CairoSVGImageData(const std::string& base64, ImageEncoding encoding)
{
    std::string imageString = base64_decode(base64);
    const unsigned char* blob_data = (const unsigned char*)malloc(imageString.size());
    if (!blob_data)
        throw("no memory\n");
    memcpy((void *)blob_data, imageString.data(), imageString.size());

    if (encoding == ImageEncoding::kJPEG)
    {
        mImageData = _cairo_image_surface_create_from_jpeg_stream(blob_data, imageString.size());
        if (mImageData)
        {
            cairo_surface_set_mime_data(mImageData, "image/jpeg", blob_data, imageString.size(), free, (void*)blob_data);
            return;
        }
    }
    else if (encoding == ImageEncoding::kPNG)
    {
        /* this closure is used during the construction of the surface, but no need in later */
        _png_blob_closure_t png_closure{ blob_data, 0, imageString.size() };
        mImageData = cairo_image_surface_create_from_png_stream(_png_blob_read_func, &png_closure);
        if (mImageData)
        {
            cairo_surface_set_mime_data(mImageData, "image/png", blob_data, imageString.size(), free, (void*)blob_data);
            return;
        }
    }
    free((void *)blob_data);
    throw("image is broken, or not PNG or JPEG\n");
}

CairoSVGImageData::~CairoSVGImageData()
{
    cairo_surface_destroy(mImageData);
}

float CairoSVGImageData::Width() const
{
    if (!mImageData)
        return 0;
    return static_cast<float>(cairo_image_surface_get_width(mImageData));
}

float CairoSVGImageData::Height() const
{
    if (!mImageData)
        return 0;
    return static_cast<float>(cairo_image_surface_get_height(mImageData));
}

CairoSVGRenderer::CairoSVGRenderer()
{
}

CairoSVGRenderer::~CairoSVGRenderer()
{
}

inline cairo_path_t* getPathObjFromCairoSvgPath( const Path* path )
{
    cairo_t* cr = static_cast<const CairoSVGPath*>(path)->mPathCtx;
    return cairo_copy_path(cr);
}

inline cairo_path_t* getTransformedClippingPath( const ClippingPath* clippingPath )
{
    cairo_path_t* path = getPathObjFromCairoSvgPath(clippingPath->path.get());
    if (!clippingPath->transform)
        return path;

    cairo_matrix_t matrix = static_cast<const CairoSVGTransform*>(clippingPath->transform.get())->mMatrix;
    cairo_path_t* pathTransformed = (cairo_path_t*)malloc(sizeof(cairo_path_t));
    pathTransformed->num_data = path->num_data;
    pathTransformed->status = path->status;
    pathTransformed->data = (cairo_path_data_t*)malloc(path->num_data * sizeof(cairo_path_data_t));
    for (int i = 0; i < path->num_data; i += path->data[i].header.length)
    {
        pathTransformed->data[i].header = path->data[i].header;
        for (int j = 1; j < path->data[i].header.length; j ++)
        {
            double x = path->data[i+j].point.x;
            double y = path->data[i+j].point.y;
            cairo_matrix_transform_point(&matrix, &x, &y);
            pathTransformed->data[i+j].point.x = x;
            pathTransformed->data[i+j].point.y = y;
        }
    }
    cairo_path_destroy(path);
    return pathTransformed;
}

inline void setCairoFillAndClipRule(cairo_t* cr, WindingRule rule)
{
    // Cairo reuses the fill-rule to control clipping behaviour.

    switch (rule)
    {
    case WindingRule::kEvenOdd:
        cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
        return;
    case WindingRule::kNonZero:
    default:
        cairo_set_fill_rule(cr, CAIRO_FILL_RULE_WINDING);
        return;
    }
}

void CairoSVGRenderer::Save(const GraphicStyle& graphicStyle)
{
    SVG_ASSERT(mCairo);
    cairo_save(mCairo);

    if (graphicStyle.transform)
        cairo_transform(mCairo, &(static_cast<CairoSVGTransform*>(graphicStyle.transform.get())->mMatrix));

    if (graphicStyle.clippingPath && graphicStyle.clippingPath->path)
    {
        cairo_path_t* path = getTransformedClippingPath( graphicStyle.clippingPath.get() );
        cairo_append_path(mCairo, path);
        setCairoFillAndClipRule(mCairo, graphicStyle.clippingPath->clipRule);
        cairo_clip(mCairo);
        cairo_path_destroy(path);
    }
}

void CairoSVGRenderer::Restore()
{
    SVG_ASSERT(mCairo);
    cairo_restore(mCairo);
}

inline void createCairoPattern(const Paint& paint, float opacity, cairo_pattern_t** pat)
{
    *pat = NULL;

    if (paint.type() != typeid(Gradient))
        return;

    const auto& gradient = boost::get<Gradient>(paint);

    SVG_ASSERT(gradient.type <= GradientType::kRadialGradient);

    // in Cairo, gradient type might be set before setting color stops. See
    // https://www.cairographics.org/samples/gradient/
    switch (gradient.type)
    {
    case GradientType::kLinearGradient:
        *pat = cairo_pattern_create_linear(gradient.x1, gradient.y1,
                                           gradient.x2, gradient.y2);
        break;

    case GradientType::kRadialGradient:
        *pat = cairo_pattern_create_radial(gradient.fx, gradient.fy, 0,
                                           gradient.cx, gradient.cy, gradient.r);
        break;
    default:
        throw("unsupported gradient type\n");
    }

    // set transform matrix
    if (gradient.transform)
        cairo_pattern_set_matrix(*pat, &(static_cast<CairoSVGTransform*>(gradient.transform.get())->mMatrix));

    // set "stop"s of gradient
    for (const auto& stop : gradient.colorStops)
    {
        // here, ColorStop is a pair of offset (in float) and color
        const auto& stopOffset = stop.first;
        const auto& stopColor = stop.second;

        cairo_pattern_add_color_stop_rgba(*pat, stopOffset,
                                          stopColor[0],
                                          stopColor[1],
                                          stopColor[2],
                                          opacity * stopColor[3]);
    }

    // set the mode how to fill the wide area by a small pattern
    switch (gradient.method)
    {
    case SpreadMethod::kReflect:
        cairo_pattern_set_extend(*pat, CAIRO_EXTEND_REFLECT);
        break;
    case SpreadMethod::kRepeat:
        cairo_pattern_set_extend(*pat, CAIRO_EXTEND_REPEAT);
        break;
    case SpreadMethod::kPad:
        cairo_pattern_set_extend(*pat, CAIRO_EXTEND_PAD);
        break;
    default:
        cairo_pattern_set_extend(*pat, CAIRO_EXTEND_NONE);
        break;
    }
    return;
}

inline void appendCairoSvgPath(cairo_t* mCairo, const Path& path)
{
    cairo_path_t* cairoPath = getPathObjFromCairoSvgPath(&path);
    cairo_append_path(mCairo, cairoPath);
    cairo_path_destroy(cairoPath);
}

void CairoSVGRenderer::DrawPath(
    const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle)
{
    SVG_ASSERT(mCairo);
    Save(graphicStyle);

    if (fillStyle.hasFill)
    {
        if (fillStyle.paint.type() == typeid(Gradient))
        {
            cairo_pattern_t* pat;
            createCairoPattern(fillStyle.paint, fillStyle.fillOpacity * graphicStyle.opacity, &pat);
            cairo_set_source(mCairo, pat);
        }
        else
        {
            const auto& color = boost::get<Color>(fillStyle.paint);
            cairo_set_source_rgba(mCairo,
                                  color[0],
                                  color[1],
                                  color[2],
                                  color[3] * fillStyle.fillOpacity * graphicStyle.opacity);
        }

        cairo_new_path(mCairo);
        appendCairoSvgPath(mCairo, path);
        setCairoFillAndClipRule(mCairo, fillStyle.fillRule);
        cairo_fill(mCairo);
    }
    if (strokeStyle.hasStroke)
    {
        const auto& color = boost::get<Color>(strokeStyle.paint);
        cairo_set_source_rgba(mCairo,
                              color[0],
                              color[1],
                              color[2],
                              color[3] * strokeStyle.strokeOpacity * graphicStyle.opacity);

        cairo_set_line_width(mCairo, strokeStyle.lineWidth);
        cairo_set_miter_limit(mCairo, strokeStyle.miterLimit);
        switch (strokeStyle.lineCap)
        {
        case LineCap::kRound:
            cairo_set_line_cap(mCairo, CAIRO_LINE_CAP_ROUND);
            break;
        case LineCap::kSquare:
            cairo_set_line_cap(mCairo, CAIRO_LINE_CAP_SQUARE);
            break;
        case LineCap::kButt:
        default:
            cairo_set_line_cap(mCairo, CAIRO_LINE_CAP_BUTT);
        }

        switch (strokeStyle.lineJoin)
        {
        case LineJoin::kRound:
            cairo_set_line_join(mCairo, CAIRO_LINE_JOIN_ROUND);
            break;
        case LineJoin::kBevel:
            cairo_set_line_join(mCairo, CAIRO_LINE_JOIN_BEVEL);
            break;
        case LineJoin::kMiter:
        default:
            cairo_set_line_join(mCairo, CAIRO_LINE_JOIN_MITER);
        }

        if (!strokeStyle.dashArray.empty())
        {
            std::vector<double> dashes;
            for (auto dash : strokeStyle.dashArray)
                 dashes.push_back(dash);
            cairo_set_dash(mCairo, dashes.data(), strokeStyle.dashArray.size(), strokeStyle.dashOffset);
        }

        cairo_new_path(mCairo);
        appendCairoSvgPath(mCairo, path);
        cairo_stroke(mCairo);
    }
    Restore();
}

void CairoSVGRenderer::DrawImage(
    const ImageData& image, const GraphicStyle& graphicStyle, const Rect& clipArea, const Rect& fillArea)
{
    SVG_ASSERT(mCairo);
    Save(graphicStyle);
    cairo_new_path(mCairo);
    cairo_rectangle(mCairo, clipArea.x, clipArea.y, clipArea.width, clipArea.height);
    cairo_clip(mCairo);

    cairo_surface_t *mImageData = (static_cast<const CairoSVGImageData&>(image)).mImageData;

    cairo_translate(mCairo, fillArea.x, fillArea.y);
    cairo_scale(mCairo, fillArea.width / image.Width(), fillArea.height / image.Height());
    cairo_set_source_surface(mCairo, mImageData, 0, 0);
    cairo_paint_with_alpha(mCairo, graphicStyle.opacity);

    Restore();
}

void CairoSVGRenderer::SetCairo(cairo_t* cr)
{
    SVG_ASSERT(cr);
    mCairo = cr;
}

} // namespace SVGNative
