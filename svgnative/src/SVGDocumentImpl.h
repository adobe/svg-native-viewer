/*
Copyright 2014 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#pragma once

#include "SVGRenderer.h"
#include "StyleSheet/Document.h"
#include "StyleSheet/Parser.h"

#include <array>
#include <boost/property_tree/detail/xml_parser_read_rapidxml.hpp>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <vector>

namespace SVGNative
{
using XMLNode = boost::property_tree::detail::rapidxml::xml_node<>;
using XMLDocument = boost::property_tree::detail::rapidxml::xml_document<>;

struct GradientImpl;

// At this point we just support 'currentColor'
enum class ColorKeys
{
    kCurrentColor
};

using Variable = std::pair<std::string, Color>;
using ColorImpl = boost::variant<Color, Variable, ColorKeys>;
using PaintImpl = boost::variant<Color, GradientImpl, Variable, ColorKeys>;
using ColorStopImpl = std::tuple<float, ColorImpl, float>;

struct GradientImpl : public Gradient
{
    std::vector<ColorStopImpl> internalColorStops;
};

struct FillStyleImpl : public FillStyle
{
    PaintImpl internalPaint = Color{{0.0f, 0.0f, 0.0f, 1.0f}};

    // Other inherited properties. We handle them here for simplicity.
    bool visibility{true};
    ColorImpl color = Color{{0.0f, 0.0f, 0.0f, 1.0f}};
    WindingRule clipRule = WindingRule::kNonZero;
};

struct StrokeStyleImpl : public StrokeStyle
{
    PaintImpl internalPaint = Color{{0.0f, 0.0f, 0.0f, 1.0f}};
};

struct GraphicStyleImpl : public GraphicStyle
{
    // Other non-inherited properties
    bool display{true};
    float stopOpacity{1.0f};
    ColorImpl stopColor = Color{{0.0f, 0.0f, 0.0f, 1.0f}};
};

class SVGDocumentImpl
{
public:
    enum class ElementType
    {
        kImage,
        kGraphic,
        kGroup
    };

    struct Element
    {
        Element(GraphicStyleImpl& aGraphicStyle, std::set<std::string>& aClasses)
            : graphicStyle{aGraphicStyle}
            , classNames{aClasses}
        {
        }

        virtual ~Element() = default;

        GraphicStyleImpl graphicStyle;
        std::set<std::string> classNames;
        virtual ElementType Type() const = 0;
    };

    struct Image : public Element
    {
        Image(GraphicStyleImpl& aGraphicStyle, std::set<std::string>& aClasses, std::unique_ptr<ImageData> aImageData,
            const Rect& aClipArea, const Rect& aFillArea)
            : Element(aGraphicStyle, aClasses)
            , imageData{std::move(aImageData)}
            , clipArea{aClipArea}
            , fillArea{aFillArea}
        {
        }

        std::unique_ptr<ImageData> imageData;
        Rect clipArea;
        Rect fillArea;
        ElementType Type() const override { return ElementType::kImage; }
    };

    struct Group : public Element
    {
        Group(GraphicStyleImpl& aGraphicStyle, std::set<std::string>& aClasses)
            : Element(aGraphicStyle, aClasses)
        {
        }

        std::vector<std::unique_ptr<Element>> children;
        ElementType Type() const override { return ElementType::kGroup; }
    };

    struct Graphic : public Element
    {
        Graphic(GraphicStyleImpl& aGraphicStyle, std::set<std::string>& aClasses, FillStyleImpl& aFillStyle, StrokeStyleImpl& aStrokeStyle,
            std::unique_ptr<Path> aPath)
            : Element(aGraphicStyle, aClasses)
            , fillStyle{aFillStyle}
            , strokeStyle{aStrokeStyle}
            , path{std::move(aPath)}
        {
        }

        FillStyleImpl fillStyle;
        StrokeStyleImpl strokeStyle;
        std::unique_ptr<Path> path;

        ElementType Type() const override { return ElementType::kGraphic; }
    };

    SVGDocumentImpl(std::shared_ptr<SVGRenderer> renderer);
    ~SVGDocumentImpl() {}

    void TraverseSVGTree();

    enum class Result
    {
        kSuccess,
        kInvalid,
        kDisabled
    };

    enum class LengthType
    {
        kHorrizontal,
        kVertical,
        kDiagonal
    };

#ifdef STYLE_SUPPORT
    void AddCustomCSS(const StyleSheet::CssDocument* cssDocument);
    void ClearCustomCSS();
#endif
    void Render(const ColorMap& colorMap, float width, float height);

    XMLDocument mXMLDocument;
    std::array<float, 4> mViewBox;
    std::shared_ptr<SVGRenderer> mRenderer;

private:
    bool HasAttr(XMLNode* node, const char* attrName);
    float ParseLengthFromAttr(XMLNode* child, const char* attrName, LengthType lengthType = LengthType::kHorrizontal, float fallback = 0);
    float RelativeLength(LengthType lengthType) const;

    float ParseColorStop(XMLNode* node, std::vector<SVGNative::ColorStopImpl>& colorStops, float lastOffset);
    void ParseColorStops(XMLNode* node, SVGNative::GradientImpl& gradient);
    void ParseGradient(XMLNode* gradient);

    void ParseResources(XMLNode* node);
    void ParseResource(XMLNode* node);

    void ParseChildren(XMLNode* node);
    void ParseChild(XMLNode* node);

    std::unique_ptr<Path> ParseShape(XMLNode* node);

    GraphicStyleImpl ParseGraphic(XMLNode* node, FillStyleImpl& fillStyle, StrokeStyleImpl& strokeStyle, std::set<std::string>& classNames);
    void ParseFillProperties(FillStyleImpl& fillStyle, const StyleSheet::CssPropertySet& propertySet);
    void ParseStrokeProperties(StrokeStyleImpl& strokeStyle, const StyleSheet::CssPropertySet& propertySet);
    void ParseGraphicsProperties(GraphicStyleImpl& graphicsStyle, const StyleSheet::CssPropertySet& propertySet);

    StyleSheet::CssPropertySet ParsePresentationAttributes(XMLNode* node);

#ifdef STYLE_SUPPORT
    void ParseStyle(XMLNode* child);
#endif

    void TraverseTree(const ColorMap& colorMap, const Element*);
    void ApplyCSSStyle(
        const std::set<std::string>& classNames, GraphicStyleImpl& graphicStyle, FillStyleImpl& fillStyle, StrokeStyleImpl& strokeStyle);

    void AddChildToCurrentGroup(std::unique_ptr<Element> element);

private:
    // All stroke and fill CSS properties are so called
    // inherited CSS properties. Ancestors can define the
    // stroke properties for desandents. Decendants override
    // specifies from ancestors.
    // We need to keep the stack of settings in based on DOM
    // hiearchy.
    std::stack<StrokeStyleImpl> mStrokeStyleStack;
    std::stack<FillStyleImpl> mFillStyleStack;

    const StyleSheet::CssDocument* mOverrideStyle{};
    StyleSheet::CssDocument mCSSInfo;
    StyleSheet::CssDocument mCustomCSSInfo;

    std::map<std::string, GradientImpl> mGradients;
    std::map<std::string, XMLNode*> mResourceIDs;
    std::map<std::string, std::shared_ptr<ClippingPath>> mClippingPaths;

    std::stack<Group*> mGroupStack;
    std::unique_ptr<Group> mGroup;

#if DEBUG
    std::string mTitle;
#endif
};

} // namespace SVGNative
