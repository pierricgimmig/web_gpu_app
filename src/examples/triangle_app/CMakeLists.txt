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

if(EMSCRIPTEN)
  set_target_properties(triangle_app PROPERTIES SUFFIX ".html")
  target_link_options(triangle_app PRIVATE "-sUSE_WEBGPU=1" "-sUSE_GLFW=3")
endif()