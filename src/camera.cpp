#include "camera.hpp"
#include "material.hpp"
#include "vec3.hpp"

// Renders and outputs the image
void camera::render(const world &w, std::ofstream &output_file) {
  this->initialize();

  // PPM header
  output_file << "P3" << std::endl
              << this->img_width << " " << this->img_height << std::endl
              << "255" << std::endl;

  // Printing the RGB values of each pixel
  for (int i = 0; i < this->img_height; i++) {
    for (int j = 0; j < this->img_width; j++) {

      // Computes the color of the pixel
      color pixel_color = color(0.0f, 0.0f, 0.0f);

      // Traverses the amount of sample rays to consider
      for (int k = 0; k < this->samples_per_pixel; k++) {

        // Gets the ray (camera -> random sample around pixel)
        ray r = this->get_ray_sample(i, j);

        // Sums its color contribution to the total color
        pixel_color += this->pixel_sample_color_scale *
                       this->ray_color(r, this->max_recursion_depth, w);
      }

      // Writes the pixel to the image
      write_color(output_file, pixel_color);
    }

    // Logging
    std::cout << "\rScanlines remaining: " << (this->img_height - i) << ' '
              << std::flush;
  }

  // Logging
  std::cout << "\rDone.                 " << std::endl; // Logging
}

void camera::initialize() {
  // Image parameters
  double actual_aspect_ratio =
      double(this->img_width) / double(this->img_height);

  // Rendering parameters
  this->pixel_sample_color_scale = 1.0 / samples_per_pixel;

  // Camera parameters
  double theta = degrees_to_radians(this->fov);
  double h = std::tan(theta / 2.0f);

  double vp_height = 2.0f * h * this->focus_distance;
  double vp_width = vp_height * actual_aspect_ratio;

  this->w = unit_vector(this->eye - lookat);
  this->u = unit_vector(cross(this->up, w));
  this->v = cross(w, u);

  vec3 vp_u = vp_width * this->u;
  vec3 vp_v = -vp_height * this->v;
  this->pixel_u = vp_u / img_width;
  this->pixel_v = vp_v / img_height;

  point3 vp_pos_upper_left = this->eye - (this->focus_distance * this->w) -
                             (vp_u / 2.0f) - (vp_v / 2.0f);
  this->pixel_pos_upper_left = vp_pos_upper_left + 0.5f * (pixel_u + pixel_v);

  double defocus_radius =
      this->focus_distance *
      std::tan(degrees_to_radians(this->defocus_angle / 2.0f));
  this->defocus_disk_hor_radius = this->u * defocus_radius;
  this->defocus_disk_ver_radius = this->v * defocus_radius;
}

ray camera::get_ray_sample(int i, int j) const {
  // Samples a pixel position around the center of the given pixel
  vec3 offset = sample_square();
  vec3 pixel_sample = this->pixel_pos_upper_left +
                      ((j + offset.x()) * this->pixel_u) +
                      ((i + offset.y()) * this->pixel_v);

  // Constructs the ray from the camera to the pixel sample position
  point3 ray_origin =
      ((this->defocus_angle) <= 0.0f)
          ? this->eye
          : sample_disk(this->eye, this->defocus_disk_hor_radius,
                        this->defocus_disk_ver_radius);
  vec3 ray_direction = pixel_sample - ray_origin;
  return ray(ray_origin, ray_direction);
}

color camera::ray_color(const ray &r, int depth, const world &w) const {
  // Stops if the maximum depth has been reached
  if (depth <= 0)
    return color(0.0f, 0.0f, 0.0f);

  // Gets the first object the ray hits, if it hits any
  hit_record record;
  bool hit_anything =
      w.check_hit(r, interval(0.001f, mathconst::infinity), record);

  // If an object was hit, decides the color of the pixel based on its color
  // Recursive calls this function for the reflected ray
  if (hit_anything) {
    // Just return its color if it is a light
    if (record.is_light) {
      return record.lig->emitted(record.tex_u, record.tex_v, record.point);
    }

    ray scattered;
    color attenuation;
    bool ray_was_scattered_by_object;
    color final_color = color(0.0f, 0.0f, 0.0f);

    // Ambient color
    double ambient_light_coeff;
    color object_color = record.mat->get_ambient(
        ambient_light_coeff, record.tex_u, record.tex_v, record.point);
    final_color += ambient_light_coeff * object_color;

    // Diffuse ray
    double diffuse_c;
    color diffuse_color;
    ray_was_scattered_by_object = record.mat->scatter_diffuse(
        r, record, attenuation, scattered, diffuse_c);
    if (ray_was_scattered_by_object)
      diffuse_color = attenuation * ray_color(scattered, depth - 1, w);

    // Reflective ray
    double reflective_c;
    color reflective_color;
    ray_was_scattered_by_object = record.mat->scatter_reflective(
        r, record, attenuation, scattered, reflective_c);
    if (ray_was_scattered_by_object && (reflective_c > 0.0f))
      reflective_color = attenuation * ray_color(scattered, depth - 1, w);

    // Refractive ray
    double refractive_c;
    color refractive_color;
    ray_was_scattered_by_object = record.mat->scatter_refractive(
        r, record, attenuation, scattered, refractive_c);
    if (ray_was_scattered_by_object && (refractive_c > 0.0f))
      refractive_color = attenuation * ray_color(scattered, depth - 1, w);

    final_color += (diffuse_c * diffuse_color) +
                   (reflective_c * reflective_color) +
                   (refractive_c * refractive_color);
    return final_color;
  }

  // If the ray hits nothing, returns background color
  return this->background_color;
}