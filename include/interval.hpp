#pragma once

#include "constants.hpp"

class interval {
public:
  double min, max;

  interval()
      : min(+mathconst::infinity), max(-mathconst::infinity) {
  } // Default interval is empty
  interval(double min, double max) : min(min), max(max) {}

  double size() const { return max - min; }
  bool contains(double x) const { return min <= x && x <= max; }
  bool surrounds(double x) const { return min < x && x < max; }
  double clamp(double x) const;

  static const interval empty, universe;
};
