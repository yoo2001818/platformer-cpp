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

cubemap_pbr::cubemap_pbr(const std::shared_ptr<texture_cube> &pOriginalTexture,
                         const texture_options &pTextureOptions)
    : cubemap_quad(std::make_shared<file_shader>("res/skyboxgen.vert",
                                                 "res/skyboxgen_pbr.frag"),
                   pTextureOptions),
      mOriginalTexture(pOriginalTexture) {}

cubemap_pbr::cubemap_pbr(const std::shared_ptr<texture_cube> &pOriginalTexture,
                         const texture_options &pTextureOptions,
                         const texture_format &pTextureFormat)
    : cubemap_quad(std::make_shared<file_shader>("res/skyboxgen.vert",
                                                 "res/skyboxgen_pbr.frag"),
                   pTextureOptions, pTextureFormat),
      mOriginalTexture(pOriginalTexture) {}

void cubemap_pbr::set_uniforms(int pTarget, int pLevel,
                               const glm::mat4 &pProjection,
                               const glm::mat4 &pView) {
  auto width = this->mTextureOptions.width;
  int numMipmapLevels = 1 + std::floor(std::log2(width));
  this->mOriginalTexture->prepare(2);
  this->mShader->set("uSource", 2);
  this->mShader->set("uMipLevel", pLevel);
  // We want at least 8*8 for irradiance too
  this->mShader->set("uMaxMipLevel", numMipmapLevels - 3);
  this->mShader->set("uResolution", (float)width);
}

bool cubemap_pbr::is_rendered_per_mipmap_level() { return true; }
