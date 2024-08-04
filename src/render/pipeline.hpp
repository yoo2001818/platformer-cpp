#ifndef __PIPELINE_HPP__
#define __PIPELINE_HPP__

#include "render/render.hpp"

namespace platformer {
class renderer;
class pipeline {
  // While it should support both forward and deferred in the future, let's just
  // stick with forward rendering
public:
  pipeline(renderer &pRenderer);
  void render();

private:
  std::vector<render_light> collect_lights();
  renderer &mRenderer;
};
} // namespace platformer

#endif
