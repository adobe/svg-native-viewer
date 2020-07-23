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

#ifndef SVGViewer_StringSVGRenderer_h
#define SVGViewer_StringSVGRenderer_h

#include "svgnative/SVGRenderer.h"

#include <iostream>
#include <sstream>
#include <string>

namespace SVGNative
{
class StringSVGPath final : public Path
{
public:
    StringSVGPath();

    void Rect(float x, float y, float width, float height) override;
    void RoundedRect(float x, float y, float width, float height, float rx, float ry) override;
    void Ellipse(float cx, float cy, float rx, float ry) override;

    void MoveTo(float x, float y) override;
    void LineTo(float x, float y) override;
    void CurveTo(float x1, float y1, float x2, float y2, float x3, float y3) override;
    void CurveToV(float x2, float y2, float x3, float y3) override;
    void ClosePath() override;

    std::string String() const;

private:
    std::ostringstream mStringStream;
};

class StringSVGTransform final : public Transform
{
public:
    struct AffineTransform
    {
        AffineTransform() = default;
        AffineTransform(float aA, float aB, float aC, float aD, float aE, float aF)
            : a{aA}
            , b{aB}
            , c{aC}
            , d{aD}
            , e{aE}
            , f{aF}
        {
        }
        float a{1};
        float b{0};
        float c{0};
        float d{1};
        float e{0};
        float f{0};
    };

    StringSVGTransform(float a, float b, float c, float d, float tx, float ty);

    void Set(float a, float b, float c, float d, float tx, float ty) override;
    void Rotate(float r) override;
    void Translate(float tx, float ty) override;
    void Scale(float sx, float sy) override;
    void Concat(float a, float b, float c, float d, float tx, float ty) override;

    std::string String() const;

private:
    void Multiply(const AffineTransform& o);

private:
    AffineTransform mTransform{};
};

class StringSVGImageData final : public ImageData
{
public:
    StringSVGImageData(const std::string& base64, ImageEncoding encoding)
        : mBase64{base64}
    {
        mBase64.insert(0, encoding == ImageEncoding::kPNG ? "(PNG) " : "(JPEG) ");
    }

    // We are not able to encode PNG images here so we return a fixed size.
    float Width() const override { return 160.0f; }

    float Height() const override { return 110.0f; }

    std::string String() const { return mBase64; }

private:
    std::string mBase64;
};

class SVG_IMP_EXP StringSVGRenderer final : public SVGRenderer
{
public:
    StringSVGRenderer();

    std::unique_ptr<ImageData> CreateImageData(const std::string& base64, ImageEncoding encoding) override { return std::unique_ptr<StringSVGImageData>(new StringSVGImageData(base64, encoding)); }

    std::unique_ptr<Path> CreatePath() override;

    std::unique_ptr<Transform> CreateTransform(
        float a = 1.0, float b = 0.0, float c = 0.0, float d = 1.0, float tx = 0.0, float ty = 0.0) override;

    void Save(const GraphicStyle& graphicStyle) override;
    void Restore() override;

    void DrawPath(const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle) override;
    void DrawImage(const ImageData& image, const GraphicStyle& graphicStyle, const Rect& clipArea, const Rect& fillArea) override;

    std::string String() const;

private:
    void IncIndent();
    void DecIndent();

    void WriteNewline();
    void WriteIndent();

    void WriteFill(const FillStyle& fillStyle);
    void WriteStroke(const StrokeStyle& strokeStyle);
    void WriteGraphic(const GraphicStyle& graphicStyle);
    void WritePaint(const Paint& paint);

    size_t mIndent{};
    std::ostringstream mStringStream;
};

} // namespace SVGNative

#endif // SVGViewer_StringSVGRenderer_h
