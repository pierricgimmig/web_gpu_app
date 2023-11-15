#pragma once

#include <webgpu/webgpu_cpp.h>

struct GLFWwindow;

namespace web_gpu_app {

wgpu::Device Renderer::GetDevice(const wgpu::Instance& instance) {
  wgpu::Device result;
  instance.RequestAdapter(
      nullptr,
      [](WGPURequestAdapterStatus status, WGPUAdapter c_adapter, const char* message,
         void* userdata) {
        if (status != WGPURequestAdapterStatus_Success) {
          exit(0);
        }
        wgpu::Adapter adapter = wgpu::Adapter::Acquire(c_adapter);
        adapter.RequestDevice(
            nullptr,
            [](WGPURequestDeviceStatus status, WGPUDevice c_device, const char* message,
               void* userdata) {
              wgpu::Device* device = reinterpret_cast<wgpu::Device*>(userdata);
              *device = wgpu::Device::Acquire(c_device);
              device->SetUncapturedErrorCallback(OnDeviceError, nullptr);
              device->SetDeviceLostCallback(OnDeviceLost, device->Get());
            },
            userdata);
      },
      reinterpret_cast<void*>(&result));
  return result;
}

wgpu::Surface Renderer::GetSurface(const wgpu::Instance& instance, GLFWwindow* window) {
  wgpu::Surface surface;
#if defined(__EMSCRIPTEN__)
  wgpu::SurfaceDescriptorFromCanvasHTMLSelector canvasDesc{};
  canvasDesc.selector = "#canvas";
  wgpu::SurfaceDescriptor surfaceDesc{.nextInChain = &canvasDesc};
  surface = instance.CreateSurface(&surfaceDesc);
#else
  surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);
#endif
  return surface;
}

wgpu::SwapChain Renderer::GetSwapChain(wgpu::Surface surface, wgpu::Device device, uint32_t width,
                                    uint32_t height) {
  wgpu::SwapChainDescriptor descriptor{.usage = wgpu::TextureUsage::RenderAttachment,
                                       .format = wgpu::TextureFormat::BGRA8Unorm,
                                       .width = width,
                                       .height = height,
                                       .presentMode = wgpu::PresentMode::Fifo};
  return device.CreateSwapChain(surface, &descriptor);
}

wgpu::Texture Renderer::GetDepthTexture(wgpu::Device device, wgpu::TextureFormat depth_texture_format,
                                     uint32_t width, uint32_t height) {
  wgpu::TextureDescriptor depthTextureDesc;
  depthTextureDesc.dimension = wgpu::TextureDimension::e2D;
  depthTextureDesc.format = depth_texture_format;
  depthTextureDesc.mipLevelCount = 1;
  depthTextureDesc.sampleCount = 1;
  depthTextureDesc.size = {static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1};
  depthTextureDesc.usage = wgpu::TextureUsage::RenderAttachment;
  depthTextureDesc.viewFormatCount = 1;
  depthTextureDesc.viewFormats = &depth_texture_format;
  return device.CreateTexture(&depthTextureDesc);
}

wgpu::TextureView Renderer::GetDepthTextureView(wgpu::Texture depth_texture,
                                             wgpu::TextureFormat depth_texture_format) {
  wgpu::TextureViewDescriptor depth_texture_view_descriptor;
  depth_texture_view_descriptor.aspect = wgpu::TextureAspect::DepthOnly;
  depth_texture_view_descriptor.baseArrayLayer = 0;
  depth_texture_view_descriptor.arrayLayerCount = 1;
  depth_texture_view_descriptor.baseMipLevel = 0;
  depth_texture_view_descriptor.mipLevelCount = 1;
  depth_texture_view_descriptor.dimension = wgpu::TextureViewDimension::e2D;
  depth_texture_view_descriptor.format = depth_texture_format;
  return depth_texture.CreateView(&depth_texture_view_descriptor);
}

wgpu::RenderPipeline Renderer::GetRenderPipeline(wgpu::Device device, const char* shader_code) {
  wgpu::ShaderModuleWGSLDescriptor wgsl_descriptor{};
  wgsl_descriptor.code = shader_code;

  wgpu::ShaderModuleDescriptor shader_module_descriptor{.nextInChain = &wgsl_descriptor};
  wgpu::ShaderModule shader_module = device.CreateShaderModule(&shader_module_descriptor);

  wgpu::ColorTargetState color_target_state{.format = wgpu::TextureFormat::BGRA8Unorm};

  wgpu::FragmentState fragmentState{.module = shader_module,
                                    .entryPoint = "fragmentMain",
                                    .targetCount = 1,
                                    .targets = &color_target_state};

  wgpu::DepthStencilState depth_stencil_state;
  depth_stencil_state.depthCompare = wgpu::CompareFunction::Less;
  depth_stencil_state.depthWriteEnabled = true;
  depth_stencil_state.format = wgpu::TextureFormat::Depth24Plus;
  depth_stencil_state.stencilReadMask = 0;
  depth_stencil_state.stencilWriteMask = 0;

  wgpu::RenderPipelineDescriptor descriptor{
      .vertex = {.module = shader_module, .entryPoint = "vertexMain"}, .fragment = &fragmentState};

  descriptor.depthStencil = &depth_stencil_state;
  descriptor.multisample.count = 1;
  descriptor.multisample.mask = ~0u;
  descriptor.multisample.alphaToCoverageEnabled = false;

  return device.CreateRenderPipeline(&descriptor);
}

}  // namespace web_gpu_app