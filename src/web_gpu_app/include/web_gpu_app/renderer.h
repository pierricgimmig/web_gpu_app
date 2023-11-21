#pragma once

#include <stb/stb_image.h>
#include <tinyobj/tiny_obj_loader.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <span>

using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;
using Quat = glm::quat;
using Color = glm::vec4;

struct Line {
  Vec3 from;
  Vec3 to;
  Color color;
};

struct Tripod {
  Mat4 transform;
  float size = 1.f;
};

struct Cube {
  Mat4 transform;
  float size;
  Color color;
};

struct Sphere {
  Mat4 transform;
  float radius;
  Color color;
};

struct Mesh {
  Mat4 transform;
  tinyobj::mesh_t mesh;
  float scale = 1.f;
};

struct Renderables {
  std::span<Line> lines;
  std::span<Tripod> tripods;
  std::span<Cube> cubes;
  std::span<Sphere> spheres;
  std::span<Mesh> meshes;
};

class Renderer {
 public:
  Renderer() = default;
  virtual ~Renderer(){};
  virtual void BeginFrame() = 0;
  virtual void EndFrame(const Renderables& renderables) = 0;
  virtual void OnResize(int width, int height) = 0;
};