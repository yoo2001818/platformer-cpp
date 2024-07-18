#include "cubemap.hpp"
#include "render/framebuffer.hpp"
#include "render/render_state.hpp"
#include "render/texture.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <memory>
#include <stdexcept>

using namespace platformer;

cubemap::cubemap(const texture_options &pTextureOptions)
    : mTextureOptions(pTextureOptions),
      mTexture(std::make_shared<texture_cube>()),
      mFramebuffer(std::make_shared<framebuffer>()) {}
cubemap::~cubemap() {}

texture_cube &cubemap::getTexture() {
  if (!this->mTexture->is_valid()) {
    this->render();
  }
  return *(this->mTexture);
}

void cubemap::render() {
  auto width = this->mTextureOptions.width;
  auto height = this->mTextureOptions.height;
  if (width <= 0 || height <= 0) {
    throw std::runtime_error(
        "cubemap width/height must be present in texture options");
  }
  texture_source_buffer texSource{
      .format = GL_RGBA,
      .internalFormat = GL_RGBA,
      .type = GL_UNSIGNED_BYTE,
      .width = width,
      .height = height,
  };
  this->mTexture->options(this->mTextureOptions);
  this->mTexture->source({
      texSource,
      texSource,
      texSource,
      texSource,
      texSource,
      texSource,
  });
  this->mTexture->prepare(0);
  glm::mat4 projection =
      glm::perspectiveFov(glm::pi<float>() / 2.0f, 1.0f, 1.0f, 0.1f, 10.0f);
  glm::mat4 transforms[] = {
      glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, -1.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f)),
  };
  // Render each side
  for (int i = 0; i < 6; i += 1) {
    auto &fb = this->mFramebuffer;
    fb->options({.colors = {{.texture = this->mTexture,
                             .target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i}}});
    fb->bind();
    this->renderSide(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, projection,
                     transforms[i]);
  }
  // Finalize the cubemap (generate mipmap, etc)
}

void cubemap::renderSide(int pTarget, glm::mat4 &pProjection,
                         glm::mat4 &pView) {}

cubemap_quad::cubemap_quad(const texture_options &pTextureOptions)
    : cubemap(pTextureOptions) {}
cubemap_quad::~cubemap_quad() {}

void cubemap_quad::renderSide(int pTarget, glm::mat4 &pProjection,
                              glm::mat4 &pView) {}
