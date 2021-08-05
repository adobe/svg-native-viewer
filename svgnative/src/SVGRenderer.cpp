#include "svgnative/SVGRenderer.h"
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
        IntervalPair rect_Intervals = Intervals();
        return std::get<0>(rect_Intervals).IsEmpty() || std::get<1>(rect_Intervals).IsEmpty();
    }
    bool Rect::Contains(Rect other) const
    {
        IntervalPair this_intervals = Intervals();
        IntervalPair other_intervals = other.Intervals();
        return std::get<0>(this_intervals).Contains(std::get<0>(other_intervals)) &&
            std::get<1>(this_intervals).Contains(std::get<1>(other_intervals));
    }
    bool Rect::operator==(Rect other) const
    {
        return x == other.x && y == other.y && width == other.width && height == other.height;
    }
    Rect Rect::operator&(Rect other) const
    {
        IntervalPair intervals_a = Intervals();
        IntervalPair intervals_b = other.Intervals();
        Interval result_x = std::get<0>(intervals_a) & std::get<0>(intervals_b);
        Interval result_y = std::get<1>(intervals_a) & std::get<1>(intervals_b);
        if (result_x.IsEmpty() || result_y.IsEmpty())
            return Rect{0, 0, 0, 0};
        return Rect(result_x.Min(), result_y.Min(), result_x.Max() - result_x.Min(), result_y.Max() - result_y.Min());
    }
    Rect Rect::operator|(Rect other) const
    {
        IntervalPair intervals_a = Intervals();
        IntervalPair intervals_b = other.Intervals();
        Interval result_x = std::get<0>(intervals_a) | std::get<0>(intervals_b);
        Interval result_y = std::get<1>(intervals_a) | std::get<1>(intervals_b);
        if (result_x.IsEmpty() || result_y.IsEmpty())
            return Rect{0, 0, 0, 0};
        return Rect(result_x.Min(), result_y.Min(), result_x.Max() - result_x.Min(), result_y.Max() - result_y.Min());
    }
    float Rect::MaxDiffVertex(Rect other) const
    {
        float top_left_diff = std::sqrt(std::pow(Left() - other.Left(), 2) + std::pow(Top() - other.Top(), 2));
        float top_right_diff = std::sqrt(std::pow(Right() - other.Right(), 2) + std::pow(Top() - other.Top(), 2));
        float bottom_left_diff = std::sqrt(std::pow(Left() - other.Left(), 2) + std::pow(Bottom() - other.Bottom(), 2));
        float bottom_right_diff = std::sqrt(std::pow(Right() - other.Right(), 2) + std::pow(Bottom() - other.Bottom(), 2));
        return std::max({top_left_diff, top_right_diff, bottom_left_diff, bottom_right_diff});
    }

    IntervalPair Rect::Intervals() const { return IntervalPair(Interval(x, x + width), Interval(y, y + height)); }
}
