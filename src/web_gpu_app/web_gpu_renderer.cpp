#include "web_gpu_app/web_gpu_renderer.h"

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <webgpu/webgpu_cpp.h>

#include <filesystem>
#include <iostream>

#include "web_gpu_app/utils.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

struct GLFWwindow;

namespace web_gpu_app {

GLFWwindow* WebGpuRenderer::g_window_;
std::function<void(std::unique_ptr<WebGpuRenderer>)> WebGpuRenderer::g_create_callback_;
wgpu::Instance WebGpuRenderer::g_instance_;

namespace {
void OnDeviceError(WGPUErrorType type, const char* message, void* userdata) {
  std::cout << "Dawn error: " << message << std::endl;
  exit(0);
}

void OnDeviceLost(WGPUDeviceLostReason reason, char const* message, void* userdata) {
  std::cout << "Dawn device lost: " << message << std::endl;
  std::cout << "Reason: " << reason << std::endl;
  std::cout << "Device: " << userdata << std::endl;
}

const char* shader_code = R"(
@vertex 
fn vertex_main(@builtin(vertex_index) i : u32) ->
    @builtin(position) vec4f {
    const pos = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
    return vec4f(pos[i], 0, 1);
}
@fragment 
fn fragment_main() -> @location(0) vec4f {
    return vec4f(0.1, 0.4, 0, 1);
}
)";

}  // namespace

void GetDevice(wgpu::Instance instance, void (*callback)(wgpu::Device)) {
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
              wgpu::Device device = wgpu::Device::Acquire(c_device);
              reinterpret_cast<void (*)(wgpu::Device)>(userdata)(device);
            },
            userdata);
      },
      reinterpret_cast<void*>(callback));
}

void WebGpuRenderer::Create(GLFWwindow* window,
                            std::function<void(std::unique_ptr<WebGpuRenderer>)> callback) {
  g_window_ = window;
  g_create_callback_ = std::move(callback);
  g_instance_ = wgpu::CreateInstance();
  web_gpu_app::GetDevice(g_instance_, [](wgpu::Device device) {
    g_create_callback_(std::make_unique<WebGpuRenderer>(g_instance_, device, g_window_));
  });
}

WebGpuRenderer::WebGpuRenderer(wgpu::Instance instance, wgpu::Device device, GLFWwindow* window)
    : instance_(instance), device_(device), window_(window) {
#if !defined(__EMSCRIPTEN__)
  device_.SetUncapturedErrorCallback(OnDeviceError, nullptr);
  device_.SetDeviceLostCallback(OnDeviceLost, device.Get());
#endif

  shader_code_ = shader_code;
  glfwGetFramebufferSize(window_, &width_, &height_);
  surface_ = CreateSurface(instance_, window);
  swap_chain_ = CreateSwapChain(surface_, device_, width_, height_);
  depth_texture_ = CreateDepthTexture(device_, depth_texture_format_, width_, height_);
  depth_texture_view_ = CreateDepthTextureView(depth_texture_, depth_texture_format_);
  render_pipeline_ = CreateRenderPipeline(device_, shader_code_.c_str());
  ui_ = std::make_unique<Ui>(window_, device_);
}

WebGpuRenderer::~WebGpuRenderer() {}

