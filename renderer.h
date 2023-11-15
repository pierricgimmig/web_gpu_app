#pragma once

#include <webgpu/webgpu_cpp.h>

struct GLFWwindow;

namespace web_gpu_app {

struct Renderer {
  Renderer() = default;
  virtual ~Renderer() = default;
  virtual GLFWwindow* GetGlfwWindow(const char* title, void* user_pointer);
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
}

};

}  // namespace web_gpu_app