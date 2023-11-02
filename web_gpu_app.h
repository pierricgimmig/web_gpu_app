#include <webgpu/webgpu_cpp.h>

#include <string>

struct GLFWwindow;

namespace web_gpu_app {

class App {
 public:
  App();
  virtual ~App();

 protected:
  void GetDevice();
  void OnDevice(wgpu::Device device);
  void SetupSwapChain(wgpu::Surface surface);
  bool InitDepthBuffer();
  void CreateRenderPipeline();
  void MainLoop();
  void Render();
  virtual std::string_view GetName() { return "App"; };

  wgpu::Instance instance_;
  wgpu::Device device_;
  wgpu::SwapChain swap_chain_;
  wgpu::RenderPipeline pipeline_;

  // Depth Buffer.
	wgpu::TextureFormat depth_texture_format_ = wgpu::TextureFormat::Depth24Plus;
	wgpu::Texture depth_texture_ = nullptr;
	wgpu::TextureView depth_texture_view_ = nullptr;

  GLFWwindow* window_ = nullptr;
  uint32_t width_ = 0;
  uint32_t height_ = 0;
};

}  // namespace web_gpu_app