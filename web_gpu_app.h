#pragma once

#include <memory>

#include "web_gpu_renderer.h"

namespace web_gpu_app {

class App {
 public:
  App();
  virtual ~App();
  void Run();
  virtual const char* GetAppName() { return "web_gpu_app"; }

 protected:
  void SetupUi();
  void MainLoop();

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

  GLFWwindow* window_ = nullptr;
  std::unique_ptr<WebGpuRenderer> renderer_;
};

}  // namespace web_gpu_app
