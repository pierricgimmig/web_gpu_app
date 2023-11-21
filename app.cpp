#include "app.h"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "utils.h"
#include "web_gpu_renderer.h"

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

}  // namespace

namespace web_gpu_app {

App::App() { window_ = CreateGlfwWindow("web_gpu_app", this); }

App::~App() {}

void App::Run() {
#if defined(__EMSCRIPTEN__)
  emscripten_set_main_loop_arg(reinterpret_cast<void*>(this), EmscriptenMainLoop, 0, false);
#else
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
    Render();
  }
#endif
}

void App::Render() {
  GetRenderer()->BeginFrame();
  Renderables renderables = Update();
  GetRenderer()->EndFrame(renderables);
}

GLFWwindow* App::CreateGlfwWindow(const char* title, void* user_pointer) {
  if (!glfwInit()) {
    return nullptr;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  static constexpr uint32_t kInitialWidth = 600;
  static constexpr uint32_t kInitialHeight = 400;
  GLFWwindow* window = glfwCreateWindow(kInitialWidth, kInitialHeight, title,
                                        /*monitor*/ nullptr, /*share*/ nullptr);

  glfwSetWindowUserPointer(window, user_pointer);
  glfwSetFramebufferSizeCallback(window, &OnGlfwResize);
  glfwSetCursorPosCallback(window, &OnGlfwSetCursorPos);
  glfwSetMouseButtonCallback(window, &OnGlfwSetMouseButton);
  glfwSetScrollCallback(window, OnGlfwScroll);
  return window;
}

void App::OnResize(int width, int height) { GetRenderer()->OnResize(width, height); }
void App::OnMouseMove(double xpos, double ypos) {}
void App::OnMouseButton(int button, int action, int mods) {}
void App::OnScroll(double xoffset, double yoffset) {}

void App::OnGlfwResize(GLFWwindow* window, int width, int height) {
  AppFromWindow(window)->OnResize(width, height);
}

void App::OnGlfwSetCursorPos(GLFWwindow* window, double xpos, double ypos) {
  AppFromWindow(window)->OnMouseMove(xpos, ypos);
}

void App::OnGlfwSetMouseButton(GLFWwindow* window, int button, int action, int mods) {
  AppFromWindow(window)->OnMouseButton(button, action, mods);
}

void App::OnGlfwScroll(GLFWwindow* window, double x_offset, double y_offset) {
  AppFromWindow(window)->OnScroll(x_offset, y_offset);
}

}  // namespace web_gpu_app