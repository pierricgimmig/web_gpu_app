#include "web_gpu_app.h"

#include <GLFW/glfw3.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

namespace web_gpu_app {

namespace {
#if defined(__EMSCRIPTEN__)
void EmscriptenMainLoop(void* app) { reinterpret_cast<App*>(app)->Render(); }
#endif
}  // namespace

static constexpr uint32_t kDefaultWidth = 1024;
static constexpr uint32_t kDefaultHeight = 512;

const char shaderCode[] = R"(
    @vertex fn vertexMain(@builtin(vertex_index) i : u32) ->
      @builtin(position) vec4f {
        const pos = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
        return vec4f(pos[i], 0, 1);
    }
    @fragment fn fragmentMain() -> @location(0) vec4f {
        return vec4f(0.1, 0.4, 0, 1);
    }
)";

App::App() : width_(kDefaultWidth), height_(kDefaultHeight) {
  instance_ = wgpu::CreateInstance();
  GetDevice();
  MainLoop();
}

void App::GetDevice() {
  instance_.RequestAdapter(
      nullptr,
      [](WGPURequestAdapterStatus status, WGPUAdapter cAdapter, const char* message,
         void* userdata) {
        if (status != WGPURequestAdapterStatus_Success) {
          exit(0);
        }
        wgpu::Adapter adapter = wgpu::Adapter::Acquire(cAdapter);
        adapter.RequestDevice(
            nullptr,
            [](WGPURequestDeviceStatus status, WGPUDevice cDevice, const char* message,
               void* userdata) {
              wgpu::Device device = wgpu::Device::Acquire(cDevice);
              App* app = reinterpret_cast<App*>(userdata);
              app->OnDevice(device);
            },
            userdata);
      },
      reinterpret_cast<void*>(this));
}

void App::OnDevice(wgpu::Device device) { device_ = device; }

void App::SetupSwapChain(wgpu::Surface surface) {
  wgpu::SwapChainDescriptor scDesc{.usage = wgpu::TextureUsage::RenderAttachment,
                                   .format = wgpu::TextureFormat::BGRA8Unorm,
                                   .width = width_,
                                   .height = height_,
                                   .presentMode = wgpu::PresentMode::Fifo};
  swap_chain_ = device_.CreateSwapChain(surface, &scDesc);
}

void App::CreateRenderPipeline() {
  wgpu::ShaderModuleWGSLDescriptor wgsl_descriptor{};
  wgsl_descriptor.code = shaderCode;

  wgpu::ShaderModuleDescriptor shader_module_descriptor{.nextInChain = &wgsl_descriptor};
  wgpu::ShaderModule shader_module = device_.CreateShaderModule(&shader_module_descriptor);

  wgpu::ColorTargetState color_target_state{.format = wgpu::TextureFormat::BGRA8Unorm};

  wgpu::FragmentState fragmentState{.module = shader_module,
                                    .entryPoint = "fragmentMain",
                                    .targetCount = 1,
                                    .targets = &color_target_state};

  wgpu::RenderPipelineDescriptor descriptor{
      .vertex = {.module = shader_module, .entryPoint = "vertexMain"}, .fragment = &fragmentState};
  pipeline_ = device_.CreateRenderPipeline(&descriptor);
}

void App::MainLoop() {
  if (!glfwInit()) {
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(width_, height_, "WebGPU window", nullptr, nullptr);

#if defined(__EMSCRIPTEN__)
  wgpu::SurfaceDescriptorFromCanvasHTMLSelector canvasDesc{};
  canvasDesc.selector = "#canvas";

  wgpu::SurfaceDescriptor surfaceDesc{.nextInChain = &canvasDesc};
  wgpu::Surface surface = instance.CreateSurface(&surfaceDesc);
#else
  wgpu::Surface surface = wgpu::glfw::CreateSurfaceForWindow(instance_, window);
#endif

  SetupSwapChain(surface);
  CreateRenderPipeline();

#if defined(__EMSCRIPTEN__)
  emscripten_set_main_loop_arg(reinterpret_cast<void*>(this), EmscriptenMainLoop, 0, false);
#else
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    Render();
    swap_chain_.Present();
  }
#endif
}

void App::Render() {
  wgpu::RenderPassColorAttachment attachment{.view = swap_chain_.GetCurrentTextureView(),
                                             .loadOp = wgpu::LoadOp::Clear,
                                             .storeOp = wgpu::StoreOp::Store};

  wgpu::RenderPassDescriptor renderpass{.colorAttachmentCount = 1, .colorAttachments = &attachment};

  wgpu::CommandEncoder encoder = device_.CreateCommandEncoder();
  wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass);
  pass.SetPipeline(pipeline_);
  pass.Draw(3);
  pass.End();
  wgpu::CommandBuffer commands = encoder.Finish();
  device_.GetQueue().Submit(1, &commands);
}

}  // namespace web_gpu_app