#include "svgnative/Rect.h"
#include "Interval.h"
#include <tuple>
#include <cmath>
#include <stdexcept>

namespace SVGNative
{
    Rect::Rect(float aX, float aY, float aWidth, float aHeight)
    {
        if (aWidth < 0 || aHeight < 0)
        {
            throw std::invalid_argument("Height or Width of a rectangle can't be negative!");
        }
        x = aX;
        y = aY;
        width = aWidth;
        height = aHeight;
    }
    bool Rect::IsEmpty() const
    {
        IntervalPair rectIntervals = Intervals();
        return std::get<0>(rectIntervals).IsEmpty() || std::get<1>(rectIntervals).IsEmpty();
    }
    bool Rect::Contains(Rect other) const
    {
        IntervalPair thisIntervals = Intervals();
        IntervalPair otherIntervals = other.Intervals();
        return std::get<0>(thisIntervals).Contains(std::get<0>(otherIntervals)) &&
            std::get<1>(thisIntervals).Contains(std::get<1>(otherIntervals));
    }
    bool Rect::operator==(Rect other) const
    {
        return x == other.x && y == other.y && width == other.width && height == other.height;
    }
    Rect Rect::operator&(Rect other) const
    {
        IntervalPair intervalsA = Intervals();
        IntervalPair intervalsB = other.Intervals();
        Interval resultX = std::get<0>(intervalsA) & std::get<0>(intervalsB);
        Interval resultY = std::get<1>(intervalsA) & std::get<1>(intervalsB);
        if (resultX.IsEmpty() || resultY.IsEmpty())
            return Rect{0, 0, 0, 0};
        return Rect(resultX.Min(), resultY.Min(), resultX.Max() - resultX.Min(), resultY.Max() - resultY.Min());
    }
    Rect Rect::operator|(Rect other) const
    {
        IntervalPair intervalsA = Intervals();
        IntervalPair intervalsB = other.Intervals();
        Interval resultX = std::get<0>(intervalsA) | std::get<0>(intervalsB);
        Interval resultY = std::get<1>(intervalsA) | std::get<1>(intervalsB);
        if (resultX.IsEmpty() || resultY.IsEmpty())
            return Rect{0, 0, 0, 0};
        return Rect(resultX.Min(), resultY.Min(), resultX.Max() - resultX.Min(), resultY.Max() - resultY.Min());
    }
    float Rect::MaxDiffVertex(Rect other) const
    {
        float topLeftDiff = std::sqrt(std::pow(Left() - other.Left(), 2) + std::pow(Top() - other.Top(), 2));
        float topRightDiff = std::sqrt(std::pow(Right() - other.Right(), 2) + std::pow(Top() - other.Top(), 2));
        float bottomLeftDiff = std::sqrt(std::pow(Left() - other.Left(), 2) + std::pow(Bottom() - other.Bottom(), 2));
        float bottomRightDiff = std::sqrt(std::pow(Right() - other.Right(), 2) + std::pow(Bottom() - other.Bottom(), 2));
        return std::max({topLeftDiff, topRightDiff, bottomLeftDiff, bottomRightDiff});
    }

    IntervalPair Rect::Intervals() const { return IntervalPair(Interval(x, x + width), Interval(y, y + height)); }
}
