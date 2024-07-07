#include "triangle_app.h"
#include "web_gpu_app/web_gpu_renderer.h"

int main() {
  GLFWwindow* window = web_gpu_app::App::CreateGlfwWindow();
  web_gpu_app::WebGpuRenderer::Create(window, [](std::unique_ptr<Renderer> renderer) {
    static web_gpu_app::TriangleApp app(std::move(renderer));
    app.Run();
  });
}
