#include "triangle_app.h"

#include "imgui.h"

namespace web_gpu_app {

TriangleApp::TriangleApp(std::unique_ptr<Renderer> renderer) : renderer_(std::move(renderer)) {}

TriangleApp::~TriangleApp() {}

const char* TriangleApp::GetTitle() { return "Triangle App"; }

Renderer* TriangleApp::GetRenderer() { return renderer_.get(); }

Renderables TriangleApp::Update() {
  ImGui::ShowDemoWindow();
  return {};
}

}  // namespace web_gpu_app
