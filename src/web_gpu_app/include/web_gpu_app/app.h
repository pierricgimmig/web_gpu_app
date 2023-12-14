#pragma once

#include <memory>

#include "renderer.h"

struct GLFWwindow;
struct EmscriptenUiEvent;

namespace web_gpu_app {

class App {
 public:
  App();
  virtual ~App();
  virtual void Run();

  virtual const char* GetTitle() = 0;
  virtual Renderer* GetRenderer() = 0;
  virtual Renderables Update() = 0;

 protected:
  virtual void Render();
  virtual void OnResize(int width, int height);
  virtual void OnMouseMove(double xpos, double ypos);
  virtual void OnMouseButton(int button, int action, int mods);
  virtual void OnScroll(double xoffset, double yoffset);

  static GLFWwindow* CreateGlfwWindow(const char* title, void* user_pointer);
  static void OnGlfwResize(GLFWwindow* window, int width, int height);
  static void OnGlfwSetCursorPos(GLFWwindow* window, double xpos, double ypos);
  static void OnGlfwSetMouseButton(GLFWwindow* window, int button, int action, int mods);
  static void OnGlfwScroll(GLFWwindow* window, double x_offset, double y_offset);

#if defined(__EMSCRIPTEN__)
  static void EmscriptenMainLoop(void* app);
  static int EmscriptenCanvasSizeChanged(int event_type, const EmscriptenUiEvent* ui_event,
                                         void* user_data);
#endif

  GLFWwindow* window_ = nullptr;
};

}  // namespace web_gpu_app
