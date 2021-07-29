#include "svgnative/SVGRenderer.h"
#include <tuple>
#include <cmath>
#include <stdexcept>

namespace SVGNative {
  Interval::Interval(float u, float v)
  {
    if (u <= v)
    {
      a = u;
      b = v;
    }
    else
    {
      a = v;
      b = u;
    }
  }

  Interval Interval::operator&(Interval other)
  {
    // return an empty interval if either of the intervals is empty
    if ((!*this) || (!other))
      return Interval();

    float u = (std::max)(this->Min(), other.Min());
    float v = (std::min)(this->Max(), other.Max());
    return (u <= v) ? Interval(u, v) : Interval();
  }
  Interval Interval::operator|(Interval other)
  {
    if (this->isEmpty() && !other.isEmpty())
      return other;
    else if(!this->isEmpty() && other.isEmpty())
      return *this;
    else if(this->isEmpty() && other.isEmpty())
      return Interval();
    else
      return Interval((std::min)(this->Min(), other.Min()), (std::max)(this->Max(), other.Max()));
  }
  bool Interval::contains(Interval other)
  {
    if (other.isEmpty())
      return true;
    else if(this->isEmpty())
      return false;
    else
      return this->Min() <= other.Min() && this->Max() >= other.Max();
  }

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
  bool Rect::isEmpty()
  {
    Intervals rect_intervals = intervals();
    return std::get<0>(rect_intervals).isEmpty() || std::get<1>(rect_intervals).isEmpty();
  }
  bool Rect::contains(Rect other)
  {
    Intervals this_intervals = intervals();
    Intervals other_intervals = other.intervals();
    return std::get<0>(this_intervals).contains(std::get<0>(other_intervals)) &&
           std::get<1>(this_intervals).contains(std::get<1>(other_intervals));
  }
  bool Rect::operator==(Rect other)
  {
    return x == other.x && y == other.y && width == other.width && height == other.height;
  }
  Rect Rect::operator&(Rect other)
  {
    Intervals intervals_a = intervals();
    Intervals intervals_b = other.intervals();
    Interval result_x = std::get<0>(intervals_a) & std::get<0>(intervals_b);
    Interval result_y = std::get<1>(intervals_a) & std::get<1>(intervals_b);
    if (result_x.isEmpty() || result_y.isEmpty())
      return Rect{};
    return Rect(result_x.Min(), result_y.Min(), result_x.Max() - result_x.Min(), result_y.Max() - result_y.Min());
  }
  Rect Rect::operator|(Rect other)
  {
    Intervals intervals_a = intervals();
    Intervals intervals_b = other.intervals();
    Interval result_x = std::get<0>(intervals_a) | std::get<0>(intervals_b);
    Interval result_y = std::get<1>(intervals_a) | std::get<1>(intervals_b);
    if (result_x.isEmpty() || result_y.isEmpty())
      return Rect{};
    return Rect(result_x.Min(), result_y.Min(), result_x.Max() - result_x.Min(), result_y.Max() - result_y.Min());
  }
  float Rect::MaxDiffVertex(Rect other)
  {
      float top_left_diff = std::sqrt(std::pow(Left() - other.Left(), 2) + std::pow(Top() - other.Top(), 2));
      float top_right_diff = std::sqrt(std::pow(Right() - other.Right(), 2) + std::pow(Top() - other.Top(), 2));
      float bottom_left_diff = std::sqrt(std::pow(Left() - other.Left(), 2) + std::pow(Bottom() - other.Bottom(), 2));
      float bottom_right_diff = std::sqrt(std::pow(Right() - other.Right(), 2) + std::pow(Bottom() - other.Bottom(), 2));
      return std::max({top_left_diff, top_right_diff, bottom_left_diff, bottom_right_diff});
  }
}
