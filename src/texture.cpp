#include "texture.hpp"
#include "interval.hpp"
#include "rtw_stb_image.h"

color checker::value(double u, double v, const point3 &p) const {
  // Gets the "bin" of the point
  int xInteger = int(std::floor(this->inv_scale * p.x()));
  int yInteger = int(std::floor(this->inv_scale * p.y()));
  int zInteger = int(std::floor(this->inv_scale * p.z()));

  // Checks to which color it pertains and returns it
  bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;
  return isEven ? even : odd;
}

image::image(const char *filename) { this->img = new rtw_image(filename); }

image::~image() { delete this->img; }

color image::value(double u, double v, const point3 &p) const {
  // If we have no texture data, then return solid cyan as a debugging aid
  if (this->img->height() <= 0)
    return color(0, 1, 1);

  // Clamp input texture coordinates to [0,1] x [1,0]
  u = interval(0, 1).clamp(u);
  v = 1.0 - interval(0, 1).clamp(v); // Flip V to image coordinates

  // Reads the pixel color from the image
  int i = int(u * this->img->width());
  int j = int(v * this->img->height());
  const unsigned char *pixel = this->img->pixel_data(i, j);

  double color_scale = 1.0 / 255.0;
  return color(color_scale * pixel[0], color_scale * pixel[1],
               color_scale * pixel[2]);
}