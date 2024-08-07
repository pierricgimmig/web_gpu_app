#pragma once

#include <webgpu/webgpu_cpp.h>

#include <memory>

#include "web_gpu_app/renderer.h"
#include "web_gpu_app/ui.h"

struct GLFWwindow;

namespace web_gpu_app {

void GetDevice(wgpu::Instance instance, void (*callback)(wgpu::Device));

class WebGpuRenderer : public Renderer {
 public:
  static void Create(GLFWwindow* window,
                     std::function<void(std::unique_ptr<WebGpuRenderer>)> callback);

  WebGpuRenderer(wgpu::Instance instance, wgpu::Device device, GLFWwindow* window);
  virtual ~WebGpuRenderer();

  void BeginFrame() override;
  void EndFrame(const Renderables& renderables) override;
  void OnResize(int width, int height) override;
  void* GetWindow() const override;

 protected:  
  virtual wgpu::Surface CreateSurface(const wgpu::Instance& instance, GLFWwindow* window);
  virtual wgpu::SwapChain CreateSwapChain(wgpu::Surface surface, wgpu::Device device,
                                          uint32_t width, uint32_t height);
  virtual wgpu::Texture CreateDepthTexture(wgpu::Device device,
                                           wgpu::TextureFormat depth_texture_format, uint32_t width,
                                           uint32_t height);
  virtual wgpu::TextureView CreateDepthTextureView(wgpu::Texture depth_texture,
                                                   wgpu::TextureFormat depth_texture_format);
  virtual wgpu::RenderPipeline CreateRenderPipeline(wgpu::Device device, const char* shader_code);

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
  std::unique_ptr<Ui> ui_;

  static GLFWwindow* g_window_;
  static std::function<void(std::unique_ptr<WebGpuRenderer>)> g_create_callback_;
  static wgpu::Instance g_instance_;
};

}  // namespace web_gpu_app