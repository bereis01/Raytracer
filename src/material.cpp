#include "material.hpp"

bool diffuse::scatter(const ray &incident, const hit_record &record,
                      color &attenuation, ray &scattered) const {
  // Generates the direction to which the ray is reflected
  // In the case of diffuse material, "random" following the normal
  vec3 scatter_direction = record.normal + random_unit_vector();

  // Catch degenerate scatter direction
  if (scatter_direction.near_zero())
    scatter_direction = record.normal;

  scattered = ray(record.point, scatter_direction);
  attenuation = this->coloration;
  return true;
}

bool reflective::scatter(const ray &incident, const hit_record &record,
                         color &attenuation, ray &scattered) const {
  // Generates the direction to which the ray is reflected
  // In the case of reflective material, symmetric according to normal
  vec3 scatter_direction = reflect(incident.get_direction(), record.normal);

  // Generates the fuzziness
  scatter_direction =
      unit_vector(scatter_direction) + (fuzz * random_unit_vector());

  // Catch degenerate scatter direction
  if (scatter_direction.near_zero())
    scatter_direction = record.normal;

  scattered = ray(record.point, scatter_direction);
  attenuation = this->coloration;
  return (dot(scattered.get_direction(), record.normal) > 0);
}

bool refractive::scatter(const ray &incident, const hit_record &record,
                         color &attenuation, ray &scattered) const {
  // Adjusts the refraction index according to if the ray is entering or exiting
  // the material
  double refraction_index = record.is_ray_outside
                                ? (1.0 / this->refraction_index)
                                : this->refraction_index;

  // Generates the scattered ray as the refracted ray
  // Considers the case in which the ray actually reflects at sufficient glance
  // angles and low refraction indexes
  vec3 unit_direction = unit_vector(incident.get_direction());

  double cos_theta = std::fmin(dot(-unit_direction, record.normal), 1.0);
  double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
  bool cannot_refract = (refraction_index * sin_theta) > 1.0;
  bool schlick_correction =
      (cannot_refract ||
       this->reflectance(cos_theta, refraction_index) > random_double());

  vec3 scatter_direction =
      schlick_correction
          ? reflect(unit_direction, record.normal)
          : refract(unit_direction, record.normal, refraction_index);

  scattered = ray(record.point, scatter_direction);
  attenuation = color(1.0, 1.0, 1.0);
  return true;
}