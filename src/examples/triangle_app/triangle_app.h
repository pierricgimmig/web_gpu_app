#pragma once

#include <memory>

#include "web_gpu_app/app.h"
#include "web_gpu_app/renderer.h"

struct GLFWwindow;

namespace web_gpu_app {

class TriangleApp : public App {
 public:
  explicit TriangleApp(std::unique_ptr<Renderer> renderer);
  virtual ~TriangleApp();

  const char* GetTitle() override;
  Renderer* GetRenderer() override;
  Renderables Update() override;

 protected:
  std::unique_ptr<Renderer> renderer_;
};

}  // namespace web_gpu_app
