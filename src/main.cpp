#include "camera.hpp"
#include "material.hpp"
#include "vec3.hpp"
#include "world.hpp"

int main() {
  // Camera setup
  camera c;
  c.aspect_ratio = 16.0f / 9.0f;
  c.img_width = 1200;
  c.samples_per_pixel = 10;
  c.max_recursion_depth = 5;
  c.fov = 20;
  c.eye = point3(13.0f, 2.0f, 3.0f);
  c.lookat = point3(0.0f, 0.0f, 0.0f);
  c.up = vec3(0.0f, 1.0f, 0.0f);
  c.defocus_angle = 0.6f;
  c.focus_distance = 10.0f;

  // World setup
  // The sphere to which the materials are assigned deletes them when destructed
  world w;

  material *material_ground = new diffuse(color(0.5f, 0.5f, 0.5f));
  w.add_sphere(point3(0.0f, -1000.0f, 0.0f), 1000.0f, material_ground);

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      double choose_mat = random_double();
      point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9) {
        material *sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          vec3 albedo = color::random() * color::random();
          sphere_material = new diffuse(albedo);
          w.add_sphere(center, 0.2, sphere_material);
        } else if (choose_mat < 0.95) {
          // metal
          vec3 albedo = color::random(0.5, 1);
          double fuzz = random_double(0, 0.5);
          sphere_material = new reflective(albedo, fuzz);
          w.add_sphere(center, 0.2, sphere_material);
        } else {
          // glass
          sphere_material = new refractive(1.5);
          w.add_sphere(center, 0.2, sphere_material);
        }
      }
    }
  }

  material *material1 = new refractive(1.5);
  w.add_sphere(point3(0, 1, 0), 1.0, material1);

  material *material2 = new diffuse(color(0.4, 0.2, 0.1));
  w.add_sphere(point3(-4, 1, 0), 1.0, material2);

  material *material3 = new reflective(color(0.7, 0.6, 0.5), 0.0);
  w.add_sphere(point3(4, 1, 0), 1.0, material3);

  // Renders the image
  c.render(w);
}