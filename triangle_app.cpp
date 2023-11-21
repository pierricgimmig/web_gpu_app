#include "triangle_app.h"

#include "imgui.h"

namespace web_gpu_app {

TriangleApp::TriangleApp() { renderer_ = std::make_unique<WebGpuRenderer>(window_); }

TriangleApp::~TriangleApp() {}

Renderables TriangleApp::Update() {
  ImGui::ShowDemoWindow();
  return {};
}

}  // namespace web_gpu_app
