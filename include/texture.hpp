#pragma once

#include "color.hpp"

class rtw_image;

class texture {
public:
  virtual ~texture() = default;

  virtual color value(double u, double v, const point3 &p) const = 0;
};

class solid : public texture {
public:
  solid(const color &coloration) : coloration(coloration) {}

  solid(double red, double green, double blue)
      : solid(color(red, green, blue)) {}

  color value(double u, double v, const point3 &p) const override {
    return coloration;
  }

private:
  color coloration;
};

class checker : public texture {
public:
  checker(double scale, color even, color odd)
      : inv_scale(1.0 / scale), even(even), odd(odd) {}

  color value(double u, double v, const point3 &p) const override;

private:
  double inv_scale;
  color even;
  color odd;
};

class image : public texture {
public:
  image(const char *filename);
  ~image();

  color value(double u, double v, const point3 &p) const override;

private:
  rtw_image *img;
};