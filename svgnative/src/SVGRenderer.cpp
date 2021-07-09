#include "svgnative/SVGRenderer.h"

namespace SVGNative {
  Interval::Interval(float u, float v)
  {
    isEmpty = false;
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

  bool Interval::contains(Interval other)
  {
    return this->Min() <= other.Min() && this->Max() >= other.Max();
  }

}
