#pragma once

#include <webgpu/webgpu_cpp.h>

#include <functional>
#include <vector>

struct GLFWwindow;

namespace web_gpu_app {

class Ui {
 public:
  Ui(GLFWwindow* window, wgpu::Device device);
  ~Ui();

  void BeginUiFrame();
  void EndUiFrame(wgpu::RenderPassEncoder render_pass);

  void SetThemeDark();
  void SetThemeDarker();

 private:
  std::function<void()> callback_;
};

}  // namespace web_gpu_app