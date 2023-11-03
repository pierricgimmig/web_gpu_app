#include "web_gpu_app.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_wgpu.h"
#include "imgui/backends/imgui_impl_glfw.h"

#include <GLFW/glfw3.h>
#include <iostream>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

namespace {

bool InitGui(GLFWwindow* window, wgpu::Device device) {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOther(window, true);
	ImGui_ImplWGPU_Init(device.Get(), 3, WGPUTextureFormat_BGRA8Unorm, WGPUTextureFormat_Depth24Plus);
	return true;
}

void TerminateGui() {
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplWGPU_Shutdown();
}

void UpdateGui(wgpu::RenderPassEncoder renderPass) {
	// Start the Dear ImGui frame
	ImGui_ImplWGPU_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Build our UI
	{
		static float f = 0.0f;
		static int counter = 0;
		static bool show_demo_window = true;
		static bool show_another_window = false;
		static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		ImGui::Begin("Hello, world!");                                // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");                     // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);            // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);                  // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color);       // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                                  // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	// Draw the UI
	ImGui::EndFrame();
	// Convert the UI defined above into low-level drawing commands
	ImGui::Render();
	// Execute the low-level drawing commands on the WebGPU backend
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass.Get());
}

void OnDeviceError(WGPUErrorType type, const char* message, void* userdata) {
    std::cout << "Dawn error: " << message << std::endl;
    exit(0);
}

void OnDeviceLost(WGPUDeviceLostReason reason, char const * message, void * userdata) {
  std::cout << "Dawn device lost: " << message << std::endl;
  std::cout << "Reason: " << reason << std::endl;
  std::cout << "Device: " << userdata << std::endl;
}

}  // namespace

namespace web_gpu_app {

namespace {
#if defined(__EMSCRIPTEN__)
void EmscriptenMainLoop(void* app) { reinterpret_cast<App*>(app)->Render(); }
#endif
}  // namespace

static constexpr uint32_t kDefaultWidth = 3024;
static constexpr uint32_t kDefaultHeight = 1708;

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

App::~App() {
  TerminateGui();
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

void App::OnDevice(wgpu::Device device) {
  device_ = device;
  device.SetUncapturedErrorCallback(OnDeviceError, nullptr);
  device.SetDeviceLostCallback(OnDeviceLost, device.Get());
}

void App::SetupSwapChain(wgpu::Surface surface) {
  wgpu::SwapChainDescriptor descriptor{.usage = wgpu::TextureUsage::RenderAttachment,
                                   .format = wgpu::TextureFormat::BGRA8Unorm,
                                   .width = width_,
                                   .height = height_,
                                   .presentMode = wgpu::PresentMode::Fifo};
  swap_chain_ = device_.CreateSwapChain(surface, &descriptor);
}

bool App::InitDepthBuffer() {
	// Get the current size of the window's framebuffer:
	int width, height;
	glfwGetFramebufferSize(window_, &width, &height);

	// Create the depth texture
	wgpu::TextureDescriptor depthTextureDesc;
	depthTextureDesc.dimension = wgpu::TextureDimension::e2D;
	depthTextureDesc.format = depth_texture_format_;
	depthTextureDesc.mipLevelCount = 1;
	depthTextureDesc.sampleCount = 1;
	depthTextureDesc.size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 };
	depthTextureDesc.usage = wgpu::TextureUsage::RenderAttachment;
	depthTextureDesc.viewFormatCount = 1;
	depthTextureDesc.viewFormats = &depth_texture_format_;
	depth_texture_ = device_.CreateTexture(&depthTextureDesc);
	std::cout << "Depth texture: " << depth_texture_.Get() << std::endl;

	// Create the view of the depth texture manipulated by the rasterizer
	wgpu::TextureViewDescriptor depthTextureViewDesc;
	depthTextureViewDesc.aspect = wgpu::TextureAspect::DepthOnly;
	depthTextureViewDesc.baseArrayLayer = 0;
	depthTextureViewDesc.arrayLayerCount = 1;
	depthTextureViewDesc.baseMipLevel = 0;
	depthTextureViewDesc.mipLevelCount = 1;
	depthTextureViewDesc.dimension = wgpu::TextureViewDimension::e2D;
	depthTextureViewDesc.format = depth_texture_format_;
	depth_texture_view_ = depth_texture_.CreateView(&depthTextureViewDesc);
	std::cout << "Depth texture view: " << depth_texture_view_.Get() << std::endl;

	return depth_texture_view_ != nullptr;
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

  pipeline_ = device_.CreateRenderPipeline(&descriptor);
}

void App::MainLoop() {
  if (!glfwInit()) {
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = glfwCreateWindow(width_, height_, "WebGPU window", nullptr, nullptr);

  if(!InitGui(window_, device_)){
    std::cout << "Failed to initialize Dear ImGui!" << std::endl;
  }

#if defined(__EMSCRIPTEN__)
  wgpu::SurfaceDescriptorFromCanvasHTMLSelector canvasDesc{};
  canvasDesc.selector = "#canvas";

  wgpu::SurfaceDescriptor surfaceDesc{.nextInChain = &canvasDesc};
  wgpu::Surface surface = instance.CreateSurface(&surfaceDesc);
#else
  wgpu::Surface surface = wgpu::glfw::CreateSurfaceForWindow(instance_, window_);
#endif

  SetupSwapChain(surface);
  InitDepthBuffer();
  CreateRenderPipeline();

#if defined(__EMSCRIPTEN__)
  emscripten_set_main_loop_arg(reinterpret_cast<void*>(this), EmscriptenMainLoop, 0, false);
#else
  while (!glfwWindowShouldClose(window_)) {
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
  pass.SetPipeline(pipeline_);
  pass.Draw(3);
  UpdateGui(pass);
  pass.End();
  wgpu::CommandBuffer commands = encoder.Finish();
  device_.GetQueue().Submit(1, &commands);
  device_.Tick();
}

}  // namespace web_gpu_app