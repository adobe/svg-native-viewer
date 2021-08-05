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

#ifndef SVGViewer_SVGRenderer_h
#define SVGViewer_SVGRenderer_h

#include "Config.h"

#include <array>
#include <boost/variant.hpp>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace SVGNative
{
/**
 * Supported image encoding formats are PNG and JPEG.
 * The assumed encoding format based on the base64 string.
 */
enum class ImageEncoding
{
    kPNG,
    kJPEG
};

/**
 * Line caps as described in:
 * https://www.w3.org/TR/SVG2/painting.html#LineCaps
 */
enum class LineCap
{
    kButt,
    kRound,
    kSquare
};

/**
 * Line joins as described in:
 * https://www.w3.org/TR/SVG2/painting.html#LineJoin
 */
enum class LineJoin
{
    kMiter,
    kRound,
    kBevel
};

/**
 * Winding rules as described in:
 * https://www.w3.org/TR/SVG2/painting.html#WindingRule
 */
enum class WindingRule
{
    kNonZero,
    kEvenOdd
};

/**
 * Gradient type. SVG Native supports the 2 gradient types
 * * linear gradient and
 * * radial gradient.
 */
enum class GradientType
{
    kLinearGradient,
    kRadialGradient
};

/**
 * Gradient spread method.
 * * pad
 * * reflect
 * * repeat
 *
 * @note See https://www.w3.org/TR/SVG11/pservers.html#LinearGradientElementSpreadMethodAttribute
 */
enum class SpreadMethod
{
    kPad,
    kReflect,
    kRepeat
};

struct Gradient;
class Transform;
class Path;
class Shape;
class Interval;

using Color = std::array<float, 4>;
using Paint = boost::variant<Color, Gradient>;
using ColorStop = std::pair<float, Color>;
using ColorMap = std::map<std::string, Color>;


/**
 * Represents a rectangle.
 *
 * The following points hold true for a Rectangle:
 * 1. A rectangle is simply defined by a pair of two Intervals. The set
 * of a rectangle $R$ defined by two Intervals $A$ and $B$ is the set
 * of all points $(x, y)$ such that $x$ is a member of Interval $A$'s
 * set while $y$ is a member of Interval $B$'s set.
 * 2. A rectangle is empty if its set is of length zero.
 * 3. A rectangle $A$ contains a rectangle $B$ if all points of rectangle
 * $B$'s set are also members of the set of rectangle $A$.
 * 4. The intersection of two rectangles $A$ and $B$ is a rectangle whose
 * set contains the elements that are common in the point set of $A$ and
 * that of $B$.
 * 5. The join of two rectangles $A$ and $B$ is a rectangle with the smallest
 * possible point set such that it has all points of $A$ as well as those of
 * set $B$ and is complete, meaning that for any two points in the set, all
 * the points lying between will also be a part of the set. By smallest we mean
 * that no other proper subset of that set should satisfy this requirement.
 */
class Rect
{
using IntervalPair = std::tuple<Interval, Interval>;
  public:

    Rect() = default;
    Rect(float aX, float aY, float aWidth, float aHeight);
    /* Returns if the rectangle is empty */
    bool IsEmpty() const;
    /* Returns the two intervals defining the rectangle */
    /* Returns true if Rect contains the other Rect within it */
    bool Contains(Rect other) const;
    /* Computes the intersection of Rect with the other Rect, meaning a rectangle encompassing area
     * that is common in both. */
    Rect operator&(Rect other) const;
    /* Returns true if two rectangles are the same */
    bool operator==(Rect other) const;
    /* Computes the join of two rectangles, meaning a bigger rectangle that contains both of those. */
    Rect operator|(Rect other) const;
    float Area() const { return width * height; }
    float MaxDiffVertex(Rect other) const;
    float Left() const { return x; }
    float Right() const { return x + width; }
    float Top() const { return y; }
    float Bottom() const { return y + height; }

    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;

  private:
    IntervalPair Intervals() const;
};

/**
 * Representation of a linear gradient paint server.
 */
struct Gradient
{
    GradientType type = GradientType::kLinearGradient;
    SpreadMethod method = SpreadMethod::kPad;
    std::vector<ColorStop> colorStops; /** Color stops with offset-color pairs **/
    float x1 = std::numeric_limits<float>::quiet_NaN(); /** x1 for linearGradient **/
    float y1 = std::numeric_limits<float>::quiet_NaN(); /** y1 for linearGradient **/
    float x2 = std::numeric_limits<float>::quiet_NaN(); /** x2 for linearGradient **/
    float y2 = std::numeric_limits<float>::quiet_NaN(); /** y2 for linearGradient **/
    float cx = std::numeric_limits<float>::quiet_NaN(); /** cx for radialGradient **/
    float cy = std::numeric_limits<float>::quiet_NaN(); /** cy for radialGradient **/
    float fx = std::numeric_limits<float>::quiet_NaN(); /** fx for radialGradient **/
    float fy = std::numeric_limits<float>::quiet_NaN(); /** fy for radialGradient **/
    float r = std::numeric_limits<float>::quiet_NaN(); /** r for radialGradient **/
    std::shared_ptr<Transform> transform; /** Joined transformation matrix based to the "transform" attribute. **/
};

/**
 * Stroke style information.
 */
struct StrokeStyle
{
    bool hasStroke = false;
    float strokeOpacity = 1.0;
    float lineWidth = 1.0;
    LineCap lineCap = LineCap::kButt;
    LineJoin lineJoin = LineJoin::kMiter;
    float miterLimit = 4.0;
    std::vector<float> dashArray;
    float dashOffset = 0.0;
    Paint paint = Color{{0, 0, 0, 1.0}};
};

/**
 * Fill style information.
 */
struct FillStyle
{
    bool hasFill = true;
    WindingRule fillRule = WindingRule::kNonZero;
    float fillOpacity = 1.0;
    Paint paint = Color{{0, 0, 0, 1.0}};
};

/**
 * Representation of a 2D affine transform with 6 values.
 */
class Transform
{
public:
    virtual ~Transform() = default;

    virtual void Set(float a, float b, float c, float d, float tx, float ty) = 0;
    virtual void Rotate(float r) = 0;
    virtual void Translate(float tx, float ty) = 0;
    virtual void Scale(float sx, float sy) = 0;
    virtual void Concat(float a, float b, float c, float d, float tx, float ty) = 0;
};

struct ClippingPath
{
    ClippingPath(bool aHasClipContent, WindingRule aClipRule, std::shared_ptr<Path> aPath, std::shared_ptr<Transform> aTransform)
        : hasClipContent{aHasClipContent}
        , clipRule{aClipRule}
        , path{aPath}
        , transform{aTransform}
    {}

    bool hasClipContent = false;
    WindingRule clipRule = WindingRule::kNonZero;
    std::shared_ptr<Path> path; /** Clipping path. **/
    std::shared_ptr<Transform> transform; /** Joined transformation matrix based to the "transform" attribute. **/
};

/**
 * All compositing related properties. With the exception of the
 */
struct GraphicStyle
{
    // Add blend modes and other graphic style options here.
    float opacity = 1.0; /** Corresponds to the "opacity" CSS property. **/
    std::shared_ptr<Transform> transform; /** Joined transformation matrix based to the "transform" attribute. **/
    std::shared_ptr<ClippingPath> clippingPath;
};

/**
 * A presentation of a path.
 */
class Path
{
public:
    virtual ~Path() = default;

    virtual void Rect(float x, float y, float width, float height) = 0;
    virtual void RoundedRect(float x, float y, float width, float height, float cornerRadiusX, float cornerRadiusY) = 0;
    virtual void Ellipse(float cx, float cy, float rx, float ry) = 0;

    virtual void MoveTo(float x, float y) = 0;
    virtual void LineTo(float x, float y) = 0;
    virtual void CurveTo(float x1, float y1, float x2, float y2, float x3, float y3) = 0;
    virtual void CurveToV(float x2, float y2, float x3, float y3) = 0;
    virtual void ClosePath() = 0;
};

/**
 * An image object generated from a base64 string.
 * The port needs to decode the Base64 string and provide
 * information about the dimensions of the image.
 **/
class ImageData
{
public:
    virtual ~ImageData() = default;

    virtual float Width() const = 0;
    virtual float Height() const = 0;
};

/**
 * Base class for deriving, platform dependent renderer classes with immediate
 * graphic library calls.
 */
class SVGRenderer
{
public:
    virtual ~SVGRenderer() = default;

    virtual std::unique_ptr<ImageData> CreateImageData(const std::string& base64, ImageEncoding) = 0;
    virtual std::unique_ptr<Path> CreatePath() = 0;
    virtual std::unique_ptr<Transform> CreateTransform(
        float a = 1.0, float b = 0.0, float c = 0.0, float d = 1.0, float tx = 0.0, float ty = 0.0) = 0;

    virtual void Save(const GraphicStyle& graphicStyle) = 0;
    virtual void Restore() = 0;

    virtual void DrawPath(
        const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle) = 0;
    virtual void DrawImage(const ImageData& image, const GraphicStyle& graphicStyle, const Rect& clipArea, const Rect& fillArea) = 0;
    virtual Rect GetBounds(const Path& path, const GraphicStyle& graphicStyle, const FillStyle& fillStyle, const StrokeStyle& strokeStyle)
    {
      throw "Bound calculation functionality not implemented in this port";
      return Rect{};
    }
};

class SaveRestoreHelper
{
public:
    SaveRestoreHelper(std::weak_ptr<SVGRenderer> renderer, const GraphicStyle& graphicStyle)
        : mRenderer{renderer}
    {
        if (auto renderer = mRenderer.lock())
            renderer->Save(graphicStyle);
    }

    ~SaveRestoreHelper()
    {
        if (auto renderer = mRenderer.lock())
            renderer->Restore();
    }
private:
    std::weak_ptr<SVGRenderer> mRenderer{};
};

} // namespace SVGNative

#endif // SVGViewer_SVGRenderer_h
