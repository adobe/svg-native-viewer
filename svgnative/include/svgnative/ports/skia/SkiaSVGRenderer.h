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

#ifndef SVGViewer_SkiaSVGRenderer_h
#define SVGViewer_SkiaSVGRenderer_h

#include "svgnative/SVGRenderer.h"
#include "svgnative/Rect.h"
#include "SkPath.h"

struct SkRect;
class SkCanvas;
class SkImage;

namespace SVGNative
{
class SkiaSVGPath final : public Path
{
public:
    SkiaSVGPath();

    void Rect(float x, float y, float width, float height) override;
    void RoundedRect(float x, float y, float width, float height, float rx, float ry) override;
    void Ellipse(float cx, float cy, float rx, float ry) override;

    void MoveTo(float x, float y) override;
    void LineTo(float x, float y) override;
    void CurveTo(float x1, float y1, float x2, float y2, float x3, float y3) override;
    void CurveToV(float x2, float y2, float x3, float y3) override;
    void ClosePath() override;

    SkPath mPath;

private:
    float mCurrentX{};
    float mCurrentY{};
};

class SkiaSVGTransform final : public Transform
{
public:
    SkiaSVGTransform(float a, float b, float c, float d, float tx, float ty);

    void Set(float a, float b, float c, float d, float tx, float ty) override;
    void Rotate(float r) override;
    void Translate(float tx, float ty) override;
    void Scale(float sx, float sy) override;
    void Concat(float a, float b, float c, float d, float tx, float ty) override;

    SkMatrix mMatrix;
};

class SkiaSVGImageData final : public ImageData
{
public:
    SkiaSVGImageData(const std::string& base64, ImageEncoding encoding);
    ~SkiaSVGImageData() override;

    float Width() const override;

    float Height() const override;

    sk_sp<SkImage> mImageData;
};

class SVG_IMP_EXP SkiaSVGRenderer final : public SVGRenderer
{
public:
    SkiaSVGRenderer();

    std::unique_ptr<ImageData> CreateImageData(const std::string& base64, ImageEncoding encoding) override { return std::unique_ptr<SkiaSVGImageData>(new SkiaSVGImageData(base64, encoding)); }

    std::unique_ptr<Path> CreatePath() override { return std::unique_ptr<SkiaSVGPath>(new SkiaSVGPath); }

    std::unique_ptr<Transform> CreateTransform(
        float a = 1.0, float b = 0.0, float c = 0.0, float d = 1.0, float tx = 0.0, float ty = 0.0) override
    {
        return std::unique_ptr<SkiaSVGTransform>(new SkiaSVGTransform(a, b, c, d, tx, ty));
    }

    void Save(const GraphicStyle& graphicStyle) override;
    void Restore() override;

    void DrawPath(const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle) override;
    void DrawImage(const ImageData& image, const GraphicStyle& graphicStyle, const Rect& clipArea, const Rect& fillArea) override;
    Rect GetBounds(const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle) override;

    void SetSkCanvas(SkCanvas* canvas);

private:
    SkCanvas* mCanvas;
};

} // namespace SVGNative

#endif // SVGViewer_SkiaSVGRenderer_h
