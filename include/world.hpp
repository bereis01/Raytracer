#pragma once

#include "object.hpp"
#include <vector>

class world {
public:
  world();
  ~world();

  void add_sphere(point3 center, double radius, material *diffuse,
                  material *reflective, material *refractive);

  bool check_hit(const ray &r, interval ray_t, hit_record &record) const;

private:
  std::vector<object *> objects;
};