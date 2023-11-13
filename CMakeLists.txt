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
	third_party/imgui/backends/imgui_impl_wgpu.h
	third_party/imgui/backends/imgui_impl_wgpu.cpp
	third_party/imgui/backends/imgui_impl_glfw.h
	third_party/imgui/backends/imgui_impl_glfw.cpp
	third_party/imgui/misc/cpp/imgui_stdlib.h
	third_party/imgui/misc/cpp/imgui_stdlib.cpp
	third_party/imgui/imconfig.h
	third_party/imgui/imgui.h
	third_party/imgui/imgui.cpp
	third_party/imgui/imgui_demo.cpp
	third_party/imgui/imgui_draw.cpp
	third_party/imgui/imgui_internal.h
	third_party/imgui/imgui_tables.cpp
	third_party/imgui/imgui_widgets.cpp
	third_party/imgui/imstb_rectpack.h
	third_party/imgui/imstb_textedit.h
	third_party/imgui/imstb_truetype.h
)
target_include_directories(imgui PUBLIC third_party/imgui)
target_link_libraries(imgui PUBLIC webgpu_cpp webgpu_glfw)

if(EMSCRIPTEN)
  set_target_properties(app PROPERTIES SUFFIX ".html")
  target_link_options(app PRIVATE "-sUSE_WEBGPU=1" "-sUSE_GLFW=3")
else()
  set(DAWN_FETCH_DEPENDENCIES ON)
  add_subdirectory("third_party/dawn" EXCLUDE_FROM_ALL)
  target_link_libraries(app PRIVATE webgpu_cpp webgpu_dawn webgpu_glfw)
endif()

target_include_directories(app PRIVATE third_party/glm)
target_include_directories(app PRIVATE third_party/imgui)
target_link_libraries(app PRIVATE imgui)