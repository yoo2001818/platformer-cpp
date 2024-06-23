#include "render/texture.hpp"
#include "stb_image.h"
#include <GL/glew.h>
#include <cstdint>
#include <stdexcept>
#include <variant>

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
                     texture_options &pOptions) {
  if (std::holds_alternative<texture_source_buffer>(pSource)) {
    auto &source = std::get<texture_source_buffer>(pSource);
    if (source.data.has_value()) {
      glTexImage2D(pTarget, 0, source.internalFormat, source.width,
                   source.height, 0, source.format, source.type,
                   source.data->data());
    } else {
      glTexImage2D(pTarget, 0, source.internalFormat, source.width,
                   source.height, 0, source.format, source.type, nullptr);
    }
  } else if (std::holds_alternative<texture_source_image>(pSource)) {
    auto &source = std::get<texture_source_image>(pSource);
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    unsigned char *data = nullptr;
    if (source.data.has_value()) {
      data = stbi_load_from_memory(
          reinterpret_cast<const unsigned char *>(source.data->data()),
          source.data->size(), &width, &height, &channels, STBI_rgb_alpha);
    } else {
      data = stbi_load(source.filename.c_str(), &width, &height, &channels,
                       STBI_rgb_alpha);
    }
    if (data == nullptr) {
      throw std::runtime_error(stbi_failure_reason());
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    if (glGetError() != GL_NO_ERROR) {
      throw std::runtime_error("glGetError failed");
    }
    pOptions.width = width;
    pOptions.height = height;
  }
}

void texture::set_options(int pTarget, const texture_options &pOptions) {
  glTextureParameteri(pTarget, GL_TEXTURE_MAG_FILTER, pOptions.magFilter);
  glTextureParameteri(pTarget, GL_TEXTURE_MIN_FILTER, pOptions.minFilter);
  glTextureParameteri(pTarget, GL_TEXTURE_WRAP_S, pOptions.wrapS);
  glTextureParameteri(pTarget, GL_TEXTURE_WRAP_T, pOptions.wrapT);
}

void texture::generate_mipmap(int pTarget) { glGenerateMipmap(pTarget); }

texture_2d::texture_2d() {}
texture_2d::texture_2d(const texture_source &pSource) {}
texture_2d::texture_2d(texture_source &&pSource) {}
texture_2d::texture_2d(const texture_source &pSource,
                       const texture_options &pOptions) {}
texture_2d::texture_2d(texture_source &&pSource,
                       const texture_options &pOptions) {}

texture_2d::~texture_2d() {}

const texture_source &texture_2d::source() const { return this->mSource; }
void texture_2d::source(const texture_source &pSource) {
  this->mSource = pSource;
  this->mIsValid = false;
}
void texture_2d::source(texture_source &&pSource) {
  this->mSource = pSource;
  this->mIsValid = false;
}

void texture_2d::init() {
  this->upload(GL_TEXTURE_2D, this->mSource, this->mOptions);
  this->set_options(GL_TEXTURE_2D, this->mOptions);
  if (this->mOptions.mipmap) {
    this->generate_mipmap(GL_TEXTURE_2D);
  }
}

texture_cube::texture_cube() {}
texture_cube::texture_cube(const texture_cube_source &pSource) {}
texture_cube::texture_cube(texture_cube_source &&pSource) {}
texture_cube::texture_cube(const texture_cube_source &pSource,
                           const texture_options &pOptions) {}
texture_cube::texture_cube(texture_cube_source &&pSource,
                           const texture_options &pOptions) {}

texture_cube::~texture_cube() {}

const texture_cube_source &texture_cube::source() const {
  return this->mSource;
}
void texture_cube::source(const texture_cube_source &pSource) {
  this->mSource = pSource;
  this->mIsValid = false;
}
void texture_cube::source(texture_cube_source &&pSource) {
  this->mSource = pSource;
  this->mIsValid = false;
}

void texture_cube::init() {
  this->upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, this->mSource.left,
               this->mOptions);
  this->upload(GL_TEXTURE_CUBE_MAP_POSITIVE_X, this->mSource.right,
               this->mOptions);
  this->upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, this->mSource.down,
               this->mOptions);
  this->upload(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, this->mSource.up,
               this->mOptions);
  this->upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, this->mSource.back,
               this->mOptions);
  this->upload(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, this->mSource.front,
               this->mOptions);
  this->set_options(GL_TEXTURE_CUBE_MAP, this->mOptions);
  if (this->mOptions.mipmap) {
    this->generate_mipmap(GL_TEXTURE_CUBE_MAP);
  }
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
