#include "pbr.hpp"
#include "render/geometry.hpp"
#include "render/shader.hpp"

using namespace platformer;

void texture_brdf::render_image() {
  file_shader vShader("res/quad.vert", "res/brdfmap.frag");
  geometry vQuad = geometry::make_quad();
  vShader.prepare();
  vQuad.prepare(vShader);
  vQuad.render();
}
