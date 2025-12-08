#include "object.hpp"
#include "material.hpp"
#include "vec3.hpp"
#include <algorithm>
#include <vector>

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

polyhedron::polyhedron(int num_of_faces, vec3 *normals, double *intercepts,
                       material *mat) {
  this->num_of_faces = num_of_faces;
  this->normals = normals;
  this->intercepts = intercepts;
  this->mat = mat;
}

polyhedron::~polyhedron() {
  delete this->intercepts;
  delete this->normals;
  delete this->mat;
}

bool polyhedron::check_hit(const ray &r, interval ray_t,
                           hit_record &record) const {
  std::vector<point3> intersection_points;
  for (int i = 0; i < this->num_of_faces; i++) {
    vec3 normal = this->normals[i];
    double D = this->intercepts[i];
    bool hit = true;

    // Gets the point of interception between the ray and the plane
    double denom = dot(normal, r.get_direction());
    if (std::fabs(denom) < 1e-8) // No hit if the ray is parallel to the plane.
      hit = false;
    double t = (D - dot(normal, r.get_origin())) / denom;
    if (!ray_t.contains(t)) // Clips on the interval
      hit = false;
    point3 intersection = r.at(t);

    // Saves the point
    if (hit)
      intersection_points.emplace_back(intersection);
  }

  // Orders the points by distance from the ray's origin
  std::sort(intersection_points.begin(), intersection_points.end(),
            [&r](const point3 &a, const point3 &b) {
              return squared_distance(a, r.get_origin()) <
                     squared_distance(b, r.get_origin());
            });

  // Traverses them
  for (int i = 0; i < intersection_points.size(); i++) {
    point3 point = intersection_points[i];

    // Checks if they are on the same orientation for every plane
    double D;
    vec3 normal;

    int count_outside_or_on = 0;
    for (int j = 0; j < this->num_of_faces; j++) {
      D = this->intercepts[j];
      normal = this->normals[j];
      double result = dot(normal, point) + D;
      if (result >= 0)
        count_outside_or_on++;
    }

    int count_inside_or_on = 0;
    for (int j = 0; j < this->num_of_faces; j++) {
      D = this->intercepts[j];
      normal = this->normals[j];
      double result = dot(normal, point) + D;
      if (result <= 0)
        count_inside_or_on++;
    }

    // If they are, returns
    bool hit = (count_outside_or_on == this->num_of_faces) ||
               (count_inside_or_on == this->num_of_faces);
    if (hit) {
      record.t =
          (D - dot(normal, r.get_origin())) / dot(normal, r.get_direction());
      record.point = point;
      record.mat = this->mat;
      record.lig = nullptr;
      this->get_polyhedron_uv(point, normal, record.tex_u, record.tex_v);
      record.adjust_normal_for_ray(r, normal);
      record.is_light = false;

      return true;
    }
  }

  return false;
}

void polyhedron::get_polyhedron_uv(const point3 &p, const point3 &normal,
                                   double &u, double &v) {
  // Maps the given point p to 2D space of uv
  vec3 unit_normal = unit_vector(normal);
  vec3 e1 = unit_vector(cross(unit_normal, vec3(1, 0, 0)));
  if (e1.near_zero())
    e1 = unit_vector(cross(unit_normal, vec3(0, 0, 1)));
  vec3 e2 = unit_vector(cross(unit_normal, e1));

  u = dot(e1, p);
  v = dot(e2, p);

  u = u - int(u);
  u = u < 0 ? 1 + u : u;

  v = v - int(v);
  v = v < 0 ? 1 + v : v;
}