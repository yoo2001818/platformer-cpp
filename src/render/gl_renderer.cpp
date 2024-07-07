#include "gl_renderer.hpp"
#include "render/render_state.hpp"

using namespace platformer;

gl_renderer::gl_renderer() {}

void gl_renderer::apply_render_state(const render_state &to) {
  platformer::apply_render_state(this->mRenderState, to);
}
