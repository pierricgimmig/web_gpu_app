#pragma once

#include <webgpu/webgpu_cpp.h>

#include <functional>
#include <vector>

struct GLFWwindow;

namespace web_gpu_app {

class Ui {
 public:
  Ui(GLFWwindow* window, wgpu::Device device, std::function<void()> callback);
  ~Ui();

  void Render(wgpu::RenderPassEncoder render_pass);

 private:
  std::function<void()> callback_;
};

}  // namespace web_gpu_app