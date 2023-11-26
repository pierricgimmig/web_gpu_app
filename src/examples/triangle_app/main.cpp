#include "triangle_app.h"
#include "web_gpu_app/web_gpu_renderer.h"

wgpu::Instance instance;

int main() {
  instance = wgpu::CreateInstance();
  web_gpu_app::GetDevice(instance, [](wgpu::Device device) {
    static web_gpu_app::TriangleApp app(instance, device);
    app.Run();
  });
}
