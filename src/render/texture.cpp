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

texture &texture::operator=(const texture &pValue) {
  this->mTexture = pValue.mTexture;
  return *this;
}

texture &texture::operator=(texture &&pValue) {
  this->mTexture = pValue.mTexture;
  pValue.mTexture = -1;
  return *this;
}

texture::~texture() { this->dispose(); }

void texture::prepare(int pSlot) {
  if (this->mTexture == -1) {
    glCreateTextures(GL_TEXTURE_2D, 1, &(this->mTexture));
    glActiveTexture(GL_TEXTURE0 + pSlot);
    glBindTexture(GL_TEXTURE_2D, this->mTexture);
    this->init();
  }
  glActiveTexture(GL_TEXTURE0 + pSlot);
  glBindTexture(GL_TEXTURE_2D, this->mTexture);
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

buffer_texture::~buffer_texture() {}

void buffer_texture::init() {
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->mWidth, this->mHeight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, this->mBuffer.data());
}

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
