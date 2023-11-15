#pragma once

#include <webgpu/webgpu_cpp.h>

#include <string>

#include "imgui.h"

struct GLFWwindow;

namespace web_gpu_app {

class App {
 public:
  App();
  virtual ~App();

 protected:
  void SetupUi();

  void MainLoop();
  void Render();
  virtual void RenderUi(wgpu::RenderPassEncoder render_pass);

  virtual std::string GetAppName() { return "web_gpu_app"; };

  void OnResize(int width, int height);
  void OnMouseMove(double xpos, double ypos);
  void OnMouseButton(int button, int action, int mods);
  void OnScroll(double xoffset, double yoffset);

  wgpu::Instance instance_;
  wgpu::Device device_;
  wgpu::Surface surface_;
  wgpu::SwapChain swap_chain_;
  wgpu::RenderPipeline render_pipeline_;
  wgpu::TextureFormat depth_texture_format_ = wgpu::TextureFormat::Depth24Plus;
  wgpu::Texture depth_texture_ = nullptr;
  wgpu::TextureView depth_texture_view_ = nullptr;
  GLFWwindow* window_ = nullptr;
  int width_ = 0;
  int height_ = 0;
};

}  // namespace web_gpu_app