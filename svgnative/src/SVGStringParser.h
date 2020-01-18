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

#pragma once

#include "SVGDocumentImpl.h"
#include <map>

namespace SVGNative
{
namespace SVGStringParser
{
bool ParseTransform(const SVG_STRING& transformString, Transform& matrix);
bool ParseNumber(const SVG_STRING& numberString, float& number);
bool ParseListOfNumbers(const SVG_STRING& numberListString, std::vector<float>& numberList, bool isAllOptional = true);
bool ParseListOfLengthOrPercentage(
    const SVG_STRING& lengthOrPercentageListString, float relDimensionLength, std::vector<float>& numberList, bool isAllOptional = true);
bool ParseListOfStrings(const SVG_STRING& stringListString, std::vector<SVG_STRING>& stringList);
bool ParseLengthOrPercentage(const SVG_STRING& lengthString, float relDimensionLength, float& absLengthInUnits, bool useQuirks = false);
void ParsePathString(const SVG_STRING& pathString, Path& p);
SVGDocumentImpl::Result ParseColor(const SVG_STRING& colorString, ColorImpl& paint, bool supportsCurrentColor = true);
SVGDocumentImpl::Result ParsePaint(const SVG_STRING& colorString, const std::map<SVG_STRING, GradientImpl>& gradientMap,
    const std::array<float, 4>& viewBox, PaintImpl& paint);

} // namespace SVGStringParser

} // namespace SVGNative
