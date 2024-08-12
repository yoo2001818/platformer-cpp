#ifndef __PIPELINE_HPP__
#define __PIPELINE_HPP__

#include "render/render.hpp"
#include "render/shader.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace platformer {
class renderer;
struct shader_block {
  std::vector<std::string> vertex_dependencies;
  std::string vertex_body;
  std::vector<std::string> fragment_dependencies;
  std::string fragment_header;
  std::string fragment_body;
};
class pipeline {
  // While it should support both forward and deferred in the future, let's just
  // stick with forward rendering
public:
  pipeline(renderer &pRenderer);
  void render();
  const std::vector<render_light> &get_lights() const;
  std::shared_ptr<shader>
  get_shader(const std::string &pShaderId,
             const std::function<shader_block()> &pExec);
  void prepare_shader(std::shared_ptr<shader> &pShader);

private:
  void collect_lights();
  renderer &mRenderer;
  std::vector<render_light> mLights;
};
class renderpass {
public:
  renderpass();
};
} // namespace platformer

#endif
