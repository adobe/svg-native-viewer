#include "svgnative/SVGRenderer.h"
#include <tuple>

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
      return;
    x = aX;
    y = aY;
    width = aWidth;
    height = aHeight;
  }
  std::tuple<Interval, Interval> Rect::intervals()
  {

  }
}
