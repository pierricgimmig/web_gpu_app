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
 
  add_custom_command(TARGET triangle_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/bin/triangle_app.html ${CMAKE_SOURCE_DIR}/docs/index.html
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/bin/triangle_app.js ${CMAKE_SOURCE_DIR}/docs/
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/bin/triangle_app.wasm ${CMAKE_SOURCE_DIR}/docs/
  )
endif()