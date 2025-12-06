#pragma once

#include "color.hpp"
#include "world.hpp"

class camera {
public:
  void render(const world &w);

  // Image parameters
  double aspect_ratio = 4.0f / 3.0f;
  int img_width = 400;

  // Camera parameters
  double fov = 90; // In degrees
  point3 eye = point3(0.0f, 0.0f, 0.0f);
  point3 lookat = point3(0.0f, 0.0f, -1.0f);
  vec3 up = vec3(0.0f, 1.0f, 0.0f);
  double defocus_angle = 0.0f;
  double focus_distance = 10.0f;

  // Rendering parameters
  int samples_per_pixel = 10;
  int max_recursion_depth = 10;

private:
  void initialize();
  ray get_ray_sample(int i, int j) const;
  color ray_color(const ray &r, int depth, const world &w) const;

  // Image parameters
  int img_height;
  point3 pixel_pos_upper_left;
  vec3 pixel_u;
  vec3 pixel_v;

  // Camera parameters
  vec3 u, v, w; // Camera's coordinate system
  vec3 defocus_disk_hor_radius;
  vec3 defocus_disk_ver_radius;

  // Rendering parameters
  double pixel_sample_color_scale;
};