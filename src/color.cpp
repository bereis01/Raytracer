#include "color.hpp"
#include "interval.hpp"

inline double linear_to_gamma(double linear_component) {
  // Applies a gamma 2 transformation
  if (linear_component > 0)
    return std::sqrt(linear_component);
  return 0;
}

void write_color(std::ofstream &out, const color &pixel_color) {
  double r = pixel_color.x();
  double g = pixel_color.y();
  double b = pixel_color.z();

  /* // Applies a gamma 2 transformation
  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b); */

  // Translates the [0,1] component values to the interval [0,255]
  static const interval intensity(0.000f, 0.999f);
  int rbyte = int(256 * intensity.clamp(r));
  int gbyte = int(256 * intensity.clamp(g));
  int bbyte = int(256 * intensity.clamp(b));

  // Writes out the pixel color components
  out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}
