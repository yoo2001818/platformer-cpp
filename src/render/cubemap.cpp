#include "cubemap.hpp"
#include "file.hpp"
#include "render/framebuffer.hpp"
#include "render/texture.hpp"
#include <algorithm>
#include <cmath>
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
cubemap::cubemap(const texture_options &pTextureOptions,
                 const texture_format &pTextureFormat)
    : mTextureOptions(pTextureOptions), mTextureFormat(pTextureFormat),
      mTexture(std::make_shared<texture_cube>()),
      mFramebuffer(std::make_shared<framebuffer>()) {}
cubemap::~cubemap() {}

std::shared_ptr<texture_cube> &cubemap::get_texture() {
  if (!this->mTexture->is_valid()) {
    this->render();
  }
  return this->mTexture;
}

void cubemap::render() {
  auto width = this->mTextureOptions.width;
  auto height = this->mTextureOptions.height;
  if (width <= 0 || height <= 0) {
    throw std::runtime_error(
        "cubemap width/height must be present in texture options");
  }
  auto &format = this->mTextureFormat;
  texture_source_buffer texSource{
      .format = format.format,
      .internalFormat = format.internalFormat,
      .type = format.type,
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
  unsigned int numMipmapLevels;
  bool renderedPerMipmapLevel = this->is_rendered_per_mipmap_level();
  if (renderedPerMipmapLevel) {
    numMipmapLevels = 1 + std::floor(std::log2(std::max(width, height)));
  } else {
    numMipmapLevels = 1;
  }
  for (int target = 0; target < 6; target += 1) {
    for (int level = 0; level < numMipmapLevels; level += 1) {
      auto &fb = this->mFramebuffer;
      fb->options(
          {.colors = {{.texture = this->mTexture,
                       .target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + target,
                       .level = level}}});
      fb->bind();
      this->render_side(GL_TEXTURE_CUBE_MAP_POSITIVE_X + target, level,
                        projection, transforms[target]);
      fb->unbind();
    }
  }
  // Finalize the cubemap (generate mipmap, etc)
  if (!renderedPerMipmapLevel && this->mTextureOptions.mipmap) {
    this->mTexture->generate_mipmaps();
  }
}

void cubemap::render_side(int pTarget, int pLevel, const glm::mat4 &pProjection,
                          const glm::mat4 &pView) {}

bool cubemap::is_rendered_per_mipmap_level() { return false; }

cubemap_quad::cubemap_quad(const std::shared_ptr<shader> &pShader,
                           const texture_options &pTextureOptions)
    : cubemap(pTextureOptions), mShader(pShader), mQuad(geometry::make_quad()) {
}
cubemap_quad::cubemap_quad(std::shared_ptr<shader> &&pShader,
                           const texture_options &pTextureOptions)
    : cubemap(pTextureOptions), mShader(pShader), mQuad(geometry::make_quad()) {
}
cubemap_quad::cubemap_quad(const std::shared_ptr<shader> &pShader,
                           const texture_options &pTextureOptions,
                           const texture_format &pTextureFormat)
    : cubemap(pTextureOptions, pTextureFormat), mShader(pShader),
      mQuad(geometry::make_quad()) {}
cubemap_quad::cubemap_quad(std::shared_ptr<shader> &&pShader,
                           const texture_options &pTextureOptions,
                           const texture_format &pTextureFormat)
    : cubemap(pTextureOptions, pTextureFormat), mShader(pShader),
      mQuad(geometry::make_quad()) {}
cubemap_quad::~cubemap_quad() {}

void cubemap_quad::render_side(int pTarget, int pLevel,
                               const glm::mat4 &pProjection,
                               const glm::mat4 &pView) {
  // This is just an example, obviously it needs to be improved
  glClearColor(.5f, .5f, .5f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  this->mShader->prepare();
  this->mQuad.prepare(*this->mShader);
  this->mShader->set("uView", pView);
  this->set_uniforms(pTarget, pLevel, pProjection, pView);
  this->mQuad.render();
  glEnable(GL_DEPTH_TEST);
}

void cubemap_quad::set_uniforms(int pTarget, int pLevel,
                                const glm::mat4 &pProjection,
                                const glm::mat4 &pView) {}
