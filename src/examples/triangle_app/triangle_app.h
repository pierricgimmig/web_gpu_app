#pragma once

#include <memory>

#include "web_gpu_app/app.h"
#include "web_gpu_app/web_gpu_renderer.h"

struct GLFWwindow;

namespace web_gpu_app {

class TriangleApp : public App {
 public:
  explicit TriangleApp(wgpu::Instance instance, wgpu::Device device);
  virtual ~TriangleApp();

  const char* GetTitle() override;
  Renderer* GetRenderer() override;
  Renderables Update() override;

 protected:
  std::unique_ptr<WebGpuRenderer> renderer_;
};

}  // namespace web_gpu_app
