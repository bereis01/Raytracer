#include "world.hpp"

world::world() {}

world::~world() {
  // Cleans memory
  for (auto obj : objects) {
    delete obj;
  }
}

void world::add_sphere(point3 center, double radius, material *diffuse,
                       material *reflective, material *refractive) {
  sphere *ball = new sphere(center, radius, diffuse, reflective, refractive);
  objects.emplace_back(ball);
}

bool world::check_hit(const ray &r, interval ray_t, hit_record &record) const {
  // Finds the first object the ray hits, if it hits any
  hit_record temp_rec;
  bool hit_anything = false;
  double closest_so_far = ray_t.max;
  for (auto obj : objects) {
    if (obj->check_hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      record = temp_rec;
    }
  }

  return hit_anything;
}