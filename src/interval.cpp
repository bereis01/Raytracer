#include "interval.hpp"

const interval interval::empty =
    interval(+mathconst::infinity, -mathconst::infinity);
const interval interval::universe =
    interval(-mathconst::infinity, +mathconst::infinity);

double interval::clamp(double x) const {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}