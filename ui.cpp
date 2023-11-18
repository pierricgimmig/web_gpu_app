#pragma once

#include "ui.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_wgpu.h"
#include "utils.h"

namespace web_gpu_app {

Ui::Ui(GLFWwindow* window, wgpu::Device device, std::function<void()> callback)
    : callback_(callback) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::GetIO().IniFilename = nullptr;
  ImGui_ImplGlfw_InitForOther(window, true);
  ImGui_ImplWGPU_Init(device.Get(), /*num_frames_in_flight=*/3, WGPUTextureFormat_BGRA8Unorm,
                      WGPUTextureFormat_Depth24Plus);
  SetUiThemeDark();
}

Ui::~Ui() {
  ImGui_ImplWGPU_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void Ui::Render(wgpu::RenderPassEncoder render_pass) {
  ImGui_ImplWGPU_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  callback_();

  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass.Get());
}

}  // namespace web_gpu_app