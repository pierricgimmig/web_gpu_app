#include "web_gpu_app.h"

#include <GLFW/glfw3.h>

#include <iostream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_wgpu.h"
#include "glm/glm.hpp"
#include "utils.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

namespace {

#if defined(__EMSCRIPTEN__)
void EmscriptenMainLoop(void* app) { reinterpret_cast<App*>(app)->Render(); }
#endif

web_gpu_app::App* AppFromWindow(GLFWwindow* window) {
  return reinterpret_cast<web_gpu_app::App*>(glfwGetWindowUserPointer(window));
}

void OnGlfwResize(GLFWwindow* window, int width, int height) {
  AppFromWindow(window)->OnResize(width, height);
}

void OnGlfwSetCursorPos(GLFWwindow* window, double xpos, double ypos) {
  AppFromWindow(window)->OnMouseMove(xpos, ypos);
}

void OnGlfwSetMouseButton(GLFWwindow* window, int button, int action, int mods) {
  AppFromWindow(window)->OnMouseButton(button, action, mods);
}

void OnGlfwScroll(GLFWwindow* window, double x_offset, double y_offset) {
  AppFromWindow(window)->OnScroll(x_offset, y_offset);
}

}  // namespace

namespace web_gpu_app {

App::App() {
  window_ = SetupGlfwWindow(GetAppName().c_str(), this);
  renderer_ = std::make_unique<WebGpuRenderer>(window_);
  // Imgui
  SetupUi();
  MainLoop();
}

App::~App() {
  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplWGPU_Shutdown();
}

void App::MainLoop() {
#if defined(__EMSCRIPTEN__)
  emscripten_set_main_loop_arg(reinterpret_cast<void*>(this), EmscriptenMainLoop, 0, false);
#else
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
    Render();
  }
#endif
}

void App::Render() { renderer_->Render({}); }

GLFWwindow* App::SetupGlfwWindow(const char* title, void* user_pointer) {
  if (!glfwInit()) {
    return nullptr;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  static constexpr uint32_t kInitialWidth = 600;
  static constexpr uint32_t kInitialHeight = 400;
  GLFWwindow* window = glfwCreateWindow(kInitialWidth, kInitialHeight, title,
                                        /*monitor*/ nullptr, /*share*/ nullptr);

  // Setup callbacks.
  glfwSetWindowUserPointer(window, user_pointer);
  glfwSetFramebufferSizeCallback(window, &OnGlfwResize);
  glfwSetCursorPosCallback(window, &OnGlfwSetCursorPos);
  glfwSetMouseButtonCallback(window, &OnGlfwSetMouseButton);
  glfwSetScrollCallback(window, OnGlfwScroll);
  return window;
}

void App::SetupUi() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::GetIO();
  ImGui_ImplGlfw_InitForOther(window_, true);
  ImGui_ImplWGPU_Init(renderer_->GetDevice(), 3, WGPUTextureFormat_BGRA8Unorm,
                      WGPUTextureFormat_Depth24Plus);
  SetUiThemeDark();
}

void App::OnResize(int width, int height) { renderer_->OnResize(width, height); }

void App::OnMouseMove(double xpos, double ypos) {}
void App::OnMouseButton(int button, int action, int mods) {
  TRACE_VAR(button);
  TRACE_VAR(action);
  TRACE_VAR(mods);
}
void App::OnScroll(double xoffset, double yoffset) {
  TRACE_VAR(xoffset);
  TRACE_VAR(yoffset);
}

}  // namespace web_gpu_app