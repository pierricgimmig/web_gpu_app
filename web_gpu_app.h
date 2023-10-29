#include <webgpu/webgpu_cpp.h>

#include <string>

namespace web_gpu_app {

class App {
 public:
  App();

 protected:
  void GetDevice();
  void OnDevice(wgpu::Device device);
  void SetupSwapChain(wgpu::Surface surface);
  void CreateRenderPipeline();
  void MainLoop();
  void Render();
  virtual std::string_view GetName() { return "App"; };

  wgpu::Instance instance_;
  wgpu::Device device_;
  wgpu::SwapChain swap_chain_;
  wgpu::RenderPipeline pipeline_;

  uint32_t width_ = 0;
  uint32_t height_ = 0;
};

}  // namespace web_gpu_app