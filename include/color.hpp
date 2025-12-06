#pragma once

#include "vec3.hpp"
#include <fstream>

using color = vec3;

void write_color(std::ofstream &out, const color &pixel_color);
