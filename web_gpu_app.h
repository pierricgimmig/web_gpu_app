#pragma once

#include <webgpu/webgpu_cpp.h>
#include "imgui.h"

#include <string>

struct GLFWwindow;

namespace web_gpu_app {

class App {
 public:
  App();
  virtual ~App();

 protected:
  void SetupGlfw();
  void SetupUi();
  void SetupDevice();
  void SetupSurface();
  void SetupSwapChain(wgpu::Surface surface);
  bool SetupDepthBuffer();
  void SetupRenderPipeline();
  
  void MainLoop();
  void Render();
  
  virtual std::string GetAppName() { return "web_gpu_app"; };

  void OnResize(int width, int height);
  void OnMouseMove(double xpos, double ypos);
  void OnMouseButton(int button, int action, int mods);
  void OnScroll(double xoffset, double yoffset);

  // IO.
  static void OnGlfwResize(GLFWwindow* window, int width, int height);
  static void OnGlfwSetCursorPos(GLFWwindow* window, double xpos, double ypos);
  static void OnGlfwSetMouseButton(GLFWwindow* window, int button, int action, int mods);
  static void OnGlfwScroll(GLFWwindow* window, double xoffset, double yoffset);

  wgpu::Instance instance_;
  wgpu::Device device_;
  wgpu::Surface surface_;
  wgpu::SwapChain swap_chain_;
  wgpu::RenderPipeline pipeline_;
  wgpu::TextureFormat depth_texture_format_ = wgpu::TextureFormat::Depth24Plus;
  wgpu::Texture depth_texture_ = nullptr;
  wgpu::TextureView depth_texture_view_ = nullptr;

  GLFWwindow* window_ = nullptr;
  int width_ = 0;
  int height_ = 0;
};

}  // namespace web_gpu_app