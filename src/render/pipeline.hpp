#ifndef __PIPELINE_HPP__
#define __PIPELINE_HPP__

namespace platformer {
class pipeline {
  // While it should support both forward and deferred in the future, let's just
  // stick with forward rendering
public:
  pipeline();
  void render();

private:
  void collect_lights();
};
} // namespace platformer

#endif
