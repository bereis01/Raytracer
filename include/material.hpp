#pragma once

#include "color.hpp"
#include "object.hpp"

class material {
public:
  virtual ~material() = default;

  virtual bool scatter(const ray &incident, const hit_record &record,
                       color &attenuation, ray &scattered) const {
    return false;
  }
};

class diffuse : public material {
public:
  diffuse(const color &coloration) : coloration(coloration) {}

  bool scatter(const ray &incident, const hit_record &record,
               color &attenuation, ray &scattered) const override;

private:
  color coloration; // How much color it absorbs
};

class reflective : public material {
public:
  reflective(const color &coloration, double fuzz)
      : coloration(coloration), fuzz(fuzz < 1 ? fuzz : 1) {}

  bool scatter(const ray &incident, const hit_record &record,
               color &attenuation, ray &scattered) const override;

private:
  color coloration; // How much color it absorbs
  double fuzz;
};

class refractive : public material {
public:
  refractive(double refraction_index) : refraction_index(refraction_index) {}

  bool scatter(const ray &incident, const hit_record &record,
               color &attenuation, ray &scattered) const override;

private:
  static double reflectance(double cosine, double refraction_index) {
    // Uses Schlick's approximation for reflectance
    double r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }

  double refraction_index;
};