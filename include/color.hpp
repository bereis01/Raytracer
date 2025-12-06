#pragma once

#include "vec3.hpp"
#include <iostream>

using color = vec3;

void write_color(std::ostream &out, const color &pixel_color);
