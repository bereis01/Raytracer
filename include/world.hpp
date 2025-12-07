#pragma once

#include "object.hpp"
#include <vector>

class world {
public:
  world();
  ~world();

  void add_sphere(point3 center, double radius, material *mat);
  // void add_bulb(point3 center, double radius, material *light);

  bool check_hit(const ray &r, interval ray_t, hit_record &record) const;

private:
  std::vector<object *> objects;
};