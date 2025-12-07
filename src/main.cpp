#include "camera.hpp"
#include "material.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <fstream>
#include <vector>

class pigment_description {
public:
  pigment_description(std::string type, color c) : type(type), c(c) {}
  std::string type;
  color c;
};

class material_description {
public:
  material_description(double ka, double kd, double ks, double alpha, double kr,
                       double kt, double ior)
      : ka(ka), kd(kd), ks(ks), alpha(alpha), kr(kr), kt(kt), ior(ior) {}
  double ka, kd, ks, alpha, kr, kt, ior = 0.0f;
};

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

  std::cout << "Declaring objects" << std::endl;

  camera rt_cam;
  world rt_world;
  std::vector<pigment_description> pigment_descriptions;
  std::vector<material_description> material_descriptions;

  // Temporary ground
  diffuse *dif_mat = new diffuse(color(0.5f, 0.5f, 0.5f), 1.0f);
  reflective *rfl_mat = new reflective(color(0, 0, 0), 0.0f, 0.0f);
  refractive *rfr_mat = new refractive(1.5f, 0.0f);
  rt_world.add_sphere(point3(0.0f, -1100.0f, -100.0f), 1000.0f, dif_mat,
                      rfl_mat, rfr_mat);

  ///////////////
  // Camera setup
  ///////////////

  std::cout << "Camera setup." << std::endl;

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
  rt_cam.img_width = 800;
  rt_cam.samples_per_pixel = 1;
  rt_cam.max_recursion_depth = 10;
  rt_cam.defocus_angle = 0.0f;
  rt_cam.focus_distance = 10.0f;

  //////////////
  // Light setup
  //////////////

  std::cout << "Light setup." << std::endl;

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

  std::cout << "Pigment setup." << std::endl;

  int num_of_pigments;
  input_file >> num_of_pigments;

  std::string pigment_type;
  for (int i = 0; i < num_of_pigments; i++) {
    input_file >> pigment_type;

    if (pigment_type == "solid") {
      input_file >> x >> y >> z; // Color of the pigment
      pigment_descriptions.emplace_back(
          pigment_description(pigment_type, color(x, y, z)));
    }

    else if (pigment_type == "checker") {
      double side;
      input_file >> x >> y >> z;
      input_file >> x >> y >> z;
      input_file >> side;

      pigment_descriptions.emplace_back(
          pigment_description(pigment_type, color(x, y, z)));
    }

    else if (pigment_type == "texmap") {
      std::string texture;
      input_file >> texture;
      input_file >> x >> y >> z >> w;
      input_file >> x >> y >> z >> w;

      pigment_descriptions.emplace_back(
          pigment_description(pigment_type, color(x, y, z)));
    }
  }

  /////////////////
  // Material setup
  /////////////////

  std::cout << "Material setup." << std::endl;

  int num_of_materials;
  input_file >> num_of_materials;

  double ka, kd, ks, alpha, kr, kt, ior;
  for (int i = 0; i < num_of_materials; i++) {
    input_file >> ka >> kd >> ks >> alpha >> kr >> kt >> ior;
    material_descriptions.emplace_back(
        material_description(ka, kd, ks, alpha, kr, kt, ior));
  }

  ///////////////
  // Object setup
  ///////////////

  std::cout << "Object setup." << std::endl;

  int num_of_objects;
  input_file >> num_of_objects;

  int pigment_index, material_index;
  std::string object_type;
  for (int i = 0; i < num_of_objects; i++) {
    input_file >> pigment_index >> material_index >> object_type;

    if (object_type == "sphere") {
      double radius;
      input_file >> x >> y >> z >> radius;

      pigment_description pig_param = pigment_descriptions[pigment_index];
      material_description mat_param = material_descriptions[material_index];

      diffuse *dif_mat = new diffuse(pig_param.c, mat_param.kd);
      reflective *rfl_mat = new reflective(pig_param.c, 0.0f, mat_param.kr);
      refractive *rfr_mat = new refractive(mat_param.ior, mat_param.kt);
      rt_world.add_sphere(point3(x, y, z), radius, dif_mat, rfl_mat, rfr_mat);
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

  std::cout << "Rendering." << std::endl;

  rt_cam.render(rt_world, output_file);

  ////////////
  // Finishing
  ////////////

  std::cout << "Finishing." << std::endl;

  // Closing files
  input_file.close();
  output_file.close();

  return 0;
}