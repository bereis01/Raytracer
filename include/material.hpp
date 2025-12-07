#pragma once

#include "color.hpp"
#include "object.hpp"

class material {
public:
  virtual ~material() = default;

  virtual bool scatter(const ray &incident, const hit_record &record,
                       color &attenuation, ray &scattered,
                       double &coefficient) const {
    return false;
  }
};

class diffuse : public material {
public:
  diffuse(const color &coloration, double coefficient)
      : coloration(coloration), coefficient(coefficient) {}

  bool scatter(const ray &incident, const hit_record &record,
               color &attenuation, ray &scattered,
               double &coefficient) const override;

private:
  color coloration; // How much color it absorbs
  double coefficient;
};

class reflective : public material {
public:
  reflective(const color &coloration, double fuzz, double coefficient)
      : coloration(coloration), fuzz(fuzz < 1 ? fuzz : 1),
        coefficient(coefficient) {}

  bool scatter(const ray &incident, const hit_record &record,
               color &attenuation, ray &scattered,
               double &coefficient) const override;

private:
  color coloration; // How much color it absorbs
  double fuzz;
  double coefficient;
};

class refractive : public material {
public:
  refractive(double refraction_index, double coefficient)
      : refraction_index(refraction_index), coefficient(coefficient) {}

  bool scatter(const ray &incident, const hit_record &record,
               color &attenuation, ray &scattered,
               double &coefficient) const override;

private:
  static double reflectance(double cosine, double refraction_index) {
    // Uses Schlick's approximation for reflectance
    double r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }

  double refraction_index;
  double coefficient;
};