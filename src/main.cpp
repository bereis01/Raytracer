#include "camera.hpp"
#include "material.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <fstream>
#include <vector>

int main(int argc, char *argv[]) {
  /////////////////////////////////
  // Setting up files and arguments
  /////////////////////////////////
  if (argc < 3) {
    std::cout
        << "Please provide input and output files as command-line arguments!"
        << std::endl;
    return -1;
  }

  // Parsing command-line arguments
  char *input_file_name = argv[1];
  char *output_file_name = argv[2];

  std::ifstream input_file(input_file_name);
  std::ofstream output_file(output_file_name);

  // Shared variables
  double x, y, z, w;
  material *mat;

  ////////////////////
  // Declaring objects
  ////////////////////
  camera rt_cam;
  world rt_world;
  std::vector<material *> materials;

  materials.emplace_back(new diffuse(color(0.5, 0.5, 0.5)));
  rt_world.add_sphere(point3(0.0f, -1010.0f, 0.0f), 1000.0f, materials.back());

  ///////////////
  // Camera setup
  ///////////////
  input_file >> x >> y >> z;
  rt_cam.eye = point3(x, y, z);

  input_file >> x >> y >> z;
  rt_cam.lookat = point3(x, y, z);

  input_file >> x >> y >> z;
  rt_cam.up = point3(x, y, z);

  double fov;
  input_file >> fov;
  rt_cam.fov = fov;

  rt_cam.aspect_ratio = 4.0f / 3.0f;
  rt_cam.img_width = 400;
  rt_cam.samples_per_pixel = 100;
  rt_cam.max_recursion_depth = 10;
  rt_cam.defocus_angle = 0.0f;
  rt_cam.focus_distance = 10.0f;

  //////////////
  // Light setup
  //////////////
  int num_of_lights;
  input_file >> num_of_lights;

  // Ambient light
  input_file >> x >> y >> z;

  input_file >> x >> y >> z;

  input_file >> x >> y >> z;

  // Other lights
  for (int i = 0; i < (num_of_lights - 1); i++) {
    input_file >> x >> y >> z;

    input_file >> x >> y >> z;

    input_file >> x >> y >> z;
  }

  ////////////////
  // Pigment setup
  ////////////////
  int num_of_pigments;
  input_file >> num_of_pigments;

  std::string pigment_type;
  for (int i = 0; i < num_of_pigments; i++) {
    input_file >> pigment_type;

    if (pigment_type == "solid") {
      input_file >> x >> y >> z;
    }

    else if (pigment_type == "checker") {
      double side;
      input_file >> x >> y >> z;
      input_file >> x >> y >> z;
      input_file >> side;
    }

    else if (pigment_type == "texmap") {
      std::string texture;
      input_file >> texture;
      input_file >> x >> y >> z >> w;
      input_file >> x >> y >> z >> w;
    }
  }

  /////////////////
  // Material setup
  /////////////////
  int num_of_materials;
  input_file >> num_of_materials;

  double ka, kd, ks, alpha, kr, kt, ior;
  for (int i = 0; i < num_of_materials; i++) {
    input_file >> ka >> kd >> ks >> alpha >> kr >> kt >> ior;
  }

  ///////////////
  // Object setup
  ///////////////
  int num_of_objects;
  input_file >> num_of_objects;

  int pigment, material;
  std::string object_type;
  for (int i = 0; i < num_of_objects; i++) {
    input_file >> pigment >> material >> object_type;

    if (object_type == "sphere") {
      double radius;
      input_file >> x >> y >> z >> radius;

      materials.emplace_back(new diffuse(color(x, y, z)));
      rt_world.add_sphere(point3(x, y, z), radius, materials.back());
    }

    else if (object_type == "polyhedron") {
      int num_of_faces;
      for (int j = 0; j < num_of_faces; j++) {
        input_file >> x >> y >> z >> w;
      }
    }
  }

  ////////////
  // Rendering
  ////////////
  rt_cam.render(rt_world, output_file);

  ////////////
  // Finishing
  ////////////
  // Closing files
  input_file.close();
  output_file.close();

  // Cleaning memory
  for (auto mat : materials) {
    delete mat;
  }

  return 0;
}