cmake_minimum_required(VERSION 3.13) # CMake version check
project(app)                         # Create project "app"
set(CMAKE_CXX_STANDARD 20)           # Enable C++20 standard

add_executable(app 
	"main.cpp" 
	"web_gpu_app.h" 
	"web_gpu_app.cpp"
)

if(WIN32)
# Don't launch a console.
set_target_properties(app PROPERTIES WIN32_EXECUTABLE ON)
endif()

# Imgui
add_library(imgui STATIC
	imgui/backends/imgui_impl_wgpu.h
	imgui/backends/imgui_impl_wgpu.cpp
	imgui/backends/imgui_impl_glfw.h
	imgui/backends/imgui_impl_glfw.cpp
	imgui/misc/cpp/imgui_stdlib.h
	imgui/misc/cpp/imgui_stdlib.cpp
	imgui/imconfig.h
	imgui/imgui.h
	imgui/imgui.cpp
	imgui/imgui_demo.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_internal.h
	imgui/imgui_tables.cpp
	imgui/imgui_widgets.cpp
	imgui/imstb_rectpack.h
	imgui/imstb_textedit.h
	imgui/imstb_truetype.h
)
target_include_directories(imgui PUBLIC imgui)
target_link_libraries(imgui PUBLIC webgpu_cpp webgpu_glfw)



if(EMSCRIPTEN)
  set_target_properties(app PROPERTIES SUFFIX ".html")
  target_link_options(app PRIVATE "-sUSE_WEBGPU=1" "-sUSE_GLFW=3")
else()
  set(DAWN_FETCH_DEPENDENCIES ON)
  add_subdirectory("dawn" EXCLUDE_FROM_ALL)
  target_link_libraries(app PRIVATE webgpu_cpp webgpu_dawn webgpu_glfw)
endif()

target_link_libraries(app PRIVATE imgui)