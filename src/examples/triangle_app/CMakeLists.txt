cmake_minimum_required(VERSION 3.13)

project(triangle_app)

add_executable(triangle_app
  main.cpp
  triangle_app.cpp
  triangle_app.h
)

target_link_libraries(triangle_app PRIVATE
  imgui
  web_gpu_app
)
