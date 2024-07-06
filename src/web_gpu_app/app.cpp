#include "web_gpu_app/app.h"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "web_gpu_app/utils.h"
#include "web_gpu_app/web_gpu_renderer.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

namespace {

web_gpu_app::App* AppFromWindow(GLFWwindow* window) {
  return reinterpret_cast<web_gpu_app::App*>(glfwGetWindowUserPointer(window));
}

}  // namespace

namespace web_gpu_app {

App::App() {
  CanvasSize canvas_size = GetInitialCanvasSize();
  window_ = CreateGlfwWindow("", canvas_size.width, canvas_size.height, this);
}

App::~App() {}

void App::Run() {
#if defined(__EMSCRIPTEN__)
  emscripten_set_main_loop_arg(EmscriptenMainLoop, reinterpret_cast<void*>(this), 0, false);
  emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, glfwGetCurrentContext(), false,
                                 EmscriptenCanvasSizeChanged);
#else
  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
    Render();
  }
#endif
}

void App::Render() {
  glfwSetWindowTitle(window_, GetTitle());
  Renderer* renderer = GetRenderer();
  renderer->BeginFrame();
  Renderables renderables = Update();
  renderer->EndFrame(renderables);
}

GLFWwindow* App::CreateGlfwWindow(const char* title, int width, int height, void* user_pointer) {
  if (!glfwInit()) {
    return nullptr;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

  GLFWwindow* window =
      glfwCreateWindow(width, height, title, /*monitor*/ nullptr, /*share*/ nullptr);

  glfwSetWindowUserPointer(window, user_pointer);
  glfwSetFramebufferSizeCallback(window, &OnGlfwResize);
  glfwSetWindowSizeCallback(window, &OnGlfwResize);
  glfwSetCursorPosCallback(window, &OnGlfwSetCursorPos);
  glfwSetMouseButtonCallback(window, &OnGlfwSetMouseButton);
  glfwSetScrollCallback(window, OnGlfwScroll);
  return window;
}

void App::OnResize(int width, int height) { 
    GetRenderer()->OnResize(width, height); 
    Render();
}
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

#if defined(__EMSCRIPTEN__)
void App::EmscriptenMainLoop(void* app) { reinterpret_cast<App*>(app)->Render(); }

int App::EmscriptenCanvasSizeChanged(int event_type, const EmscriptenUiEvent* ui_event,
                                     void* user_data) {
  CanvasSize canvas_size = GetCanvasSize();
  glfwSetWindowSize(glfwGetCurrentContext(), canvas_size.width, canvas_size.height);
  return 1;
}

CanvasSize App::GetCanvasSize() {
  double canvas_width = 0;
  double canvas_height = 0;
  emscripten_get_element_css_size("#canvas", &canvas_width, &canvas_height);
  return {static_cast<int>(canvas_width), static_cast<int>(canvas_height)};
}

CanvasSize App::GetInitialCanvasSize() const { return GetCanvasSize(); }

#else

CanvasSize App::GetInitialCanvasSize() const {
  static constexpr int kInitialWidth = 600;
  static constexpr int kInitialHeight = 400;
  return {kInitialWidth, kInitialHeight};
}

#endif  // defined(__EMSCRIPTEN__)

}  // namespace web_gpu_app