cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME "rt_on")
project(${PROJECT_NAME})

# Necessary for clangd identifying headers
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include/)

# add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(${PROJECT_NAME} 
                src/camera.cpp
                src/color.cpp
                src/interval.cpp
                src/main.cpp
                src/material.cpp
                src/object.cpp
                src/world.cpp)

# target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)