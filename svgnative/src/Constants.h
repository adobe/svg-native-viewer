/*
Copyright 2020 Adobe. All rights reserved.
This file is licensed to you under the Apache License, Version 2.0 (the "License")
you may not use this file except in compliance with the License. You may obtain a copy
of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under
the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
OF ANY KIND, either express or implied. See the License for the specific language
governing permissions and limitations under the License.
*/

#pragma once

namespace SVGNative
{
#ifdef MSXML
#include <cwchar>
#define SVG_CHAR wchar_t
#define SVG_C_CHAR const wchar_t*
#define SVG_STRING std::wstring
#define SVG_TO_C_CHAR(string) L#string
#define SVG_CONSTEXPR_C_CHAR(attrKey, attrValue) \
constexpr const wchar_t* attrKey { L#attrValue };
#else
#define SVG_CHAR char
#define SVG_C_CHAR const char*
#define SVG_STRING std::string
#define SVG_TO_C_CHAR(string) #string
#define SVG_CONSTEXPR_C_CHAR(attrKey, attrValue) \
constexpr const char* attrKey { #attrValue };
#endif

// Attributes
SVG_CONSTEXPR_C_CHAR(kIdAttr, id)
SVG_CONSTEXPR_C_CHAR(kXAttr, x)
SVG_CONSTEXPR_C_CHAR(kYAttr, y)
SVG_CONSTEXPR_C_CHAR(kWidthAttr, width)
SVG_CONSTEXPR_C_CHAR(kHeightAttr, height)
SVG_CONSTEXPR_C_CHAR(kRxAttr, rx)
SVG_CONSTEXPR_C_CHAR(kRyAttr, ry)
SVG_CONSTEXPR_C_CHAR(kRAttr, r)
SVG_CONSTEXPR_C_CHAR(kDAttr, d)
SVG_CONSTEXPR_C_CHAR(kCxAttr, cx)
SVG_CONSTEXPR_C_CHAR(kCyAttr, cy)
SVG_CONSTEXPR_C_CHAR(kFxAttr, fx)
SVG_CONSTEXPR_C_CHAR(kFyAttr, fy)
SVG_CONSTEXPR_C_CHAR(kX1Attr, x1)
SVG_CONSTEXPR_C_CHAR(kY1Attr, y1)
SVG_CONSTEXPR_C_CHAR(kX2Attr, x2)
SVG_CONSTEXPR_C_CHAR(kY2Attr, y2)
SVG_CONSTEXPR_C_CHAR(kPointsAttr, points)
SVG_CONSTEXPR_C_CHAR(kHrefAttr, href)
SVG_CONSTEXPR_C_CHAR(kTransformAttr, transform)
SVG_CONSTEXPR_C_CHAR(kGradientTransformAttr, gradientTransform)
SVG_CONSTEXPR_C_CHAR(kViewBoxAttr, viewBox)
SVG_CONSTEXPR_C_CHAR(kSpreadMethodAttr, spreadMethod)
SVG_CONSTEXPR_C_CHAR(kOffsetAttr, offset)
#if DEBUG
SVG_CONSTEXPR_C_CHAR(kDataNameAttr, data-name)
#endif
#ifdef STYLE_SUPPORT
SVG_CONSTEXPR_C_CHAR(kClassAttr, class)
SVG_CONSTEXPR_C_CHAR(kStyleAttr, style)
#endif

// Properties
SVG_CONSTEXPR_C_CHAR(kColorProp, color)
SVG_CONSTEXPR_C_CHAR(kClipRuleProp, clip-rule)
SVG_CONSTEXPR_C_CHAR(kFillProp, fill)
SVG_CONSTEXPR_C_CHAR(kFillRuleProp, fill-rule)
SVG_CONSTEXPR_C_CHAR(kFillOpacityProp, fill-opacity)
SVG_CONSTEXPR_C_CHAR(kStrokeProp, stroke)
SVG_CONSTEXPR_C_CHAR(kStrokeDasharrayProp, stroke-dasharray)
SVG_CONSTEXPR_C_CHAR(kStrokeDashoffsetProp, stroke-dashoffset)
SVG_CONSTEXPR_C_CHAR(kStrokeLinecapProp, stroke-linecap)
SVG_CONSTEXPR_C_CHAR(kStrokeLinejoinProp, stroke-linejoin)
SVG_CONSTEXPR_C_CHAR(kStrokeMiterlimitProp, stroke-miterlimit)
SVG_CONSTEXPR_C_CHAR(kStrokeOpacityProp, stroke-opacity)
SVG_CONSTEXPR_C_CHAR(kStrokeWidthProp, stroke-width)
SVG_CONSTEXPR_C_CHAR(kVisibilityProp, visibility)
SVG_CONSTEXPR_C_CHAR(kClipPathProp, clip-path)
SVG_CONSTEXPR_C_CHAR(kDisplayProp, display)
SVG_CONSTEXPR_C_CHAR(kOpacityProp, opacity)
SVG_CONSTEXPR_C_CHAR(kStopOpacityProp, stop-opacity)
SVG_CONSTEXPR_C_CHAR(kStopColorProp, stop-color)
SVG_CONSTEXPR_C_CHAR(kPreserveAspectRatioAttr, preserveAspectRatio)

// Elements
SVG_CONSTEXPR_C_CHAR(kLineElem, line)
SVG_CONSTEXPR_C_CHAR(kRectElem, rect)
SVG_CONSTEXPR_C_CHAR(kPathElem, path)
SVG_CONSTEXPR_C_CHAR(kPolygonElem, polygon)
SVG_CONSTEXPR_C_CHAR(kPolylineElem, polyline)
SVG_CONSTEXPR_C_CHAR(kEllipseElem, ellipse)
SVG_CONSTEXPR_C_CHAR(kCircleElem, circle)
SVG_CONSTEXPR_C_CHAR(kGElem, g)
SVG_CONSTEXPR_C_CHAR(kClipPathElem, clipPath)
SVG_CONSTEXPR_C_CHAR(kSymbolElem, symbol)
SVG_CONSTEXPR_C_CHAR(kStyleElem, style)
SVG_CONSTEXPR_C_CHAR(kLinearGradientElem, linearGradient)
SVG_CONSTEXPR_C_CHAR(kRadialGradientElem, radialGradient)
SVG_CONSTEXPR_C_CHAR(kDefsElem, defs)
SVG_CONSTEXPR_C_CHAR(kUseElem, use)
SVG_CONSTEXPR_C_CHAR(kImageElem, image)
SVG_CONSTEXPR_C_CHAR(kStopElem, stop)
SVG_CONSTEXPR_C_CHAR(kSvgElem, svg)

// Values
SVG_CONSTEXPR_C_CHAR(kSliceVal, slice)
SVG_CONSTEXPR_C_CHAR(kXMinYMinVal, xMinYMin)
SVG_CONSTEXPR_C_CHAR(kXMidYMinVal, xMidYMin)
SVG_CONSTEXPR_C_CHAR(kXMaxYMinVal, xMaxYMin)
SVG_CONSTEXPR_C_CHAR(kXMinYMidVal, xMinYMid)
SVG_CONSTEXPR_C_CHAR(kXMaxYMidVal, xMaxYMid)
SVG_CONSTEXPR_C_CHAR(kXMinYMaxVal, xMinYMax)
SVG_CONSTEXPR_C_CHAR(kXMidYMaxVal, xMidYMax)
SVG_CONSTEXPR_C_CHAR(kXMaxYMaxVal, xMaxYMax)
SVG_CONSTEXPR_C_CHAR(kEvenoddVal, evenodd)
SVG_CONSTEXPR_C_CHAR(kNonzeroVal, nonzero)
SVG_CONSTEXPR_C_CHAR(kHiddenVal, hidden)
SVG_CONSTEXPR_C_CHAR(kCollapseVal, collapse)
SVG_CONSTEXPR_C_CHAR(kVisibleVal, visible)
SVG_CONSTEXPR_C_CHAR(kRoundVal, round)
SVG_CONSTEXPR_C_CHAR(kSquareVal, square)
SVG_CONSTEXPR_C_CHAR(kBevelVal, bevel)
SVG_CONSTEXPR_C_CHAR(kNoneVal, none)
SVG_CONSTEXPR_C_CHAR(kPadVal, pad)
SVG_CONSTEXPR_C_CHAR(kReflectVal, reflect)
SVG_CONSTEXPR_C_CHAR(kRepeatVal, repeat)
SVG_CONSTEXPR_C_CHAR(kTranslateVal, translate)
SVG_CONSTEXPR_C_CHAR(kRotateVal, rotate)
SVG_CONSTEXPR_C_CHAR(kScaleVal, scale)
SVG_CONSTEXPR_C_CHAR(kMatrixVal, matrix)
SVG_CONSTEXPR_C_CHAR(kSkewXVal, skewX)
SVG_CONSTEXPR_C_CHAR(kSkewYVal, skewY)
SVG_CONSTEXPR_C_CHAR(kCurrentcolorVal, currentcolor)
#ifdef MSXML
constexpr const wchar_t* kUrlVal { L"url(#" };
constexpr const wchar_t* kVarVal { L"var(" };
constexpr const wchar_t* kRgbVal { L"rgb(" };
constexpr const wchar_t* kDataUrlPngVal { L"data:image/png;base64," };
constexpr const wchar_t* kDataUrlJpgVal { L"data:image/jpg;base64," };
constexpr const wchar_t* kDataUrlJpegVal { L"data:image/jpeg;base64," };
#else
constexpr const char* kUrlVal { "url(#" };
constexpr const char* kVarVal { "var(" };
constexpr const char* kRgbVal { "rgb(" };
constexpr const char* kDataUrlPngVal { "data:image/png;base64," };
constexpr const char* kDataUrlJpgVal { "data:image/jpg;base64," };
constexpr const char* kDataUrlJpegVal { "data:image/jpeg;base64," };
#endif

// Units
SVG_CONSTEXPR_C_CHAR(kCmVal, cm)
SVG_CONSTEXPR_C_CHAR(kMmVal, mm)
SVG_CONSTEXPR_C_CHAR(kInVal, in)
SVG_CONSTEXPR_C_CHAR(kPcVal, pc)
SVG_CONSTEXPR_C_CHAR(kPtVal, pt)
SVG_CONSTEXPR_C_CHAR(kPxVal, px)

// Others
SVG_CONSTEXPR_C_CHAR(kXlinkNS, xlink)

} // namespace SVGNative
