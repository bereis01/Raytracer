#include "camera.hpp"
#include "material.hpp"
#include "world.hpp"

int main() {
  // Camera setup
  camera c;

  // Material setup
  // The sphere to which they are assigned deletes them when destructed
  material *material_ground = new diffuse(color(0.8f, 0.8f, 0.0f));
  material *material_center = new diffuse(color(0.1f, 0.2f, 0.5f));
  material *material_left = new refractive(1.5f);
  material *material_bubble = new refractive(1.0f / 1.5f);
  material *material_right = new reflective(color(0.8f, 0.6f, 0.2f), 0.3f);

  // World setup
  world w;

  w.add_sphere(point3(0.0f, -100.5f, -1.0f), 100.0f, material_ground);
  w.add_sphere(point3(0.0f, 0.0f, -1.2f), 0.5f, material_center);
  w.add_sphere(point3(-1.0f, 0.0f, -1.0f), 0.5f, material_left);
  w.add_sphere(point3(-1.0f, 0.0f, -1.0f), 0.4f, material_bubble);
  w.add_sphere(point3(1.0f, 0.0f, -1.0f), 0.5f, material_right);

  // Renders the image
  c.render(w);
}