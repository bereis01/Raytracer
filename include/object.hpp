#pragma once

#include "interval.hpp"
#include "ray.hpp"

// Pre-defined class to avoid circular reference
class material;
class light;

class hit_record {
public:
  point3 point;
  vec3 normal;

  bool is_light;
  material *mat;
  light *lig;

  double t;
  bool is_ray_outside;

  double tex_u;
  double tex_v;

  void adjust_normal_for_ray(const ray &r, const vec3 &outward_normal) {
    // Makes the normal point opposite of the ray
    // front_face indicates if the ray is inside or outside
    is_ray_outside = dot(r.get_direction(), outward_normal) < 0;
    normal = is_ray_outside ? outward_normal : -outward_normal;
  }
};

class object {
public:
  virtual ~object() = default;

  virtual bool check_hit(const ray &r, interval ray_t,
                         hit_record &rec) const = 0;
};

class sphere : public object {
public:
  sphere(const point3 &center, double radius, material *mat);

  bool check_hit(const ray &r, interval ray_t,
                 hit_record &record) const override;

  static void get_sphere_uv(const point3 &p, double &u, double &v);

private:
  // Geometric properties
  point3 center;
  double radius;

  // Color properties
  material *mat;
};

class bulb : public object {
public:
  bulb(const point3 &center, double radius, light *lig);

  bool check_hit(const ray &r, interval ray_t,
                 hit_record &record) const override;

  static void get_sphere_uv(const point3 &p, double &u, double &v);

private:
  // Geometric properties
  point3 center;
  double radius;

  // Color properties
  light *lig;
};