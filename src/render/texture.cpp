#include "render/texture.hpp"
#include "stb_image.h"
#include <GL/glew.h>
#include <cstdint>
#include <stdexcept>

using namespace platformer;

texture::texture() : mTexture(-1) {}
texture::texture(const texture &pValue) { this->mTexture = pValue.mTexture; }
texture::texture(texture &&pValue) {
  this->mTexture = pValue.mTexture;
  pValue.mTexture = -1;
}
texture::texture(const texture_options &pOptions)
    : mTexture(-1), mOptions(pOptions) {}

texture &texture::operator=(const texture &pValue) {
  this->mTexture = pValue.mTexture;
  this->mOptions = pValue.mOptions;
  return *this;
}

texture &texture::operator=(texture &&pValue) {
  this->mTexture = pValue.mTexture;
  this->mOptions = pValue.mOptions;
  pValue.mTexture = -1;
  return *this;
}

texture::~texture() { this->dispose(); }

void texture::prepare(int pSlot) {
  if (this->mTexture == -1) {
    glCreateTextures(GL_TEXTURE_2D, 1, &(this->mTexture));
    glActiveTexture(GL_TEXTURE0 + pSlot);
    glBindTexture(GL_TEXTURE_2D, this->mTexture);
    this->mIsValid = false;
  }
  glActiveTexture(GL_TEXTURE0 + pSlot);
  glBindTexture(GL_TEXTURE_2D, this->mTexture);
  if (!this->mIsValid) {
    this->init();
    this->mIsValid = true;
  }
}
void texture::dispose() {
  if (this->mTexture != -1) {
    glDeleteTextures(1, &this->mTexture);
    this->mTexture = -1;
  }
}

void texture::init() {
  uint8_t dummy[3] = {255, 0, 255};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE,
               &dummy);
}

void texture::upload(int pTarget, const texture_source &pSource,
                     texture_options &pOptions) {}

void texture::set_options(int pTarget, const texture_options &pOptions) {}

texture_2d::texture_2d() {}
texture_2d::texture_2d(const texture_source &pSource) {}
texture_2d::texture_2d(texture_source &&pSource) {}
texture_2d::texture_2d(const texture_source &pSource,
                       const texture_options &pOptions) {}
texture_2d::texture_2d(texture_source &&pSource,
                       const texture_options &pOptions) {}

texture_2d::~texture_2d() {}

const texture_source &texture_2d::source() const {}
void texture_2d::source(const texture_source &pSource) {}
void texture_2d::source(texture_source &&pSource) {}

void texture_2d::init() {}

texture_cube::texture_cube() {}
texture_cube::texture_cube(const texture_cube_source &pSource) {}
texture_cube::texture_cube(texture_cube_source &&pSource) {}
texture_cube::texture_cube(const texture_cube_source &pSource,
                           const texture_options &pOptions) {}
texture_cube::texture_cube(texture_cube_source &&pSource,
                           const texture_options &pOptions) {}

texture_cube::~texture_cube() {}

const texture_cube_source &texture_cube::source() const {}
void texture_cube::source(const texture_cube_source &pSource) {}
void texture_cube::source(texture_cube_source &&pSource) {}

void texture_cube::init() {}

image_texture::image_texture(const std::string &pSource)
    : texture(), mSource(pSource) {}

image_texture::~image_texture() {}

void image_texture::init() {
  stbi_set_flip_vertically_on_load(true);
  int width, height, channels;
  unsigned char *data = stbi_load(this->mSource.c_str(), &width, &height,
                                  &channels, STBI_rgb_alpha);
  if (data == nullptr) {
    throw std::runtime_error(stbi_failure_reason());
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);
  if (glGetError() != GL_NO_ERROR) {
    throw std::runtime_error("glGetError failed");
  }
  if (true) {
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  } else {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  stbi_image_free(data);
}
