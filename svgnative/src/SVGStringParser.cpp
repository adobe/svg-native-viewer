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

#include "SVGStringParser.h"
#include "CSSColorKeywords.h"
#include "SVGDocument.h"
#include <algorithm>
#include <array>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

namespace SVGNative
{
void ArcToCurve(Path& path, float startX, float startY, float radiusX, float radiusY, float angle, bool large, bool sweep, float endX,
    float endY, float& endControlX, float& endControlY);

namespace SVGStringParser
{
using CharIt = std::string::const_iterator;

inline bool isDigit(char c)
{
    return c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9';
}

inline bool isHex(char c)
{
    return isDigit(c) || c == 'a' || c == 'A' || c == 'b' || c == 'B' || c == 'c' || c == 'C' || c == 'd' || c == 'D' || c == 'e'
        || c == 'E' || c == 'f' || c == 'F';
}

inline bool isWsp(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

inline bool SkipOptWspOrDelimiter(CharIt& pos, const CharIt& end, bool isAllOptional = true, char delimiter = ',')
{
    if (!isAllOptional && !isWsp(*pos) && *pos != delimiter)
        return false;

    while (pos != end && isWsp(*pos))
        pos++;
    if (*pos == delimiter)
        pos++;
    while (pos != end && isWsp(*pos))
        pos++;
    return pos != end;
}

inline bool SkipOptWspDelimiterOptWsp(CharIt& pos, const CharIt& end, char delimiter = ',')
{
    bool hasDelimiter{};
    while (pos != end && isWsp(*pos))
        pos++;
    if (*pos == delimiter)
    {
        hasDelimiter = true;
        pos++;
    }
    while (pos != end && isWsp(*pos))
        pos++;
    return pos != end && hasDelimiter;
}

inline bool SkipOptWsp(CharIt& pos, const CharIt& end)
{
    while (pos != end && isWsp(*pos))
        ++pos;
    return pos != end;
}

inline bool ParseDigit(CharIt& pos, const CharIt& end, std::int32_t& digit)
{
    if (pos == end || !isDigit(*pos))
        return false;
    while (pos != end && isDigit(*pos))
    {
        digit *= 10;
        digit += static_cast<std::int32_t>(*pos++ - '0');
    }
    return true;
}

static bool ParseScientificNumber(CharIt& pos, const CharIt& end, float& number)
{
    if (pos == end)
        return false;

    number = 0;

    float sign{1};
    float exponent{0};
    float exponentSign{1};

    if (*pos == '-' || *pos == '+')
    {
        if (*pos == '-')
            sign = -1;
        pos++;
        if (pos == end)
            return false;
    }

    bool hasNumber{};
    bool hasFraction{};
    while (pos != end && isDigit(*pos))
    {
        hasNumber = true;
        number *= 10;
        number += static_cast<float>(*pos++ - '0');
    }
    if (pos == end)
    {
        number *= sign;
        return std::isfinite(number);
    }

    if (*pos == '.')
    {
        pos++;
        if (pos == end || !isDigit(*pos))
            return false;

        float division = 10;
        while (pos != end && isDigit(*pos))
        {
            hasFraction = true;
            number += static_cast<float>(*pos++ - '0') / division;
            division *= 10;
        }
    }
    if (!hasFraction && !hasNumber)
        return false;

    if (pos == end || (*pos != 'e' && *pos != 'E'))
    {
        number *= sign;
        return std::isfinite(number);
    }

    pos++;
    if (pos == end)
        return false;

    if (*pos == '-' || *pos == '+')
    {
        if (*pos == '-')
            exponentSign = -1;
        pos++;
        if (pos == end)
            return false;
    }
    if (!isDigit(*pos))
        return false;

    while (pos != end && isDigit(*pos))
    {
        exponent *= 10.0f;
        exponent += static_cast<float>(*pos++ - '0');
    }
    if (exponent)
        number *= pow(10.0f, exponentSign * exponent);

    number *= sign;
    return std::isfinite(number);
}

static bool ParseCoordinate(CharIt& pos, const CharIt& end, float& coord)
{
    // FIXME: Remove initial SkipOptWspOrDelimiter call. Keep it here to keep
    // behavior consitent to previous implementation.
    if (!SkipOptWspOrDelimiter(pos, end))
        return false;
    if (!ParseScientificNumber(pos, end, coord))
        return false;
    return true;
}

static bool ParseCoordinatePair(CharIt& pos, const CharIt& end, float& x, float& y)
{
    // FIXME: Remove initial SkipOptWspOrDelimiter call. Keep it here to keep
    // behavior consitent to previous implementation.
    if (!SkipOptWspOrDelimiter(pos, end))
        return false;
    if (!ParseScientificNumber(pos, end, x))
        return false;
    if (!SkipOptWspOrDelimiter(pos, end))
        return false;
    if (!ParseScientificNumber(pos, end, y))
        return false;
    return true;
}

static bool ParseBool(CharIt& pos, const CharIt& end, bool& boolVal)
{
    if (!SkipOptWspOrDelimiter(pos, end))
        return false;
    if (*pos == '0')
        boolVal = false;
    else if (*pos == '1')
        boolVal = true;
    else
        return false;
    pos++;
    return true;
}

static bool ParseLengthOrPercentage(
    CharIt& pos, const CharIt& end, float relDimensionLength, float& absLengthInUnits, bool useQuirks = false)
{
    if (!ParseScientificNumber(pos, end, absLengthInUnits))
        return false;

    if (pos == end)
    {
        if (useQuirks)
            return std::isfinite(absLengthInUnits);
        return false;
    }

    if (*pos != '%' && *pos != 'c' && *pos != 'm' && *pos != 'i' && *pos != 'p')
    {
        if (useQuirks)
            return true;
        return false;
    }

    auto start = pos++;
    if (*start == '%')
    {
        absLengthInUnits /= 100;
        absLengthInUnits *= relDimensionLength;
        return true;
    }

    if (pos == end)
        return false;

    // https://www.w3.org/TR/css-values-3/#absolute-lengths
    auto unit = std::string(start, start + 2);
    std::transform(unit.begin(), unit.end(), unit.begin(), ::tolower);
    if (unit.compare("cm") == 0)
        absLengthInUnits *= (96.0f / 2.54f);
    else if (unit.compare("mm") == 0)
        absLengthInUnits *= (9.6f / 2.54f);
    else if (unit.compare("in") == 0)
        absLengthInUnits *= 96.0f;
    else if (unit.compare("pc") == 0)
        absLengthInUnits *= (96.0f / 6.0f);
    else if (unit.compare("pt") == 0)
        absLengthInUnits *= (96.0f / 72.0f);
    else if (unit.compare("px") == 0)
        absLengthInUnits *= 1.0f;
    else
        return false;
    pos++;

    return true;
}

static void ParseListOfNumbers(CharIt& pos, const CharIt& end, std::vector<float>& numberList, bool isAllOptional = true)
{
    numberList.clear();

    float number{};
    if (!SkipOptWsp(pos, end))
        return;
    auto temp = pos;
    if (!ParseScientificNumber(temp, end, number))
        return;
    pos = temp;
    numberList.push_back(number);
    while (pos != end)
    {
        temp = pos;
        if (!SkipOptWspOrDelimiter(temp, end, isAllOptional))
            return;

        if (!ParseScientificNumber(temp, end, number))
            return;

        numberList.push_back(number);
        pos = temp;
    }
}

bool ParseLengthOrPercentage(const std::string& lengthString, float relDimensionLength, float& absLengthInUnits, bool useQuirks /*= false*/)
{
    auto pos = lengthString.begin();
    auto end = lengthString.end();
    SkipOptWsp(pos, end);
    if (!ParseLengthOrPercentage(pos, end, relDimensionLength, absLengthInUnits, useQuirks))
        return false;

    return !SkipOptWsp(pos, end);
}

bool ParseNumber(const std::string& numberString, float& number)
{
    auto pos = numberString.begin();
    auto end = numberString.end();

    if (!SkipOptWsp(pos, end))
        return false;
    if (!ParseScientificNumber(pos, end, number))
        return false;
    return !SkipOptWsp(pos, end);
}

bool ParseListOfNumbers(const std::string& numberListString, std::vector<float>& numberList, bool isAllOptional /*= true*/)
{
    auto pos = numberListString.begin();
    auto end = numberListString.end();

    if (!SkipOptWsp(pos, end))
        return true;

    ParseListOfNumbers(pos, end, numberList, isAllOptional);

    return !SkipOptWsp(pos, end);
}

bool ParseListOfLengthOrPercentage(const std::string& lengthOrPercentageListString, float relDimensionLength,
    std::vector<float>& numberList, bool isAllOptional /*= true*/)
{
    auto pos = lengthOrPercentageListString.begin();
    auto end = lengthOrPercentageListString.end();

    numberList.clear();

    float number{};
    if (!SkipOptWsp(pos, end))
        return false;
    auto temp = pos;
    if (!ParseLengthOrPercentage(temp, end, relDimensionLength, number, isAllOptional))
        return false;
    pos = temp;
    numberList.push_back(number);
    while (pos != end)
    {
        temp = pos;
        if (!SkipOptWspOrDelimiter(temp, end, isAllOptional))
            return false;

        if (!ParseLengthOrPercentage(temp, end, relDimensionLength, number, isAllOptional))
            return false;

        numberList.push_back(number);
        pos = temp;
    }
    return !SkipOptWsp(pos, end);
}

bool ParseListOfStrings(const std::string& stringListString, std::vector<std::string>& stringList)
{
    auto pos = stringListString.begin();
    auto end = stringListString.end();

    if (!SkipOptWsp(pos, end))
        return false;
    while (pos != end)
    {
        auto start = pos;
        while (pos != end && !isWsp(*pos))
            pos++;
        stringList.push_back({start, pos});
        SkipOptWsp(pos, end);
    }
    return true;
}

void ParsePathString(const std::string& pathString, Path& p)
{
    auto pos = pathString.begin();
    auto end = pathString.end();
    if (!SkipOptWsp(pos, end))
        return;

    bool startSet{false};
    float startX{};
    float startY{};
    float currentX{};
    float currentY{};
    float prevControlX{}; // nanf(nullptr);
    float prevControlY{}; // nanf(nullptr);
    float prevCurvePointX{};
    float prevCurvePointY{};
    char prev = 'm';

    while (pos != end)
    {
        if (!SkipOptWsp(pos, end))
            return;
        if (!isDigit(*pos) && *pos != ',' && *pos != '-' && *pos != '.')
            prev = *pos++;
        switch (prev)
        {
        case 'M':
            if (!ParseCoordinatePair(pos, end, currentX, currentY))
                return;
            p.MoveTo(currentX, currentY);
            prevControlX = currentX;
            prevControlY = currentY;
            prev = 'L'; // https://www.w3.org/TR/SVG/paths.html#PathDataMovetoCommands

            if (!startSet)
            {
                startSet = true;
                startX = currentX;
                startY = currentY;
            }

            break;
        case 'm':
        {
            float newX{};
            float newY{};
            if (!ParseCoordinatePair(pos, end, newX, newY))
                return;
            currentX += newX;
            currentY += newY;
            p.MoveTo(currentX, currentY);
            prevControlX = currentX;
            prevControlY = currentY;
            prev = 'l'; // https://www.w3.org/TR/SVG/paths.html#PathDataMovetoCommands

            if (!startSet)
            {
                startSet = true;
                startX = currentX;
                startY = currentY;
            }

            break;
        }
        case 'Z':
        case 'z':
            p.ClosePath();

            if (startSet)
            {
                startSet = false;
                currentX = startX;
                currentY = startY;
            }

            break;
        case 'L':
            if (!ParseCoordinatePair(pos, end, currentX, currentY))
                return;
            p.LineTo(currentX, currentY);
            prevControlX = currentX;
            prevControlY = currentY;
            break;
        case 'l':
        {
            float newX{};
            float newY{};
            if (!ParseCoordinatePair(pos, end, newX, newY))
                return;
            currentX += newX;
            currentY += newY;
            p.LineTo(currentX, currentY);
            prevControlX = currentX;
            prevControlY = currentY;
            break;
        }
        case 'V':
            if (!ParseCoordinate(pos, end, currentY))
                return;
            p.LineTo(currentX, currentY);
            prevControlX = currentX;
            prevControlY = currentY;
            break;
        case 'v':
        {
            float newY{};
            if (!ParseCoordinate(pos, end, newY))
                return;
            currentY += newY;
            p.LineTo(currentX, currentY);
            prevControlX = currentX;
            prevControlY = currentY;
            break;
        }
        case 'H':
            if (!ParseCoordinate(pos, end, currentX))
                return;
            p.LineTo(currentX, currentY);
            prevControlX = currentX;
            prevControlY = currentY;
            break;
        case 'h':
        {
            float newX{};
            if (!ParseCoordinate(pos, end, newX))
                return;
            currentX += newX;
            p.LineTo(currentX, currentY);
            prevControlX = currentX;
            prevControlY = currentY;
            break;
        }
        case 'C':
        {
            float fx1{};
            float fy1{};
            if (!ParseCoordinatePair(pos, end, fx1, fy1))
                return;
            if (!ParseCoordinatePair(pos, end, prevControlX, prevControlY))
                return;
            if (!ParseCoordinatePair(pos, end, currentX, currentY))
                return;
            p.CurveTo(fx1, fy1, prevControlX, prevControlY, currentX, currentY);
            break;
        }
        case 'c':
        {
            float fx1{};
            float fy1{};
            if (!ParseCoordinatePair(pos, end, fx1, fy1))
                return;
            fx1 += currentX;
            fy1 += currentY;
            if (!ParseCoordinatePair(pos, end, prevControlX, prevControlY))
                return;
            prevControlX += currentX;
            prevControlY += currentY;
            float newX{};
            float newY{};
            if (!ParseCoordinatePair(pos, end, newX, newY))
                return;
            currentX += newX;
            currentY += newY;
            p.CurveTo(fx1, fy1, prevControlX, prevControlY, currentX, currentY);

            break;
        }
        case 'S':
        {
            prevControlX = 2 * currentX - prevControlX;
            prevControlY = 2 * currentY - prevControlY;
            float tempX{};
            float tempY{};
            if (!ParseCoordinatePair(pos, end, tempX, tempY))
                return;
            if (!ParseCoordinatePair(pos, end, currentX, currentY))
                return;
            p.CurveTo(prevControlX, prevControlY, tempX, tempY, currentX, currentY);
            prevControlX = tempX;
            prevControlY = tempY;
            break;
        }
        case 's':
        {
            float fx1 = 2 * currentX - prevControlX;
            float fx2 = 2 * currentY - prevControlY;
            if (!ParseCoordinatePair(pos, end, prevControlX, prevControlY))
                return;
            prevControlX += currentX;
            prevControlY += currentY;
            float newX{};
            float newY{};
            if (!ParseCoordinatePair(pos, end, newX, newY))
                return;
            currentX += newX;
            currentY += newY;
            p.CurveTo(fx1, fx2, prevControlX, prevControlY, currentX, currentY);

            break;
        }
        case 'Q':
        {
            // https://www.w3.org/TR/SVG/paths.html#PathDataQuadraticBezierCommands

            float fx{};
            float fy{};
            if (!ParseCoordinatePair(pos, end, fx, fy))
                return;
            if (!ParseCoordinatePair(pos, end, currentX, currentY))
                return;
            p.CurveToV(fx, fy, currentX, currentY);

            prevCurvePointX = fx;
            prevCurvePointY = fy;

            break;
        }
        case 'q':
        {
            // https://www.w3.org/TR/SVG/paths.html#PathDataQuadraticBezierCommands

            float fx{};
            float fy{};
            if (!ParseCoordinatePair(pos, end, fx, fy))
                return;
            fx += currentX;
            fy += currentY;
            float newX{};
            float newY{};
            if (!ParseCoordinatePair(pos, end, newX, newY))
                return;
            currentX += newX;
            currentY += newY;
            p.CurveToV(fx, fy, currentX, currentY);

            prevCurvePointX = fx;
            prevCurvePointY = fy;

            break;
        }
        case 'T':
        {
            // https://www.w3.org/TR/SVG/paths.html#PathDataQuadraticBezierCommands

            float nextX{};
            float nextY{};
            if (!ParseCoordinatePair(pos, end, nextX, nextY))
                return;

            // reflect previous control point
            prevCurvePointX = currentX + (currentX - prevCurvePointX);
            prevCurvePointY = currentY + (currentY - prevCurvePointY);

            p.CurveToV(prevCurvePointX, prevCurvePointY, nextX, nextY);

            currentX = nextX;
            currentY = nextY;

            break;
        }
        case 't':
        {
            // https://www.w3.org/TR/SVG/paths.html#PathDataQuadraticBezierCommands

            float nextX{};
            float nextY{};
            if (!ParseCoordinatePair(pos, end, nextX, nextY))
                return;
            nextX += currentX;
            nextY += currentY;

            // reflect previous control point
            prevCurvePointX = currentX + (currentX - prevCurvePointX);
            prevCurvePointY = currentY + (currentY - prevCurvePointY);

            p.CurveToV(prevCurvePointX, prevCurvePointY, nextX, nextY);

            currentX = nextX;
            currentY = nextY;

            break;
        }
        case 'A':
        case 'a':
        {
            float rx{};
            float ry{};
            if (!ParseCoordinatePair(pos, end, rx, ry))
                return;

            float angle{};
            if (!ParseCoordinate(pos, end, angle))
                return;

            bool flagLarge{};
            bool flagSweep{};
            if (!ParseBool(pos, end, flagLarge))
                return;
            if (!ParseBool(pos, end, flagSweep))
                return;

            float startX = currentX;
            float startY = currentY;

            if (prev == 'A')
            {
                // absolute
                if (!ParseCoordinatePair(pos, end, currentX, currentY))
                    return;
            }
            else
            {
                // relative
                float newX{};
                float newY{};
                if (!ParseCoordinatePair(pos, end, newX, newY))
                    return;
                currentX += newX;
                currentY += newY;
            }

            SVG_ASSERT(angle == 0);

            ArcToCurve(p, startX, startY, rx, ry, angle, flagLarge, flagSweep, currentX, currentY, prevControlX, prevControlY);

            SVG_PARSE_TRACE("parsePathString ArcToCurve: controlPoint: " << prevControlX << "," << prevControlY);

            break;
        }
        default:
        {
            if (isalpha(prev))
                SVG_PARSE_TRACE("parsePathString UNHANDLED: " << prev);

            prev = *pos++;
            break;
        }
        }
    }
}

std::unique_ptr<Transform> ParseTransform(const std::string& transformString, std::function<std::unique_ptr<Transform>()> createTransform)
{
    // https://www.w3.org/TR/css-transforms-1/#svg-syntax
    auto pos = transformString.begin();
    auto end = transformString.end();
    if (!SkipOptWsp(pos, end))
        return nullptr;

    auto matrix = createTransform();
    bool isFirstTransform{true};
    while (pos != end)
    {
        if (!SkipOptWsp(pos, end))
            return matrix;
        if (!isFirstTransform && *pos == ',')
        {
            if (!SkipOptWspOrDelimiter(pos, end, false))
                return nullptr;
        }
        auto length = std::distance(pos, end);
        if (length >= 6 && std::string(pos, pos + 6).compare("matrix") == 0)
        {
            pos += 6;
            if (!SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != '(')
                return nullptr;
            std::vector<float> numberList;
            ParseListOfNumbers(pos, end, numberList);
            if (numberList.size() != 6 || !SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != ')')
                return nullptr;
            auto newMatrix = createTransform();
            newMatrix->Set(numberList[0], numberList[1], numberList[2], numberList[3], numberList[4], numberList[5]);
            matrix->Concat(*newMatrix);
        }
        else if (length >= 9 && std::string(pos, pos + 9).compare("translate") == 0)
        {
            pos += 9;
            if (!SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != '(')
                return nullptr;
            std::vector<float> numberList;
            ParseListOfNumbers(pos, end, numberList);
            auto size = numberList.size();
            if ((size != 1 && size != 2) || !SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != ')')
                return nullptr;
            matrix->Translate(numberList[0], (size == 1 ? 0 : numberList[1]));
        }
        else if (length >= 5 && std::string(pos, pos + 5).compare("scale") == 0)
        {
            pos += 5;
            if (!SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != '(')
                return nullptr;
            std::vector<float> numberList;
            ParseListOfNumbers(pos, end, numberList);
            auto size = numberList.size();
            if ((size != 1 && size != 2) || !SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != ')')
                return nullptr;
            matrix->Scale(numberList[0], (size == 1 ? numberList[0] : numberList[1]));
        }
        else if (length >= 6 && std::string(pos, pos + 6).compare("rotate") == 0)
        {
            pos += 6;
            if (!SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != '(')
                return nullptr;
            std::vector<float> numberList;
            ParseListOfNumbers(pos, end, numberList);
            auto size = numberList.size();
            if ((size != 1 && size != 3) || !SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != ')')
                return nullptr;
            if (size == 3)
            {
                matrix->Translate(numberList[1], numberList[2]);
                matrix->Rotate(numberList[0]);
                matrix->Translate(-numberList[1], -numberList[2]);
            }
            else
                matrix->Rotate(numberList[0]);
        }
        else if (length >= 5 && std::string(pos, pos + 5).compare("skewX") == 0)
        {
            pos += 5;
            float number{};
            if (!SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != '(')
                return nullptr;
            if (!SkipOptWsp(pos, end))
                return nullptr;
            if (!ParseScientificNumber(pos, end, number))
                return nullptr;
            if (!SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != ')')
                return nullptr;
            auto newMatrix = createTransform();
            number *= M_PI / 180.0f;
            newMatrix->Set(1.0f, 0.0f, tan(number), 1.0f, 0.0f, 0.0f);
            matrix->Concat(*newMatrix);
        }
        else if (length >= 5 && std::string(pos, pos + 5).compare("skewY") == 0)
        {
            pos += 5;
            float number{};
            if (!SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != '(')
                return nullptr;
            if (!SkipOptWsp(pos, end))
                return nullptr;
            if (!ParseScientificNumber(pos, end, number))
                return nullptr;
            if (!SkipOptWsp(pos, end))
                return nullptr;
            if (*pos++ != ')')
                return nullptr;
            auto newMatrix = createTransform();
            number *= M_PI / 180.0f;
            newMatrix->Set(1.0f, tan(number), 0.0f, 1.0f, 0.0f, 0.0f);
            matrix->Concat(*newMatrix);
        }
        else
            return nullptr;
        isFirstTransform = false;
    }
    return matrix;
}

static bool ParseCustomPropertyName(CharIt& pos, const CharIt& end, std::string& customPropertyName)
{
    if (pos == end || *pos++ != '-')
        return false;
    if (pos == end || *pos++ != '-')
        return false;
    if (pos == end)
        return false;
    auto startPos = pos;
    // TODO: Relax requirements and allow non-ASCII and escaped characters as well. Requires UTF8 support.
    while (isDigit(*pos) || (*pos >= 'A' && *pos <= 'Z') || (*pos >= 'a' && *pos <= 'z') || *pos == '_' || *pos == '-')
    {
        if (++pos == end)
            return false;
    }
    customPropertyName = std::string(startPos, pos);
    return !customPropertyName.empty();
}

static bool ParseColor(CharIt& pos, const CharIt& end, ColorImpl& paint, bool supportsCurrentColor, SVGDocumentImpl::Result& result)
{
    if (!SkipOptWsp(pos, end))
        return false;

    Color color{{0.0f, 0.0f, 0.0f, 1.0f}};
    if (*pos == '#')
    {
        auto start = ++pos;
        while (pos != end && isHex(*pos))
            pos++;
        std::string hexString(start, pos);
        auto num = stoi(hexString, nullptr, 16);
        if (hexString.size() == 3)
        {
            // Hex color with 3 characters: #FF0 -> #FFFF00
            color[0] = (num / 0x100) / 15.0f;
            color[1] = ((num / 0x10) % 0x10) / 15.0f;
            color[2] = (num % 0x10) / 15.0f;
        }
        else if (hexString.size() == 6)
        {
            // Hex color with 6 characters: #FFFF00
            color[0] = (num / 0x10000) / 255.0f;
            color[1] = ((num / 0x100) % 0x100) / 255.0f;
            color[2] = (num % 0x100) / 255.0f;
        }
        else
        {
            result = SVGDocumentImpl::Result::kInvalid;
            return false;
        }
        paint = color;
        result = SVGDocumentImpl::Result::kSuccess;
        return true;
    }

    auto length = std::distance(pos, end);
    if (length >= 4)
    {
        std::string char4string(pos, pos + 4);
        std::transform(char4string.begin(), char4string.end(), char4string.begin(), ::tolower);
        if (char4string.compare("rgb(") == 0)
        {
            result = SVGDocumentImpl::Result::kInvalid;
            pos += 4;
            std::int32_t r{};
            std::int32_t g{};
            std::int32_t b{};
            if (!SkipOptWsp(pos, end))
                return false;
            if (!ParseDigit(pos, end, r))
                return false;
            if (!SkipOptWspDelimiterOptWsp(pos, end))
                return false;
            if (!ParseDigit(pos, end, g))
                return false;
            if (!SkipOptWspDelimiterOptWsp(pos, end))
                return false;
            if (!ParseDigit(pos, end, b))
                return false;
            if (!SkipOptWsp(pos, end))
                return false;
            if (*pos++ != ')')
                return false;
            color[0] = std::min(255, r) / 255.0f;
            color[1] = std::min(255, g) / 255.0f;
            color[2] = std::min(255, b) / 255.0f;
            paint = color;
            result = SVGDocumentImpl::Result::kSuccess;
            return true;
        }
        if (char4string.compare("var(") == 0)
        {
            result = SVGDocumentImpl::Result::kInvalid;
            pos += 4;
            if (!SkipOptWsp(pos, end))
                return false;
            std::string customPropertyName;
            if (!ParseCustomPropertyName(pos, end, customPropertyName))
                return false;
            ColorImpl fallbackPaint = Color{{0.0f, 0.0f, 0.0f, 1.0f}};
            if (!SkipOptWsp(pos, end))
                return false;
            if (*pos != ')')
            {
                if (!SkipOptWspDelimiterOptWsp(pos, end))
                    return false;
                if (!ParseColor(pos, end, fallbackPaint, true, result) || result != SVGDocumentImpl::Result::kSuccess)
                    return false;
                if (!SkipOptWsp(pos, end))
                    return false;
                if (*pos != ')')
                    return false;
            }
            pos++;
            if (fallbackPaint.type() != typeid(Color))
            {
                // Fallback color was a variable. This is allowed according to
                // CSS syntax but requires recursive C++ variables. Ignoring it
                // and setting fallback color to black is a simpler approach for now.
                fallbackPaint = Color{{0.0f, 0.0f, 0.0f, 1.0f}};
            }
            paint = Variable{customPropertyName, boost::get<Color>(fallbackPaint)};
            result = SVGDocumentImpl::Result::kSuccess;
            return true;
        }
    }

    // Parse CSS named Colors.
    for (const auto& namedColor : gCSSNamedColors)
    {
        auto namedColorSize = std::get<1>(namedColor);
        if (std::distance(pos, end) < static_cast<const long>(namedColorSize))
            continue;
        std::string nameString(pos, pos + namedColorSize);
        std::transform(nameString.begin(), nameString.end(), nameString.begin(), ::tolower);
        if (std::string(nameString).compare(std::get<0>(namedColor)) == 0)
        {
            color = std::get<2>(namedColor);
            paint = color;
            result = SVGDocumentImpl::Result::kSuccess;
            pos += namedColorSize;
            return true;
        }
    }

    if (length >= 12 && supportsCurrentColor)
    {
        std::string currentColorString(pos, pos + 12);
        std::transform(currentColorString.begin(), currentColorString.end(), currentColorString.begin(), ::tolower);
        if (currentColorString.compare("currentcolor") == 0)
        {
            paint = ColorKeys::kCurrentColor;
            result = SVGDocumentImpl::Result::kSuccess;
            pos += 12;
            return true;
        }
    }

    result = SVGDocumentImpl::Result::kInvalid;
    return false;
}

SVGDocumentImpl::Result ParseColor(const std::string& colorString, ColorImpl& paint, bool supportsCurrentColor /*= true*/)
{
    auto pos = colorString.begin();
    auto end = colorString.end();
    SVGDocumentImpl::Result result{SVGDocumentImpl::Result::kInvalid};
    if (ParseColor(pos, end, paint, supportsCurrentColor, result))
        return result;
    return SVGDocumentImpl::Result::kInvalid;
}

SVGDocumentImpl::Result ParsePaint(const std::string& colorString, const std::map<std::string, GradientImpl>& gradientMap,
    const std::array<float, 4>& viewBox, PaintImpl& paint)
{
    SVGDocumentImpl::Result result{SVGDocumentImpl::Result::kInvalid};
    if (!colorString.size())
        return result;

    auto pos = colorString.begin();
    auto end = colorString.end();
    if (!SkipOptWsp(pos, end))
        return result;

    SVGDocumentImpl::Result urlResult{SVGDocumentImpl::Result::kInvalid};
    if (std::distance(pos, end) >= 5)
    {
        std::string urlString(pos, pos + 5);
        if (urlString.find("url(#") == 0)
        {
            // FIXME: Add proper parser for url()
            auto position = colorString.find(')');
            if (position == std::string::npos)
                return SVGDocumentImpl::Result::kInvalid;
            auto id = colorString.substr(5, position - 5);
            auto it = gradientMap.find(id);
            if (it != gradientMap.end())
            {
                // * No color stops means the same as if 'none' was specified.
                // * 1 color stop means solid color fill.
                // https://www.w3.org/TR/SVG11/pservers.html#GradientStops (see notes at the end)
                // Can not be determined earlier.
                auto gradient = it->second;
                if (gradient.internalColorStops.empty())
                    return SVGDocumentImpl::Result::kDisabled;
                else if (gradient.internalColorStops.size() == 1)
                    paint = std::get<1>(gradient.internalColorStops.front());
                else
                {
                    // Percentage values that do neither correlate to horrizontal nor vertical dimensions
                    // need to be relative to the hypotenuse of both. Example: r="50%"
                    float sqr = sqrtf(viewBox[2] * viewBox[2] + viewBox[3] * viewBox[3]);
                    if (gradient.type == GradientType::kLinearGradient)
                    {
                        // https://www.w3.org/TR/SVG11/pservers.html#LinearGradients
                        gradient.x1 = std::isfinite(gradient.x1) ? gradient.x1 : 0;
                        gradient.y1 = std::isfinite(gradient.y1) ? gradient.y1 : 0;
                        gradient.x2 = std::isfinite(gradient.x2) ? gradient.x2 : viewBox[2];
                        gradient.y2 = std::isfinite(gradient.y2) ? gradient.y2 : 0;
                    }
                    else
                    {
                        // https://www.w3.org/TR/SVG11/pservers.html#RadialGradients
                        gradient.cx = std::isfinite(gradient.cx) ? gradient.cx : 0.5f * viewBox[2];
                        gradient.cy = std::isfinite(gradient.cy) ? gradient.cy : 0.5f * viewBox[3];
                        gradient.fx = std::isfinite(gradient.fx) ? gradient.fx : gradient.cx;
                        gradient.fy = std::isfinite(gradient.fy) ? gradient.fy : gradient.cy;
                        gradient.r = std::isfinite(gradient.r) ? gradient.r : 0.5f * sqr;
                    }
                    paint = gradient;
                }
            }
            pos += position + 1;
        }
    }
    if (!SkipOptWsp(pos, end))
        return result;

    ColorImpl altPaint;
    if (std::distance(pos, end) >= 4 && std::string(pos, pos + 4).compare("none") == 0)
    {
        pos += 4;
        if (urlResult == SVGDocumentImpl::Result::kInvalid)
            result = SVGDocumentImpl::Result::kDisabled;
    }
    else
    {
        if (!ParseColor(pos, end, altPaint, true, result))
            return result;
    }

    if (!SkipOptWsp(pos, end))
    {
        if (urlResult == SVGDocumentImpl::Result::kInvalid && result != SVGDocumentImpl::Result::kInvalid)
            paint = altPaint;
        return result;
    }

    return SVGDocumentImpl::Result::kInvalid;
}

} // namespace SVGStringParser

} // namespace SVGNative
