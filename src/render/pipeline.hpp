#ifndef __PIPELINE_HPP__
#define __PIPELINE_HPP__

#include "render/render.hpp"
#include <vector>

namespace platformer {
class renderer;
class pipeline {
  // While it should support both forward and deferred in the future, let's just
  // stick with forward rendering
public:
  pipeline(renderer &pRenderer);
  void render();
  const std::vector<render_light> &get_lights() const;

private:
  void collect_lights();
  renderer &mRenderer;
  std::vector<render_light> mLights;
};
} // namespace platformer

#endif
