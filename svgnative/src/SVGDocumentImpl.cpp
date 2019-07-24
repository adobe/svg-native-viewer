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

#include "SVGDocumentImpl.h"
#include "SVGDocument.h"
#include "SVGRenderer.h"
#include "SVGStringParser.h"

#include <cmath>

namespace SVGNative
{
constexpr std::array<const char*, 14> gInheritedPropertyNames{{
    "color",
    "clip-rule",
    "fill",
    "fill-rule",
    "fill-opacity",
    "stroke",
    "stroke-dasharray",
    "stroke-dashoffset",
    "stroke-linecap",
    "stroke-linejoin",
    "stroke-miterlimit",
    "stroke-opacity",
    "stroke-width",
    "visibility"
}};

constexpr std::array<const char*, 5> gNonInheritedPropertyNames{{
    "clip-path",
    "display",
    "opacity",
    "stop-opacity",
    "stop-color"
}};

SVGDocumentImpl::SVGDocumentImpl(std::shared_ptr<SVGRenderer> renderer)
    : mViewBox{{0, 0, 320.0f, 200.0f}}
    , mRenderer{renderer}
{
    mFillStyleStack.push(FillStyleImpl());
    mStrokeStyleStack.push(StrokeStyleImpl());

    GraphicStyleImpl graphicStyle{};
    std::set<std::string> classNames;
    mGroup = std::unique_ptr<Group>(new Group(graphicStyle, classNames));
    mGroupStack.push(mGroup.get());
}

void SVGDocumentImpl::TraverseSVGTree()
{
    auto rootNode = mXMLDocument.first_node();
    if (std::string(rootNode->name()) != "svg")
        return;

    if (!HasAttr(rootNode, "viewBox"))
    {
        if (HasAttr(rootNode, "x"))
            mViewBox[0] = SVGDocumentImpl::ParseLengthFromAttr(rootNode, "x", LengthType::kHorrizontal);
        if (HasAttr(rootNode, "y"))
            mViewBox[1] = SVGDocumentImpl::ParseLengthFromAttr(rootNode, "y", LengthType::kVertical);
        if (HasAttr(rootNode, "width"))
            mViewBox[2] = SVGDocumentImpl::ParseLengthFromAttr(rootNode, "width", LengthType::kHorrizontal);
        if (HasAttr(rootNode, "height"))
            mViewBox[3] = SVGDocumentImpl::ParseLengthFromAttr(rootNode, "height", LengthType::kVertical);
    }
    else
    {
        auto attr = rootNode->first_attribute("viewBox");
        std::vector<float> numberList;
        if (SVGStringParser::ParseListOfNumbers(std::string(attr->value()), numberList) && numberList.size() == 4)
            mViewBox = {{numberList[0], numberList[1], numberList[2], numberList[3]}};
    }

#if DEBUG
    if (HasAttr(rootNode, "data-name"))
    {
        auto attr = rootNode->first_attribute("data-name");
        if (attr)
            mTitle = attr->value();
    }
#endif

    ParseChildren(rootNode);

    // Clear all temporary sets
    mGradients.clear();
    mResourceIDs.clear();
    mClippingPaths.clear();
}

bool SVGDocumentImpl::HasAttr(XMLNode* node, const char* attrName)
{
    auto attr = node->first_attribute(attrName);
    return attr != nullptr;
}

float SVGDocumentImpl::RelativeLength(LengthType lengthType) const
{
    float diagonal = sqrtf(mViewBox[2] * mViewBox[2] + mViewBox[3] * mViewBox[3]);
    float relLength{};
    switch (lengthType)
    {
    case LengthType::kHorrizontal:
        relLength = mViewBox[2];
        break;
    case LengthType::kVertical:
        relLength = mViewBox[3];
        break;
    case LengthType::kDiagonal:
        relLength = diagonal;
        break;
    default:
        break;
    }
    return relLength;
}

float SVGDocumentImpl::ParseLengthFromAttr(XMLNode* node, const char* attrName, LengthType lengthType, float fallback)
{
    auto attr = node->first_attribute(attrName);
    if (!attr)
        return fallback;

    float number{};
    if (!SVGStringParser::ParseLengthOrPercentage(attr->value(), RelativeLength(lengthType), number, true))
        return fallback;

    return number;
}

void SVGDocumentImpl::ParseChildren(XMLNode* node)
{
    for (auto child = node->first_node(); child != nullptr; child = child->next_sibling())
    {
        ParseChild(child);
    }
}

void SVGDocumentImpl::ParseChild(XMLNode* child)
{
    auto fillStyle = mFillStyleStack.top();
    auto strokeStyle = mStrokeStyleStack.top();
    std::set<std::string> classNames;
    auto graphicStyle = ParseGraphic(child, fillStyle, strokeStyle, classNames);

    // Check if we have a shape rect, circle, ellipse, line, polygon, polyline
    // or path first.
    if (auto path = ParseShape(child))
    {
        AddChildToCurrentGroup(std::unique_ptr<Graphic>(new Graphic(graphicStyle, classNames, fillStyle, strokeStyle, std::move(path))));
        return;
    }

    // Look at all elements that are no shapes.
    std::string elementName = child->name();
    if (elementName == "g")
    {
        mFillStyleStack.push(fillStyle);
        mStrokeStyleStack.push(strokeStyle);

        auto group = std::unique_ptr<Group>(new Group(graphicStyle, classNames));
        auto tempGroupPtr = group.get();
        AddChildToCurrentGroup(std::move(group));
        mGroupStack.push(tempGroupPtr);

        ParseChildren(child);

        mGroupStack.pop();
        mFillStyleStack.pop();
        mStrokeStyleStack.pop();
    }
    else if (elementName == "defs")
    {
        mFillStyleStack.push(fillStyle);
        mStrokeStyleStack.push(strokeStyle);

        ParseResources(child);

        mFillStyleStack.pop();
        mStrokeStyleStack.pop();
    }
    else if (elementName == "image")
    {
        std::unique_ptr<ImageData> imageData;
        auto hrefAttr = child->first_attribute("xlink:href");
        if (hrefAttr)
        {
            std::string dataURL = hrefAttr->value();
            ImageEncoding encoding{};
            size_t base64Offset{};
            if (dataURL.find("data:image/png;base64,") == 0)
            {
                encoding = ImageEncoding::kPNG;
                base64Offset = 22;
            }
            else if (dataURL.find("data:image/jpg;base64,") == 0)
            {
                encoding = ImageEncoding::kJPEG;
                base64Offset = 22;
            }
            else if (dataURL.find("data:image/jpeg;base64,") == 0)
            {
                encoding = ImageEncoding::kJPEG;
                base64Offset = 23;
            }
            else
                return;
            imageData = mRenderer->CreateImageData(dataURL.substr(base64Offset), encoding);
        }

        if (imageData)
        {
            float imageWidth = imageData->Width();
            float imageHeight = imageData->Height();

            Rect clipArea{ParseLengthFromAttr(child, "x", LengthType::kHorrizontal),
                ParseLengthFromAttr(child, "y", LengthType::kVertical),
                ParseLengthFromAttr(child, "width", LengthType::kHorrizontal, imageWidth),
                ParseLengthFromAttr(child, "height", LengthType::kVertical, imageHeight)};

            std::string align;
            std::string meetOrSlice;
            std::vector<std::string> attrStringValues;
            auto preserveAspectRatioAttr = child->first_attribute("preserveAspectRatio");
            if (preserveAspectRatioAttr && SVGStringParser::ParseListOfStrings(preserveAspectRatioAttr->value(), attrStringValues)
                && attrStringValues.size() >= 1 && attrStringValues.size() <= 2)
            {
                align = attrStringValues[0];
                if (attrStringValues.size() == 2)
                    meetOrSlice = attrStringValues[1];
            }

            Rect fillArea{clipArea};
            if (align.compare("none") != 0)
            {
                fillArea.width = imageWidth;
                fillArea.height = imageHeight;
                float scaleX = clipArea.width / imageWidth;
                float scaleY = clipArea.height / imageHeight;
                float scale{};
                if (meetOrSlice.compare("slice") == 0)
                    scale = std::max(scaleX, scaleY);
                else
                    scale = std::min(scaleX, scaleY);
                fillArea.width *= scale;
                fillArea.height *= scale;

                if (align.compare("xMinYMin") == 0)
                {
                    fillArea.x = clipArea.x;
                    fillArea.y = clipArea.y;
                }
                else if (align.compare("xMidYMin") == 0)
                {
                    fillArea.x = (clipArea.x + clipArea.width / 2) - fillArea.width / 2;
                    fillArea.y = clipArea.y;
                }
                else if (align.compare("xMaxYMin") == 0)
                {
                    fillArea.x = clipArea.x + clipArea.width - fillArea.width;
                    fillArea.y = clipArea.y;
                }
                else if (align.compare("xMinYMid") == 0)
                {
                    fillArea.x = clipArea.x;
                    fillArea.y = (clipArea.y + clipArea.height / 2) - fillArea.height / 2;
                }
                else if (align.compare("xMaxYMid") == 0)
                {
                    fillArea.x = clipArea.x + clipArea.width - fillArea.width;
                    fillArea.y = (clipArea.y + clipArea.height / 2) - fillArea.height / 2;
                }
                else if (align.compare("xMinYMax") == 0)
                {
                    fillArea.x = clipArea.x;
                    fillArea.y = clipArea.y + clipArea.height - fillArea.height;
                }
                else if (align.compare("xMidYMax") == 0)
                {
                    fillArea.x = (clipArea.x + clipArea.width / 2) - fillArea.width / 2;
                    fillArea.y = clipArea.y + clipArea.height - fillArea.height;
                }
                else if (align.compare("xMaxYMax") == 0)
                {
                    fillArea.x = clipArea.x + clipArea.width - fillArea.width;
                    fillArea.y = clipArea.y + clipArea.height - fillArea.height;
                }
                else // default and "xMidYMid"
                {
                    fillArea.x = (clipArea.x + clipArea.width / 2) - fillArea.width / 2;
                    fillArea.y = (clipArea.y + clipArea.height / 2) - fillArea.height / 2;
                }
            }

            // Do not render 0-sized elements.
            if (imageWidth && imageHeight && clipArea.width && clipArea.height && fillArea.width && fillArea.height)
            {
                auto image = std::unique_ptr<Image>(new Image(graphicStyle, classNames, std::move(imageData), clipArea, fillArea));
                AddChildToCurrentGroup(std::move(image));
            }
        }
    }
    else if (elementName == "use")
    {
        auto href = child->first_attribute("xlink:href");
        if (!href)
            return;

        if (href->value()[0] != '#')
            return;

        auto resourceIt = mResourceIDs.find((href->value() + 1));
        if (resourceIt == mResourceIDs.end())
            return;

        mFillStyleStack.push(fillStyle);
        mStrokeStyleStack.push(strokeStyle);

        auto transform = mRenderer->CreateTransform(
            1, 0, 0, 1, ParseLengthFromAttr(child, "x", LengthType::kHorrizontal), ParseLengthFromAttr(child, "y", LengthType::kVertical));
        if (graphicStyle.transform)
            transform->Concat(*graphicStyle.transform);
        graphicStyle.transform = std::move(transform);

        auto group = std::unique_ptr<Group>(new Group(graphicStyle, classNames));
        auto tempGroupPtr = group.get();
        AddChildToCurrentGroup(std::move(group));
        mGroupStack.push(tempGroupPtr);

        if(resourceIt->second->first_node() == 0)
            ParseChild(resourceIt->second);
        else
            ParseChildren(resourceIt->second);

        mGroupStack.pop();
        mFillStyleStack.pop();
        mStrokeStyleStack.pop();
    }
    else if (elementName == "symbol")
    {
        // FIXME: Do not render <symbol> outside of <defs> section.
        // FIXME: Remove support for symbol ASAP.
        auto attr = child->first_attribute("viewBox");
        if (attr)
        {
            std::vector<float> numberList;
            if (SVGStringParser::ParseListOfNumbers(std::string(attr->value()), numberList) && numberList.size() == 4)
                graphicStyle.transform = mRenderer->CreateTransform(1, 0, 0, 1, -numberList[0], -numberList[1]);
            {
                mViewBox = {{numberList[0], numberList[1], numberList[2], numberList[3]}};
            }
        }

        auto group = std::unique_ptr<Group>(new Group(graphicStyle, classNames));
        mGroupStack.push(group.get());
        AddChildToCurrentGroup(std::move(group));

        ParseChildren(child);

        mGroupStack.pop();
    }
    else if (elementName == "style")
        ParseStyle(child);
}

void SVGDocumentImpl::ParseResources(XMLNode* node)
{
    for (auto child = node->first_node(); child != nullptr; child = child->next_sibling())
    {
        ParseResource(child);
    }
}

void SVGDocumentImpl::ParseResource(XMLNode* child)
{
    auto fillStyle = mFillStyleStack.top();
    auto strokeStyle = mStrokeStyleStack.top();
    std::set<std::string> classNames;
    auto graphicStyle = ParseGraphic(child, fillStyle, strokeStyle, classNames);

    std::string elementName = child->name();
    if (elementName == "linearGradient" || elementName == "radialGradient")
    {
        mFillStyleStack.push(fillStyle);
        mStrokeStyleStack.push(strokeStyle);

        ParseGradient(child);

        mFillStyleStack.pop();
        mStrokeStyleStack.pop();
    }
    else if (elementName == "style")
        ParseStyle(child);
    else if (elementName == "clipPath")
    {
        auto id = child->first_attribute("id");
        if (!id)
            return;

        mFillStyleStack.push(fillStyle);
        mStrokeStyleStack.push(strokeStyle);

        // SVG only allows shapes (and <use> elements referencing shapes) as children of
        // <clipPath>. Ignore all other elements.
        bool hasClipContent{false};
        for (auto clipPathChild = child->first_node(); clipPathChild != nullptr; clipPathChild = clipPathChild->next_sibling())
        {
            // WebKit and Blink allow the clipping path if there is at least one valid basic shape child.
            if (auto path = ParseShape(clipPathChild))
            {
                std::unique_ptr<Transform> transform;
                if (auto transformAttr = clipPathChild->first_attribute("transform"))
                {
                    auto transformHandler = [&]() {
                        SVG_ASSERT(mRenderer != nullptr);
                        return mRenderer->CreateTransform();
                    };
                    transform = SVGStringParser::ParseTransform(transformAttr->value(), transformHandler);
                }
                auto fillStyleChild = mFillStyleStack.top();
                auto strokeStyleChild = mStrokeStyleStack.top();
                std::set<std::string> classNames;
                ParseGraphic(child, fillStyleChild, strokeStyleChild, classNames);
                mClippingPaths[id->value()] = std::make_shared<ClippingPath>(true, fillStyleChild.clipRule, std::move(path), std::move(transform));
                hasClipContent = true;
                break;
            }
        }
        if (!hasClipContent)
            mClippingPaths[id->value()] = std::make_shared<ClippingPath>(false, WindingRule::kNonZero, nullptr, nullptr);
        mFillStyleStack.pop();
        mStrokeStyleStack.pop();
    }
    else
    {
        auto id = child->first_attribute("id");
        if (!id)
            return;

        mResourceIDs[id->value()] = child;
    }
}

std::unique_ptr<Path> SVGDocumentImpl::ParseShape(XMLNode* child)
{
    std::string elementName = child->name();
    if (elementName == "rect")
    {
        float x = ParseLengthFromAttr(child, "x", LengthType::kHorrizontal);
        float y = ParseLengthFromAttr(child, "y", LengthType::kVertical);

        float width = ParseLengthFromAttr(child, "width", LengthType::kHorrizontal);
        float height = ParseLengthFromAttr(child, "height", LengthType::kVertical);

        bool hasRx = HasAttr(child, "rx");
        bool hasRy = HasAttr(child, "ry");

        float rx{};
        float ry{};
        if (hasRx && hasRy)
        {
            rx = ParseLengthFromAttr(child, "rx", LengthType::kHorrizontal);
            ry = ParseLengthFromAttr(child, "ry", LengthType::kVertical);
        }
        else if (hasRx)
        {
            // the svg spec says that rect elements that specify a rx but not a ry
            // should use the rx value for ry
            rx = ParseLengthFromAttr(child, "rx", LengthType::kHorrizontal);
            ry = rx;
        }
        else if (hasRy)
        {
            // the svg spec says that rect elements that specify a ry but not a rx
            // should use the ry value for rx
            ry = ParseLengthFromAttr(child, "ry", LengthType::kVertical);
            rx = ry;
        }
        else
        {
            rx = 0;
            ry = 0;
        }

        rx = std::min(rx, width / 2.0f);
        ry = std::min(ry, height / 2.0f);

        auto path = mRenderer->CreatePath();
        if (rx == 0 && ry == 0)
        {
            path->Rect(x, y, width, height);
        }
        else
        {
            SVG_ASSERT(rx == ry);
            path->RoundedRect(x, y, width, height, std::max(rx, ry));
        }
        return path;
    }
    else if (elementName == "ellipse" || elementName == "circle")
    {
        float rx{}, ry{};

        if (elementName == "ellipse")
        {
            rx = ParseLengthFromAttr(child, "rx", LengthType::kHorrizontal);
            ry = ParseLengthFromAttr(child, "ry", LengthType::kVertical);
        }
        else
        {
            rx = ParseLengthFromAttr(child, "r", LengthType::kDiagonal);
            ry = rx;
        }

        float cx = ParseLengthFromAttr(child, "cx", LengthType::kHorrizontal);
        float cy = ParseLengthFromAttr(child, "cy", LengthType::kVertical);

        auto path = mRenderer->CreatePath();
        path->Ellipse(cx, cy, rx, ry);

        return path;
    }
    else if (elementName == "polygon" || elementName == "polyline")
    {
        auto attr = child->first_attribute("points");
        if (!attr)
            return nullptr;
        // This does not follow the spec which requires at least one space or comma between
        // coordinate pairs. However, Blink and WebKit do it the same way.
        std::vector<float> numberList;
        SVGStringParser::ParseListOfNumbers(attr->value(), numberList);
        size_t size = numberList.size();
        auto path = mRenderer->CreatePath();
        if (size > 1)
        {
            if (size % 2 == 1)
                --size;
            size_t i{};
            path->MoveTo(numberList[i], numberList[i + 1]);
            i += 2;
            for (; i < size; i += 2)
                path->LineTo(numberList[i], numberList[i + 1]);
            if (elementName == "polygon")
                path->ClosePath();
        }

        return path;
    }
    else if (elementName == "path")
    {
        auto attr = child->first_attribute("d");
        if (!attr)
            return nullptr;

        auto path = mRenderer->CreatePath();
        SVGStringParser::ParsePathString(attr->value(), *path);

        return path;
    }
    else if (elementName == "line")
    {
        auto path = mRenderer->CreatePath();
        path->MoveTo(ParseLengthFromAttr(child, "x1", LengthType::kHorrizontal), ParseLengthFromAttr(child, "y1", LengthType::kVertical));
        path->LineTo(ParseLengthFromAttr(child, "x2", LengthType::kHorrizontal), ParseLengthFromAttr(child, "y2", LengthType::kVertical));

        return path;
    }
    return nullptr;
}

GraphicStyleImpl SVGDocumentImpl::ParseGraphic(
    XMLNode* node, FillStyleImpl& fillStyle, StrokeStyleImpl& strokeStyle, std::set<std::string>& classNames)
{
    std::vector<PropertySet> propertySets;
    propertySets.push_back(ParsePresentationAttributes(node));
    ParseStyleAttr(node, propertySets, classNames);

    GraphicStyleImpl graphicStyle{};
    for (const auto& propertySet : propertySets)
    {
        ParseGraphicsProperties(graphicStyle, propertySet);
        ParseFillProperties(fillStyle, propertySet);
        ParseStrokeProperties(strokeStyle, propertySet);
    }

    auto attr = node->first_attribute("transform");
    if (attr)
    {
        auto transformHandler = [&]() {
            SVG_ASSERT(mRenderer != nullptr);
            return mRenderer->CreateTransform();
        };
        graphicStyle.transform = SVGStringParser::ParseTransform(attr->value(), transformHandler);
    }

    return graphicStyle;
}

PropertySet SVGDocumentImpl::ParsePresentationAttributes(XMLNode* node)
{
    PropertySet propertySet;
    auto attributeHandler = [&](const std::string& propertyName) {
        auto attr = node->first_attribute(propertyName.c_str());
        if (attr)
            propertySet.insert({propertyName, attr->value()});
    };
    for (const auto& propertyName : gInheritedPropertyNames)
        attributeHandler(propertyName);
    for (const auto& propertyName : gNonInheritedPropertyNames)
        attributeHandler(propertyName);
    return propertySet;
}

void SVGDocumentImpl::ParseFillProperties(FillStyleImpl& fillStyle, const PropertySet& propertySet)
{
    auto prop = propertySet.find("fill");
    auto iterEnd = propertySet.end();
    if (prop != iterEnd)
    {
        auto result = SVGStringParser::ParsePaint(prop->second.c_str(), mGradients, mViewBox, fillStyle.internalPaint);
        if (result == SVGDocumentImpl::Result::kDisabled)
            fillStyle.hasFill = false;
        else if (result == SVGDocumentImpl::Result::kSuccess)
            fillStyle.hasFill = true;
    }

    prop = propertySet.find("fill-opacity");
    if (prop != iterEnd)
    {
        float opacity{};
        if (SVGStringParser::ParseNumber(prop->second.c_str(), opacity))
            fillStyle.fillOpacity = std::max<float>(0.0, std::min<float>(1.0, opacity));
    }

    prop = propertySet.find("fill-rule");
    if (prop != iterEnd)
    {
        auto fillRuleString = std::string(prop->second.c_str());
        if (fillRuleString == "evenodd")
            fillStyle.fillRule = WindingRule::kEvenOdd;
        else if (fillRuleString == "nonzero")
            fillStyle.fillRule = WindingRule::kNonZero;
    }

    // Other inherited properties
    prop = propertySet.find("color");
    if (prop != iterEnd)
    {
        ColorImpl color = Color{{0.0f, 0.0f, 0.0f, 1.0f}};
        auto result = SVGStringParser::ParseColor(prop->second.c_str(), color, false);
        if (result == SVGDocumentImpl::Result::kSuccess)
            fillStyle.color = color;
    }

    prop = propertySet.find("visibility");
    if (prop != iterEnd)
    {
        auto visibilityString = std::string(prop->second.c_str());
        if (visibilityString == "hidden")
            fillStyle.visibility = false;
        else if (visibilityString == "collapse" || visibilityString == "visible")
            fillStyle.visibility = true;
    }

    prop = propertySet.find("clip-rule");
    if (prop != iterEnd)
    {
        if (std::string(prop->second.c_str()) == "evenodd")
            fillStyle.clipRule = WindingRule::kEvenOdd;
        else if (std::string(prop->second.c_str()) == "nonzero")
            fillStyle.clipRule = WindingRule::kNonZero;
    }
}

void SVGDocumentImpl::ParseStrokeProperties(StrokeStyleImpl& strokeStyle, const PropertySet& propertySet)
{
    auto prop = propertySet.find("stroke");
    auto iterEnd = propertySet.end();
    if (prop != iterEnd)
    {
        std::string strokeValue = prop->second;
        auto result = SVGStringParser::ParsePaint(strokeValue.c_str(), mGradients, mViewBox, strokeStyle.internalPaint);
        if (result == SVGDocumentImpl::Result::kDisabled)
            strokeStyle.hasStroke = false;
        else if (result == SVGDocumentImpl::Result::kSuccess)
            strokeStyle.hasStroke = true;
    }

    prop = propertySet.find("stroke-width");
    if (prop != iterEnd)
    {
        float strokeWidth{};
        // Ignore stroke-width if invalid or negative.
        if (SVGStringParser::ParseLengthOrPercentage(prop->second, RelativeLength(LengthType::kDiagonal), strokeWidth, true)
            && strokeWidth >= 0)
            strokeStyle.lineWidth = strokeWidth;
        // Disable stroke on a stroke-width of 0.
        if (strokeWidth == 0.0)
            strokeStyle.hasStroke = false;
    }

    prop = propertySet.find("stroke-linecap");
    if (prop != iterEnd)
    {
        if (prop->second == "round")
            strokeStyle.lineCap = LineCap::kRound;
        else if (prop->second == "square")
            strokeStyle.lineCap = LineCap::kSquare;
    }

    prop = propertySet.find("stroke-linejoin");
    if (prop != iterEnd)
    {
        if (prop->second == "round")
            strokeStyle.lineJoin = LineJoin::kRound;
        else if (prop->second == "bevel")
            strokeStyle.lineJoin = LineJoin::kBevel;
    }

    prop = propertySet.find("stroke-miterlimit");
    if (prop != iterEnd)
    {
        float miter{};
        // Miter must be bigger 1. Otherwise ignore.
        if (SVGStringParser::ParseNumber(prop->second.c_str(), miter) && miter >= 1)
            strokeStyle.miterLimit = miter;
    }

    prop = propertySet.find("stroke-dashoffset");
    if (prop != iterEnd)
    {
        float strokeDashoffset{};
        if (SVGStringParser::ParseLengthOrPercentage(prop->second, RelativeLength(LengthType::kDiagonal), strokeDashoffset, true))
            strokeStyle.dashOffset = strokeDashoffset;
    }

    prop = propertySet.find("stroke-dasharray");
    if (prop != iterEnd)
    {
        float diagonal = sqrtf(mViewBox[2] * mViewBox[2] + mViewBox[3] * mViewBox[3]);
        if (!SVGStringParser::ParseListOfLengthOrPercentage(prop->second.c_str(), diagonal, strokeStyle.dashArray, true))
            strokeStyle.dashArray.clear();
        for (auto it = strokeStyle.dashArray.begin(); it < strokeStyle.dashArray.end(); ++it)
        {
            if (*it < 0)
            {
                strokeStyle.dashArray.clear();
                break;
            }
        }
    }

    prop = propertySet.find("stroke-opacity");
    if (prop != iterEnd)
    {
        float opacity{};
        if (SVGStringParser::ParseNumber(prop->second.c_str(), opacity))
            strokeStyle.strokeOpacity = std::max<float>(0.0, std::min<float>(1.0, opacity));
    }
}

void SVGDocumentImpl::ParseGraphicsProperties(GraphicStyleImpl& graphicStyle, const PropertySet& propertySet)
{
    auto prop = propertySet.find("opacity");
    auto iterEnd = propertySet.end();
    if (prop != iterEnd)
    {
        float opacity{};
        if (SVGStringParser::ParseNumber(prop->second.c_str(), opacity))
            graphicStyle.opacity = std::max<float>(0.0, std::min<float>(1.0, opacity));
    }

    prop = propertySet.find("clip-path");
    if (prop != iterEnd)
    {
        // FIXME: Use proper parser.
        auto urlLength = strlen("url(#");
        std::string id = prop->second;
        id = id.substr(urlLength, id.size() - urlLength - 1);
        auto clippingPathIt = mClippingPaths.find(id);
        if (clippingPathIt != mClippingPaths.end())
            graphicStyle.clippingPath = clippingPathIt->second;
    }

    prop = propertySet.find("display");
    if (prop != iterEnd)
    {
        std::string displayString = prop->second;
        if (displayString.compare("none"))
            graphicStyle.display = false;
    }

    prop = propertySet.find("stop-opacity");
    if (prop != iterEnd)
    {
        float opacity{};
        if (SVGStringParser::ParseNumber(prop->second.c_str(), opacity))
            graphicStyle.stopOpacity = std::max<float>(0.0, std::min<float>(1.0, opacity));
    }

    prop = propertySet.find("stop-color");
    if (prop != iterEnd)
    {
        ColorImpl color = Color{{0.0f, 0.0f, 0.0f, 1.0f}};
        auto result = SVGStringParser::ParseColor(prop->second.c_str(), color, true);
        if (result == SVGDocumentImpl::Result::kSuccess)
            graphicStyle.stopColor = color;
    }
}

float SVGDocumentImpl::ParseColorStop(XMLNode* node, std::vector<ColorStopImpl>& colorStops, float lastOffset)
{
    auto fillStyle = mFillStyleStack.top();
    auto strokeStyle = mStrokeStyleStack.top();
    std::set<std::string> classNames;
    auto graphicStyle = ParseGraphic(node, fillStyle, strokeStyle, classNames);

    // * New stops may never appear before previous stops. Use offset of previous stop otherwise.
    // * Stops must be in the range [0.0, 1.0].
    auto attr = node->first_attribute("offset");
    float offset{};
    offset = (attr && SVGStringParser::ParseNumber(attr->value(), offset)) ? offset : lastOffset;
    offset = std::max<float>(lastOffset, offset);
    offset = std::min<float>(1.0, std::max<float>(0.0, offset));

    ColorImpl& paint = graphicStyle.stopColor;
    if (paint.type() == typeid(ColorKeys))
    {
        // Value is "currentColor". Simply set value to CSS color property.
        paint = fillStyle.color;
    }

    graphicStyle.stopOpacity = std::max<float>(0.0, std::min<float>(1.0, graphicStyle.stopOpacity));

    colorStops.push_back(std::make_tuple(offset, paint, graphicStyle.stopOpacity));
    return offset;
}

void SVGDocumentImpl::ParseColorStops(XMLNode* node, GradientImpl& gradient)
{
    // Return early if we don't have children.
    if (!node->first_node())
        return;
    float lastOffset{};

    std::vector<ColorStopImpl> colorStops;
    for (auto child = node->first_node(); child != nullptr; child = child->next_sibling())
    {
        if (std::string(child->name()) == "stop")
            lastOffset = ParseColorStop(child, colorStops, lastOffset);
    }
    // Make sure we always have stops in the range 0% and 100%.
    if (colorStops.size() > 1)
    {
        const auto& firstStop = colorStops.front();
        if (std::get<0>(firstStop) != 0.0f)
            colorStops.insert(colorStops.begin(), std::make_tuple(0.0f, std::get<1>(firstStop), std::get<2>(firstStop)));
        const auto& lastStop = colorStops.back();
        if (std::get<0>(lastStop) != 1.0f)
            colorStops.push_back(std::make_tuple(1.0f, std::get<1>(lastStop), std::get<2>(lastStop)));
    }
    // Keep the color stops from referenced gradients if the current gradient
    // has none.
    if (!colorStops.empty())
        gradient.internalColorStops = colorStops;
}

void SVGDocumentImpl::ParseGradient(XMLNode* node)
{
    GradientImpl gradient{};

    // SVG allows referencing other gradients. For now, we only look at already parsed
    // gradients. Since we add the current gradient after successful parsing,
    // this also avoids circular references.
    // https://www.w3.org/TR/SVG11/pservers.html#LinearGradientElementHrefAttribute
    auto attr = node->first_attribute("xlink:href");
    if (attr)
    {
        std::string href{attr->value()};
        // href starts with a #, ignore it.
        auto it = mGradients.find(href.substr(1));
        if (it != mGradients.end())
            gradient = it->second;
    }

    ParseColorStops(node, gradient);

    auto nodeName = std::string(node->name());
    if (nodeName == "linearGradient")
        gradient.type = GradientType::kLinearGradient;
    else if (nodeName == "radialGradient")
        gradient.type = GradientType::kRadialGradient;
    else
    {
        SVG_ASSERT_MSG(false, "Gradient parser called with invalid element");
        return;
    }

    // TODO: Do we want to support `gradientUnits="objectBoundnigBox"` at all?
    // This would require us to get the bounding box of the filled/stroked shape
    // when the gradient gets applied.

    if (gradient.type == GradientType::kLinearGradient)
    {
        // https://www.w3.org/TR/SVG11/pservers.html#LinearGradients
        if (HasAttr(node, "x1"))
            gradient.x1 = ParseLengthFromAttr(node, "x1", LengthType::kHorrizontal);
        if (HasAttr(node, "y1"))
            gradient.y1 = ParseLengthFromAttr(node, "y1", LengthType::kVertical);
        if (HasAttr(node, "x2"))
            gradient.x2 = ParseLengthFromAttr(node, "x2", LengthType::kHorrizontal);
        if (HasAttr(node, "y2"))
            gradient.y2 = ParseLengthFromAttr(node, "y2", LengthType::kVertical);
    }
    else
    {
        // https://www.w3.org/TR/SVG11/pservers.html#RadialGradients
        if (HasAttr(node, "cx"))
            gradient.cx = ParseLengthFromAttr(node, "cx", LengthType::kHorrizontal);
        if (HasAttr(node, "cy"))
            gradient.cy = ParseLengthFromAttr(node, "cy", LengthType::kVertical);
        if (HasAttr(node, "fx"))
            gradient.fx = ParseLengthFromAttr(node, "fx", LengthType::kHorrizontal);
        if (HasAttr(node, "fy"))
            gradient.fy = ParseLengthFromAttr(node, "fy", LengthType::kVertical);
        if (HasAttr(node, "r"))
            gradient.r = ParseLengthFromAttr(node, "r", LengthType::kDiagonal);
    }

    attr = node->first_attribute("spreadMethod");
    if (attr)
    {
        auto spreadMethodString = std::string(attr->value());
        if (spreadMethodString == "pad")
            gradient.method = SpreadMethod::kPad;
        else if (spreadMethodString == "reflect")
            gradient.method = SpreadMethod::kReflect;
        else if (spreadMethodString == "repeat")
            gradient.method = SpreadMethod::kRepeat;
    }
    attr = node->first_attribute("gradientTransform");
    if (attr)
    {
        auto transformHandler = [&]() {
            SVG_ASSERT(mRenderer != nullptr);
            return mRenderer->CreateTransform();
        };
        gradient.transform = SVGStringParser::ParseTransform(attr->value(), transformHandler);
    }

    attr = node->first_attribute("id");
    if (attr)
        mGradients.insert({attr->value(), gradient});
}

void SVGDocumentImpl::Render(const ColorMap& colorMap, float width, float height)
{
    SVG_ASSERT(mGroup);
    if (!mGroup)
        return;

    float scale = width / mViewBox[2];
    if (scale > height / mViewBox[3])
        scale = height / mViewBox[3];

    GraphicStyleImpl graphicStyle{};
    graphicStyle.transform = mRenderer->CreateTransform();
    graphicStyle.transform->Scale(scale, scale);

    mRenderer->Save(graphicStyle);

    TraverseTree(colorMap, mGroup.get());

    mRenderer->Restore();
}

void SVGDocumentImpl::AddChildToCurrentGroup(std::unique_ptr<Element> element)
{
    SVG_ASSERT(!mGroupStack.empty());
    if (mGroupStack.empty())
        return;

    mGroupStack.top()->children.push_back(std::move(element));
}

static void ResolveColorImpl(const ColorMap& colorMap, const ColorImpl& colorImpl, Color& color)
{
    if (colorImpl.type() == typeid(Variable))
    {
        const auto& var = boost::get<Variable>(colorImpl);
        auto colorIt = colorMap.find(var.first);
        color = colorIt != colorMap.end() ? colorIt->second : var.second;
    }
    else if (colorImpl.type() == typeid(Color))
        color = boost::get<Color>(colorImpl);
    else
        // Can only be reached if fallback color value of var() is currentColor.
        color = Color{{0.0f, 0.0f, 0.0f, 1.0f}};
}

static void ResolvePaintImpl(const ColorMap& colorMap, const PaintImpl& internalPaint, const Color& currentColor, Paint& paint)
{
    if (internalPaint.type() == typeid(Variable))
    {
        const auto& var = boost::get<Variable>(internalPaint);
        auto colorIt = colorMap.find(var.first);
        paint = colorIt != colorMap.end() ? colorIt->second : var.second;
    }
    else if (internalPaint.type() == typeid(GradientImpl))
    {
        // Stop colors may have variables as well.
        const auto& internalGradient = boost::get<GradientImpl>(internalPaint);
        paint = std::move(internalGradient);
        auto& gradient = boost::get<Gradient>(paint);
        for (auto& colorStop : internalGradient.internalColorStops)
        {
            Color stopColor{{0, 0, 0, 1.0}};
            const auto& colorImpl = std::get<1>(colorStop);
            if (colorImpl.type() == typeid(Variable))
            {
                const auto& var = boost::get<Variable>(colorImpl);
                auto colorIt = colorMap.find(var.first);
                stopColor = colorIt != colorMap.end() ? colorIt->second : var.second;
            }
            else if (colorImpl.type() == typeid(Color))
                stopColor = boost::get<Color>(colorImpl);
            else
            {
                SVG_ASSERT_MSG(false, "Unhandled ColorImpl type");
            }
            stopColor[3] *= std::get<2>(colorStop);
            gradient.colorStops.push_back({std::get<0>(colorStop), stopColor});
        }
    }
    else if (internalPaint.type() == typeid(Color))
        paint = boost::get<Color>(internalPaint);
    else if (internalPaint.type() == typeid(ColorKeys))
        // currentColor is the only possible enum value for now.
        paint = currentColor;
    else
        SVG_ASSERT_MSG(false, "Unhandled PaintImpl type");
}

void SVGDocumentImpl::TraverseTree(const ColorMap& colorMap, const Element* element)
{
    SVG_ASSERT(element);
    if (!element)
        return;

    // Inheritance doesn't work for override styles. Since override styles
    // are deprecated, we are not going to fix this nor is this expected by
    // (still existing) clients.
    auto graphicStyle = element->graphicStyle;
    FillStyleImpl fillStyle{};
    StrokeStyleImpl strokeStyle{};
    // Do not draw element if an applied clipPath has no content.
    if (graphicStyle.clippingPath && !graphicStyle.clippingPath->hasClipContent)
        return;
    switch (element->Type())
    {
    case ElementType::kGraphic:
    {
        const auto graphic = static_cast<const Graphic*>(element);
        // TODO: Since we keep the original fill, stroke and color property values
        // we should be able to do w/o a copy.
        fillStyle = graphic->fillStyle;
        strokeStyle = graphic->strokeStyle;
        ApplyCSSStyle(graphic->classNames, graphicStyle, fillStyle, strokeStyle);
        // If we habe a CSS var() function we need to replace the placeholder with
        // an actual color from our externally provided color map here.
        Color color{{0.0f, 0.0f, 0.0f, 1.0f}};
        ResolveColorImpl(colorMap, fillStyle.color, color);
        ResolvePaintImpl(colorMap, fillStyle.internalPaint, color, fillStyle.paint);
        ResolvePaintImpl(colorMap, strokeStyle.internalPaint, color, strokeStyle.paint);
        mRenderer->DrawPath(*(graphic->path.get()), graphicStyle, fillStyle, strokeStyle);
        break;
    }
    case ElementType::kImage:
    {
        const auto image = static_cast<const Image*>(element);
        ApplyCSSStyle(image->classNames, graphicStyle, fillStyle, strokeStyle);
        mRenderer->DrawImage(*(image->imageData.get()), graphicStyle, image->clipArea, image->fillArea);
        break;
    }
    case ElementType::kGroup:
    {
        const auto group = static_cast<const Group*>(element);
        ApplyCSSStyle(group->classNames, graphicStyle, fillStyle, strokeStyle);
        mRenderer->Save(group->graphicStyle);
        for (const auto& child : group->children)
            TraverseTree(colorMap, child.get());
        mRenderer->Restore();
        break;
    }
    default:
        SVG_ASSERT_MSG(false, "Unknown element type");
    }
}

#ifndef STYLE_SUPPORT
// Deprecated style support
void SVGDocumentImpl::ApplyCSSStyle(
    const std::set<std::string>&, GraphicStyleImpl&, FillStyleImpl&, StrokeStyleImpl&) {}
void SVGDocumentImpl::ParseStyleAttr(XMLNode*, std::vector<PropertySet>&, std::set<std::string>&) {}
void SVGDocumentImpl::ParseStyle(XMLNode*) {}
#endif

} // namespace SVGNative
