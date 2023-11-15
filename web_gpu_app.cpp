#include "web_gpu_app.h"

#include <GLFW/glfw3.h>

#include <iostream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_wgpu.h"
#include "glm/glm.hpp"
#include "utils.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

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

#if defined(__EMSCRIPTEN__)
void EmscriptenMainLoop(void* app) { reinterpret_cast<App*>(app)->Render(); }
#endif

web_gpu_app::App* AppFromWindow(GLFWwindow* window) {
  return reinterpret_cast<web_gpu_app::App*>(glfwGetWindowUserPointer(window));
}

}  // OnGlfwResize

namespace web_gpu_app {

const char simple_shader_code[] = R"(
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
  // Imgui
  SetupUi();
  MainLoop();
}

App::~App() {
  ImGui_ImplGlfw_Shutdown();
  ImGui_ImplWGPU_Shutdown();
}

void App::MainLoop() {
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
  pass.SetPipeline(render_pipeline_);
  pass.Draw(3);
  RenderUi(pass);
  pass.End();
  wgpu::CommandBuffer commands = encoder.Finish();
  device_.GetQueue().Submit(1, &commands);
  device_.Tick();
}

void App::RenderUi(wgpu::RenderPassEncoder render_pass) {
  ImGui_ImplWGPU_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::ShowDemoWindow();

  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass.Get());
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

GLFWwindow* App::SetupGlfwWindow(const char* title, void* user_pointer) {
  if (!glfwInit()) {
    return nullptr;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  static constexpr uint32_t kInitialWidth = 600;
  static constexpr uint32_t kInitialHeight = 400;
  GLFWwindow* window = glfwCreateWindow(kInitialWidth, kInitialHeight, title,
                                        /*monitor*/ nullptr, /*share*/ nullptr);

  // Setup callbacks.
  glfwSetWindowUserPointer(window, user_pointer);
  glfwSetFramebufferSizeCallback(window, &OnGlfwResize);
  glfwSetCursorPosCallback(window, &OnGlfwSetCursorPos);
  glfwSetMouseButtonCallback(window, &OnGlfwSetMouseButton);
  glfwSetScrollCallback(window, OnGlfwScroll);
  return window;
}

void App::SetupUi() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::GetIO();
  ImGui_ImplGlfw_InitForOther(window_, true);
  ImGui_ImplWGPU_Init(device_.Get(), 3, WGPUTextureFormat_BGRA8Unorm,
                      WGPUTextureFormat_Depth24Plus);
  SetUiThemeDark();
}

void App::OnResize(int width, int height) {
  width_ = width;
  height_ = height;
  swap_chain_ = SetupSwapChain(surface_, device_, width_, height_);
  depth_texture_ = SetupDepthTexture(device_, depth_texture_format_, width_, height_);
  depth_texture_view_ = SetupDepthTextureView(depth_texture_, depth_texture_format_);
}

void App::OnMouseMove(double xpos, double ypos) {}
void App::OnMouseButton(int button, int action, int mods) {
  TRACE_VAR(button);
  TRACE_VAR(action);
  TRACE_VAR(mods);
}
void App::OnScroll(double xoffset, double yoffset) {
  TRACE_VAR(xoffset);
  TRACE_VAR(yoffset);
}

}  // namespace web_gpu_app