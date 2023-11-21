#pragma once

#include <memory>

#include "web_gpu_app/app.h"
#include "web_gpu_app/web_gpu_renderer.h"

struct GLFWwindow;

namespace web_gpu_app {

class TriangleApp : public App {
 public:
  TriangleApp();
  virtual ~TriangleApp();

  const char* GetTitle() override { return "Triangle App"; }
  Renderer* GetRenderer() override { return renderer_.get(); }
  Renderables Update() override;

 protected:
  std::unique_ptr<WebGpuRenderer> renderer_;
};

}  // namespace web_gpu_app