wgpu::Surface WebGpuRenderer::CreateSurface(const wgpu::Instance& instance, GLFWwindow* window) {
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

wgpu::SwapChain WebGpuRenderer::CreateSwapChain(wgpu::Surface surface, wgpu::Device device,
                                                uint32_t width, uint32_t height) {
  wgpu::SwapChainDescriptor descriptor{.usage = wgpu::TextureUsage::RenderAttachment,
                                       .format = wgpu::TextureFormat::BGRA8Unorm,
                                       .width = width,
                                       .height = height,
                                       .presentMode = wgpu::PresentMode::Fifo};
  return device.CreateSwapChain(surface, &descriptor);
}

wgpu::Texture WebGpuRenderer::CreateDepthTexture(wgpu::Device device,
                                                 wgpu::TextureFormat depth_texture_format,
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

wgpu::TextureView WebGpuRenderer::CreateDepthTextureView(wgpu::Texture depth_texture,
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

wgpu::RenderPipeline WebGpuRenderer::CreateRenderPipeline(wgpu::Device device,
                                                          const char* shader_code) {
  wgpu::ShaderModuleWGSLDescriptor wgsl_descriptor{};
  wgsl_descriptor.code = shader_code;

  wgpu::ShaderModuleDescriptor shader_module_descriptor{.nextInChain = &wgsl_descriptor};
  wgpu::ShaderModule shader_module = device.CreateShaderModule(&shader_module_descriptor);

  wgpu::ColorTargetState color_target_state{.format = wgpu::TextureFormat::BGRA8Unorm};

  wgpu::FragmentState fragmentState{.module = shader_module,
                                    .entryPoint = "fragment_main",
                                    .targetCount = 1,
                                    .targets = &color_target_state};

  wgpu::DepthStencilState depth_stencil_state;
  depth_stencil_state.depthCompare = wgpu::CompareFunction::Less;
  depth_stencil_state.depthWriteEnabled = true;
  depth_stencil_state.format = wgpu::TextureFormat::Depth24Plus;
  depth_stencil_state.stencilReadMask = 0;
  depth_stencil_state.stencilWriteMask = 0;

  wgpu::RenderPipelineDescriptor descriptor{
      .vertex = {.module = shader_module, .entryPoint = "vertex_main"}, .fragment = &fragmentState};

  descriptor.depthStencil = &depth_stencil_state;
  descriptor.multisample.count = 1;
  descriptor.multisample.mask = ~0u;
  descriptor.multisample.alphaToCoverageEnabled = false;

  return device.CreateRenderPipeline(&descriptor);
}

void WebGpuRenderer::BeginFrame() { ui_->BeginUiFrame(); }

void WebGpuRenderer::EndFrame(const Renderables&) {
  wgpu::RenderPassColorAttachment attachment{.view = swap_chain_.GetCurrentTextureView(),
                                             .loadOp = wgpu::LoadOp::Clear,
                                             .storeOp = wgpu::StoreOp::Store};

  wgpu::RenderPassDepthStencilAttachment depthStencilAttachment;
  depthStencilAttachment.view = depth_texture_view_;
  depthStencilAttachment.depthClearValue = 1.0f;
  depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
  depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Store;
  depthStencilAttachment.depthReadOnly = false;
  depthStencilAttachment.stencilClearValue = 0;
#ifdef WEBGPU_BACKEND_WGPU
  depthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Clear;
  depthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Store;
#else
  depthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Undefined;
  depthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Undefined;
#endif
  depthStencilAttachment.stencilReadOnly = true;

  wgpu::RenderPassDescriptor renderpass{.colorAttachmentCount = 1,
                                        .colorAttachments = &attachment,
                                        .depthStencilAttachment = &depthStencilAttachment};

  wgpu::CommandEncoder encoder = device_.CreateCommandEncoder();
  wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass);
  pass.SetPipeline(render_pipeline_);
  pass.Draw(3);

  ui_->EndUiFrame(pass);
  pass.End();
  wgpu::CommandBuffer commands = encoder.Finish();
  device_.GetQueue().Submit(1, &commands);

#if !defined(__EMSCRIPTEN__)
  device_.Tick();
  swap_chain_.Present();
#endif
}

void WebGpuRenderer::OnResize(int width, int height) {
  width_ = width;
  height_ = height;
  swap_chain_ = CreateSwapChain(surface_, device_, width_, height_);
  depth_texture_ = CreateDepthTexture(device_, depth_texture_format_, width_, height_);
  depth_texture_view_ = CreateDepthTextureView(depth_texture_, depth_texture_format_);
}

void* WebGpuRenderer::GetWindow() const { return window_; }

}  // namespace web_gpu_app