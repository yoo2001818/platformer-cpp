#include "cubemap.hpp"
#include "render/texture.hpp"
#include <stdexcept>

using namespace platformer;

cubemap::cubemap(const texture_options &pTextureOptions)
    : mTextureOptions(pTextureOptions) {}
cubemap::~cubemap() {}

texture_cube &cubemap::getTexture() {
  if (!this->mTexture.is_valid()) {
    this->render();
  }
  return this->mTexture;
}

void cubemap::render() {
  auto width = this->mTextureOptions.width;
  auto height = this->mTextureOptions.height;
  if (width <= 0 || height <= 0) {
    throw std::runtime_error("cubemap width/height must be present in texture options");
  }
  texture_source_buffer texSource {
    .format = GL_RGBA,
    .internalFormat = GL_RGBA,
    .type = GL_UNSIGNED_BYTE,
    .width = width,
    .height = height,
  };
  this->mTexture.options(this->mTextureOptions);
  this->mTexture.source({
    texSource,
    texSource,
    texSource,
    texSource,
    texSource,
    texSource,
  });
  this->mTexture.prepare(0);
  // Render each side
  for (int i = 0; i < 6; i += 1) {
    auto& fb = this->mFramebuffer;
    fb.options({
      .colors = {{.texture = this->mTexture}}
    });
  }
}

void cubemap::renderSide(int pTarget, glm::mat4 &pProjection,
                         glm::mat4 &pView) {}

cubemap_quad::cubemap_quad(const texture_options &pTextureOptions)
    : cubemap(pTextureOptions) {}
cubemap_quad::~cubemap_quad() {}

void cubemap_quad::renderSide(int pTarget, glm::mat4 &pProjection,
                              glm::mat4 &pView) {}