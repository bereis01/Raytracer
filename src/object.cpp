#include "object.hpp"
#include "material.hpp"

sphere::sphere(const point3 &center, double radius, material *mat)
    : center(center), radius(std::fmax(0, radius)), mat(mat) {}

sphere::~sphere() { delete this->mat; }

void sphere::get_sphere_uv(const point3 &p, double &u, double &v) {
  // Maps the given point p to 2D space of uv
  double theta = std::acos(-p.y());
  double phi = std::atan2(-p.z(), p.x()) + mathconst::pi;

  u = phi / (2 * mathconst::pi);
  v = theta / mathconst::pi;
}

bool sphere::check_hit(const ray &r, interval ray_t, hit_record &record) const {
  // Vector from the ray's origin to the sphere's center
  vec3 eye_to_sphere = this->center - r.get_origin();

  // a, h, c refer to the terms of the quadratic formula (h+-sqrt(h2-ac))/a,
  // where b=-2h
  double a = r.get_direction().length_squared();
  double h = dot(r.get_direction(), eye_to_sphere);
  double c = eye_to_sphere.length_squared() - this->radius * this->radius;

  // Discriminant negative -> no intersection
  // Discriminant zero -> one intersection (tangent)
  // Discriminant positive -> two intersections (entering and exiting)
  double discriminant = h * h - a * c;
  if (discriminant < 0)
    return false;

  // Find the nearest root that lies in the acceptable range.
  double sqrtd = std::sqrt(discriminant);
  double root = (h - sqrtd) / a;
  if (!ray_t.surrounds(root)) {
    root = (h + sqrtd) / a;
    if (!ray_t.surrounds(root))
      return false; // If none lies there
  }

  // Registers the hit in the hit record
  record.t = root;
  record.point = r.at(record.t);
  vec3 outward_normal = (record.point - this->center) / this->radius;
  record.adjust_normal_for_ray(r, outward_normal);
  record.is_light = false;
  record.mat = this->mat;
  this->get_sphere_uv(outward_normal, record.tex_u, record.tex_v);

  return true;
}

bulb::bulb(const point3 &center, double radius, light *lig)
    : center(center), radius(std::fmax(0, radius)), lig(lig) {}

void bulb::get_sphere_uv(const point3 &p, double &u, double &v) {
  // Maps the given point p to 2D space of uv
  double theta = std::acos(-p.y());
  double phi = std::atan2(-p.z(), p.x()) + mathconst::pi;

  u = phi / (2 * mathconst::pi);
  v = theta / mathconst::pi;
}

bulb::~bulb() { delete this->lig; }

bool bulb::check_hit(const ray &r, interval ray_t, hit_record &record) const {
  // Vector from the ray's origin to the sphere's center
  vec3 eye_to_sphere = this->center - r.get_origin();

  // a, h, c refer to the terms of the quadratic formula (h+-sqrt(h2-ac))/a,
  // where b=-2h
  double a = r.get_direction().length_squared();
  double h = dot(r.get_direction(), eye_to_sphere);
  double c = eye_to_sphere.length_squared() - this->radius * this->radius;

  // Discriminant negative -> no intersection
  // Discriminant zero -> one intersection (tangent)
  // Discriminant positive -> two intersections (entering and exiting)
  double discriminant = h * h - a * c;
  if (discriminant < 0)
    return false;

  // Find the nearest root that lies in the acceptable range.
  double sqrtd = std::sqrt(discriminant);
  double root = (h - sqrtd) / a;
  if (!ray_t.surrounds(root)) {
    root = (h + sqrtd) / a;
    if (!ray_t.surrounds(root))
      return false; // If none lies there
  }

  // Registers the hit in the hit record
  record.t = root;
  record.point = r.at(record.t);
  vec3 outward_normal = (record.point - this->center) / this->radius;
  record.adjust_normal_for_ray(r, outward_normal);
  record.is_light = true;
  record.lig = this->lig;
  this->get_sphere_uv(outward_normal, record.tex_u, record.tex_v);

  return true;
}