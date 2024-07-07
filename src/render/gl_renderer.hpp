#ifndef __RENDER_GL_RENDERER_HPP__
#define __RENDER_GL_RENDERER_HPP__

#include "render_state.hpp"

namespace platformer {
class gl_renderer {
public:
  gl_renderer();

  void apply_render_state(const render_state &to);

private:
  float mWidth;
  float mHeight;
  render_state mRenderState;
};
} // namespace platformer

#endif
