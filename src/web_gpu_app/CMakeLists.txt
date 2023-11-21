cmake_minimum_required(VERSION 3.13)

project(web_gpu_app)

add_library(web_gpu_app STATIC)

target_include_directories(web_gpu_app PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include)

target_include_directories(web_gpu_app PRIVATE
  ${CMAKE_CURRENT_LIST_DIR})

target_sources(web_gpu_app PUBLIC
  include/web_gpu_app/app.h
  include/web_gpu_app/renderer.h
  include/web_gpu_app/ui.h
  include/web_gpu_app/utils.h
  include/web_gpu_app/web_gpu_renderer.h
)

target_sources(web_gpu_app PRIVATE
  app.cpp
  ui.cpp
  web_gpu_renderer.cpp
)

# Imgui
add_library(imgui STATIC
  ${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_wgpu.h
	${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_wgpu.cpp
	${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.h
	${CMAKE_SOURCE_DIR}/third_party/imgui/backends/imgui_impl_glfw.cpp
	${CMAKE_SOURCE_DIR}/third_party/imgui/misc/cpp/imgui_stdlib.h
	${CMAKE_SOURCE_DIR}/third_party/imgui/misc/cpp/imgui_stdlib.cpp
	${CMAKE_SOURCE_DIR}/third_party/imgui/imconfig.h
	${CMAKE_SOURCE_DIR}/third_party/imgui/imgui.h
	${CMAKE_SOURCE_DIR}/third_party/imgui/imgui.cpp
	${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_demo.cpp
	${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_draw.cpp
	${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_internal.h
	${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_tables.cpp
	${CMAKE_SOURCE_DIR}/third_party/imgui/imgui_widgets.cpp
	${CMAKE_SOURCE_DIR}/third_party/imgui/imstb_rectpack.h
	${CMAKE_SOURCE_DIR}/third_party/imgui/imstb_textedit.h
	${CMAKE_SOURCE_DIR}/third_party/imgui/imstb_truetype.h
)

target_include_directories(imgui PUBLIC 
  ${CMAKE_SOURCE_DIR}/third_party/imgui
)

target_link_libraries(imgui PUBLIC 
  webgpu_cpp
  webgpu_glfw
)

if(WIN32)
# Don't launch a console.
set_target_properties(web_gpu_app PROPERTIES WIN32_EXECUTABLE ON)
endif()

target_include_directories(web_gpu_app PUBLIC 
  ${CMAKE_SOURCE_DIR}/third_party/glm
  ${CMAKE_SOURCE_DIR}/third_party/imgui
  ${CMAKE_SOURCE_DIR}/third_party/stb
  ${CMAKE_SOURCE_DIR}/third_party/tinyobj
)

if(EMSCRIPTEN)
  set_target_properties(web_gpu_app PROPERTIES SUFFIX ".html")
  target_link_options(web_gpu_app PRIVATE "-sUSE_WEBGPU=1" "-sUSE_GLFW=3")
else()
  set(DAWN_FETCH_DEPENDENCIES ON)
  target_link_libraries(web_gpu_app PUBLIC webgpu_cpp webgpu_dawn webgpu_glfw)
endif()

target_link_libraries(web_gpu_app PRIVATE imgui)