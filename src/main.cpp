#include "camera.hpp"
#include "color.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "vec3.hpp"
#include "world.hpp"
#include <fstream>
#include <vector>

class pigment_description {
public:
  pigment_description(std::string type, color c) : type(type), c(c) {}
  pigment_description(std::string type, color c1, color c2, double checker_size)
      : type(type), c(c1), alt(c2), checker_size((checker_size)) {}
  pigment_description(std::string type, std::string image_path)
      : type(type), image_path(image_path) {}

  std::string type;
  color c;
  color alt;
  double checker_size;
  std::string image_path;
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

  ////////////////////
  // Declaring objects
  ////////////////////

  std::cout << "Declaring objects" << std::endl;

  camera rt_cam;
  world rt_world;
  std::vector<pigment_description> pigment_descriptions;
  std::vector<material_description> material_descriptions;

  // Temporary ground
  checker *tex = new checker(10.0f, color(0, 0, 0), color(1, 1, 1));
  material *mat = new material(tex);
  rt_world.add_sphere(point3(0.0f, -1100.0f, -100.0f), 1000.0f, mat);

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
  rt_cam.samples_per_pixel = 10;
  rt_cam.max_recursion_depth = 3;
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
  input_file >> x >> y >> z; // Color of the background
  input_file >> x >> y >> z;

  // Other lights
  for (int i = 0; i < (num_of_lights - 1); i++) {
    input_file >> x >> y >> z;
    point3 light_pos = point3(x, y, z);

    input_file >> x >> y >> z;
    color light_color = color(x, y, z);

    input_file >> x >> y >> z; // Attenuation parameters

    /* light *lig_mat = new light(4.0f * light_color);
    rt_world.add_bulb(light_pos, 10.0f, lig_mat); */
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
      input_file >> x >> y >> z;
      color c1(x, y, z);

      input_file >> x >> y >> z;
      color c2(x, y, z);

      double side;
      input_file >> side;

      pigment_descriptions.emplace_back(
          pigment_description(pigment_type, c1, c1, side));
    }

    else if (pigment_type == "texmap") {
      std::string texture;
      input_file >> texture;

      input_file >> x >> y >> z >> w;
      input_file >> x >> y >> z >> w;

      pigment_descriptions.emplace_back(
          pigment_description(pigment_type, texture));
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

      texture *tex = nullptr;
      if (pig_param.type == "solid")
        tex = new solid(pig_param.c);
      else if (pig_param.type == "checker")
        tex = new checker(pig_param.checker_size, pig_param.c, pig_param.alt);
      else if (pig_param.type == "texmap")
        tex = new image(pig_param.image_path.c_str());

      material *mat = new material(tex, 0.0f, mat_param.ka, mat_param.kd,
                                   mat_param.ks, mat_param.alpha, mat_param.kr,
                                   mat_param.kt, mat_param.ior);
      rt_world.add_sphere(point3(x, y, z), radius, mat);
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