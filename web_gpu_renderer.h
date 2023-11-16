#pragma once

#include <webgpu/webgpu_cpp.h>

#include "renderer.h"

struct GLFWwindow;

namespace web_gpu_app {

class WebGpuRenderer : public Renderer {
 public:
  WebGpuRenderer(GLFWwindow* window);
  virtual ~WebGpuRenderer() {}

  void Render(const Renderables& renderables) override;
  void OnResize(int width, int height) override;

  WGPUDevice GetDevice() const { return device_.Get(); }

 protected:
  virtual wgpu::Device GetDevice(const wgpu::Instance& instance);
  virtual wgpu::Surface GetSurface(const wgpu::Instance& instance, GLFWwindow* window);
  virtual wgpu::SwapChain GetSwapChain(wgpu::Surface surface, wgpu::Device device, uint32_t width,
                                       uint32_t height);
  virtual wgpu::Texture GetDepthTexture(wgpu::Device device,
                                        wgpu::TextureFormat depth_texture_format, uint32_t width,
                                        uint32_t height);
  virtual wgpu::TextureView GetDepthTextureView(wgpu::Texture depth_texture,
                                                wgpu::TextureFormat depth_texture_format);
  virtual wgpu::RenderPipeline GetRenderPipeline(wgpu::Device device, const char* shader_code);

  virtual void RenderUi(wgpu::RenderPassEncoder render_pass);

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
  std::string shader_code_;
};

}  // namespace web_gpu_app