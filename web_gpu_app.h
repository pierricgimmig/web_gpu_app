#pragma once

#include <memory>
#include <string>

#include "web_gpu_renderer.h"

namespace web_gpu_app {

class App {
 public:
  App();
  virtual ~App();

  void OnResize(int width, int height);
  void OnMouseMove(double xpos, double ypos);
  void OnMouseButton(int button, int action, int mods);
  void OnScroll(double xoffset, double yoffset);

 protected:
  GLFWwindow* SetupGlfwWindow(const char* title, void* user_pointer);
  void SetupUi();
  void MainLoop();

  virtual std::string GetAppName() { return "web_gpu_app"; };

  GLFWwindow* window_ = nullptr;
  std::unique_ptr<WebGpuRenderer> renderer_;
};

}  // namespace web_gpu_app