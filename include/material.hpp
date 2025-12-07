#pragma once

#include "color.hpp"
#include "object.hpp"
#include "texture.hpp"

class material {
public:
  material(texture *coloration);
  material(texture *coloration, double fuzz, double ambient_light_coeff,
           double diffuse_coeff, double specular_coeff, double specular_alpha,
           double reflection_coeff, double refraction_coeff,
           double refraction_index);

  ~material();

  bool scatter_diffuse(const ray &incident, const hit_record &record,
                       color &attenuation, ray &scattered,
                       double &diffuse_coeff) const;

  bool scatter_reflective(const ray &incident, const hit_record &record,
                          color &attenuation, ray &scattered,
                          double &reflection_coeff) const;

  bool scatter_refractive(const ray &incident, const hit_record &record,
                          color &attenuation, ray &scattered,
                          double &refraction_coeff) const;

  color emitted(double u, double v, const point3 &p) const {
    return color(0.0f, 0.0f, 0.0f);
  }

  color get_ambient(double &ambient_light_coeff, double u, double v,
                        const point3 &p) {
    ambient_light_coeff = this->ambient_light_coeff;
    return this->coloration->value(u, v, p);
  }

private:
  static double reflectance(double cosine, double refraction_index) {
    // Uses Schlick's approximation for reflectance
    double r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }

  texture *coloration;

  double ambient_light_coeff = 0.1f;

  double diffuse_coeff = 1.0f;
  double specular_coeff = 0.5f;
  double specular_alpha = 1.0f;

  double reflection_coeff = 0.0f;
  double fuzz = 0.0f;

  double refraction_coeff = 0.0f;
  double refraction_index = 1.0f;
};

/* class light : public material {
public:
  light(texture *coloration) : coloration(coloration) {}
  light(const color &emit) : coloration(new solid(emit)) {}
  ~light() { delete this->coloration; }

  color emitted(double u, double v, const point3 &p) const override {
    return coloration->value(u, v, p);
  }

private:
  texture *coloration;
}; */