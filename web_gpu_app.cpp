#include "web_gpu_app.h"

#include <GLFW/glfw3.h>

#include <iostream>

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_wgpu.h"
#include "imgui/imgui.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

namespace {

void SetGuiTheme() {
  ImGuiStyle& style = ImGui::GetStyle();
  style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
  style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
  // style.Colors[ImGuiCol_DockingPreview]        = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
  // style.Colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
  style.GrabRounding = style.FrameRounding = 2.3f;
}

void embraceTheDarkness() {
  ImVec4* colors = ImGui::GetStyle().Colors;
  colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
  colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
  colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
  colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
  colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
  colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
  colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
  colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
  colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
  colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
  colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
  colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
  colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
  colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
  colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
  // colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  // colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
  colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
  colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
  colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
  colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
  colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowPadding = ImVec2(8.00f, 8.00f);
  style.FramePadding = ImVec2(5.00f, 2.00f);
  style.CellPadding = ImVec2(6.00f, 6.00f);
  style.ItemSpacing = ImVec2(6.00f, 6.00f);
  style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
  style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
  style.IndentSpacing = 25;
  style.ScrollbarSize = 15;
  style.GrabMinSize = 10;
  style.WindowBorderSize = 1;
  style.ChildBorderSize = 1;
  style.PopupBorderSize = 1;
  style.FrameBorderSize = 1;
  style.TabBorderSize = 1;
  style.WindowRounding = 7;
  style.ChildRounding = 4;
  style.FrameRounding = 3;
  style.PopupRounding = 4;
  style.ScrollbarRounding = 9;
  style.GrabRounding = 3;
  style.LogSliderDeadzone = 4;
  style.TabRounding = 4;
}

bool InitGui(GLFWwindow* window, wgpu::Device device) {
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::GetIO();
  embraceTheDarkness();

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

  ImGui::ShowDemoWindow();
  // Build our UI
  {
    static float f = 0.0f;
    static int counter = 0;
    static bool show_demo_window = true;
    static bool show_another_window = true;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!" and append into it.

    ImGui::Text(
        "This is some useful text.");  // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window",
                    &show_demo_window);  // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3("clear color", (float*)&clear_color);  // Edit 3 floats representing a color

    if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return true when
                                  // edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                io.Framerate);
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

void OnDeviceLost(WGPUDeviceLostReason reason, char const* message, void* userdata) {
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

static constexpr uint32_t kDefaultWidth = 600;
static constexpr uint32_t kDefaultHeight = 400;

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

App::App() {
  instance_ = wgpu::CreateInstance();
  GetDevice();
  MainLoop();
}

App::~App() { TerminateGui(); }

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
                                       .width = static_cast<uint32_t>(width_),
                                       .height = static_cast<uint32_t>(height_),
                                       .presentMode = wgpu::PresentMode::Fifo};
  swap_chain_ = device_.CreateSwapChain(surface, &descriptor);
}

bool App::InitDepthBuffer() {
  // Create the depth texture
  wgpu::TextureDescriptor depthTextureDesc;
  depthTextureDesc.dimension = wgpu::TextureDimension::e2D;
  depthTextureDesc.format = depth_texture_format_;
  depthTextureDesc.mipLevelCount = 1;
  depthTextureDesc.sampleCount = 1;
  depthTextureDesc.size = {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_), 1};
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
  InitGlfw();

  if (!InitGui(window_, device_)) {
    std::cout << "Failed to initialize Dear ImGui!" << std::endl;
  }

#if defined(__EMSCRIPTEN__)
  wgpu::SurfaceDescriptorFromCanvasHTMLSelector canvasDesc{};
  canvasDesc.selector = "#canvas";

  wgpu::SurfaceDescriptor surfaceDesc{.nextInChain = &canvasDesc};
  surface_ = instance.CreateSurface(&surfaceDesc);
#else
  surface_ = wgpu::glfw::CreateSurfaceForWindow(instance_, window_);
#endif

  SetupSwapChain(surface_);
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

inline App* AppFromWindow(GLFWwindow* window) {
  return reinterpret_cast<App*>(glfwGetWindowUserPointer(window));
}

void App::OnGlfwResize(GLFWwindow* window, int width, int height) {
  AppFromWindow(window)->OnResize(width, height);
}

void App::OnGlfwSetCursorPos(GLFWwindow* window, double xpos, double ypos) {
  AppFromWindow(window)->OnMouseMove(xpos, ypos);
}

void App::OnGlfwSetMouseButton(GLFWwindow* window, int button, int action, int mods) {
  AppFromWindow(window)->OnMouseButton(button, action, mods);
}

void App::OnGlfwScroll(GLFWwindow* window, double x_offset, double y_offset) {
  AppFromWindow(window)->OnScroll(x_offset, y_offset);
}

void App::InitGlfw() {
  if (!glfwInit()) {
    return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window_ = glfwCreateWindow(kDefaultWidth, kDefaultHeight, "WebGPU window", nullptr, nullptr);
  glfwGetFramebufferSize(window_, &width_, &height_);

  // Setup callbacks.
  glfwSetWindowUserPointer(window_, this);
  glfwSetFramebufferSizeCallback(window_, &OnGlfwResize);
  glfwSetCursorPosCallback(window_, &OnGlfwSetCursorPos);
  glfwSetMouseButtonCallback(window_, &OnGlfwSetMouseButton);
  glfwSetScrollCallback(window_, OnGlfwScroll);
}

void App::OnResize(int , int) {
	glfwGetFramebufferSize(window_, &width_, &height_);
  SetupSwapChain(surface_);
  InitDepthBuffer();
}

void App::OnMouseMove(double xpos, double ypos){}
void App::OnMouseButton(int button, int action, int mods){}
void App::OnScroll(double xoffset, double yoffset){}

}  // namespace web_gpu_app