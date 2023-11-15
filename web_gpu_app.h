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
  static GLFWwindow* SetupGlfwWindow(const char* title, void* user_pointer);
  static wgpu::Device SetupDevice(const wgpu::Instance& instance);
  static wgpu::Surface SetupSurface(const wgpu::Instance& instance, GLFWwindow* window);
  static wgpu::SwapChain SetupSwapChain(wgpu::Surface surface, wgpu::Device device, uint32_t width,
                                        uint32_t height);
  static wgpu::Texture SetupDepthTexture(wgpu::Device device,
                                         wgpu::TextureFormat depth_texture_format, uint32_t width,
                                         uint32_t height);
  static wgpu::TextureView SetupDepthTextureView(wgpu::Texture depth_texture,
                                                 wgpu::TextureFormat depth_texture_format);
  static wgpu::RenderPipeline SetupRenderPipeline(wgpu::Device device, const char* shader_code);
  void SetupUi();

  void MainLoop();
  void Render();
  virtual void RenderUi(wgpu::RenderPassEncoder render_pass);

  virtual std::string GetAppName() { return "web_gpu_app"; };

  void OnResize(int width, int height);
  void OnMouseMove(double xpos, double ypos);
  void OnMouseButton(int button, int action, int mods);
  void OnScroll(double xoffset, double yoffset);

  static void OnGlfwResize(GLFWwindow* window, int width, int height);
  static void OnGlfwSetCursorPos(GLFWwindow* window, double xpos, double ypos);
  static void OnGlfwSetMouseButton(GLFWwindow* window, int button, int action, int mods);
  static void OnGlfwScroll(GLFWwindow* window, double xoffset, double yoffset);

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