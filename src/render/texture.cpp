#include "render/texture.hpp"
#include "render/buffer.hpp"
#include "stb_image.h"
#include "util/debug.hpp"
#include <GL/glew.h>
#include <GL/glu.h>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <variant>

using namespace platformer;

texture::texture() : mTexture(-1) {}
texture::texture(const texture &pValue) : mOptions(pValue.mOptions) {
  this->mTexture = pValue.mTexture;
}
texture::texture(texture &&pValue) : mOptions(pValue.mOptions) {
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
  auto type = this->type();
  if (this->mTexture == -1) {
    glActiveTexture(GL_TEXTURE0 + pSlot);
    glGenTextures(1, &(this->mTexture));
    glBindTexture(type, this->mTexture);
    this->mIsValid = false;
  } else {
    glActiveTexture(GL_TEXTURE0 + pSlot);
    glBindTexture(type, this->mTexture);
  }
  if (!this->mIsValid) {
    this->init();
    this->mIsValid = true;
    // DEBUG("Texture {} initialized", this->mTexture);
  }
}
void texture::dispose() {
  if (this->mTexture != -1) {
    glDeleteTextures(1, &this->mTexture);
    // DEBUG("Texture {} disposed", this->mTexture);
    this->mTexture = -1;
  }
}

void texture::generate_mipmaps() { this->generate_mipmap(this->type()); }

void texture::init() {
  uint8_t dummy[3] = {255, 0, 255};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE,
               &dummy);
}

int texture::type() { return GL_TEXTURE_2D; }

void texture::upload(int pTarget, const texture_source &pSource,
                     texture_options &pOptions) {
  if (std::holds_alternative<texture_source_buffer>(pSource)) {
    auto &source = std::get<texture_source_buffer>(pSource);
    if (source.data.has_value()) {
      glTexImage2D(pTarget, 0, source.format.internalFormat, source.width,
                   source.height, 0, source.format.format, source.format.type,
                   source.data->data());
      DEBUG("Texture {} target {:x} upload from buffer ({} x {})",
            this->mTexture, pTarget, source.width, source.height);
    } else {
      glTexImage2D(pTarget, 0, source.format.internalFormat, source.width,
                   source.height, 0, source.format.format, source.format.type,
                   nullptr);
      DEBUG("Texture {} target {:x} upload from null ({} x {})", this->mTexture,
            pTarget, source.width, source.height);
    }
    pOptions.width = source.width;
    pOptions.height = source.height;
  } else if (std::holds_alternative<texture_source_image>(pSource)) {
    auto &source = std::get<texture_source_image>(pSource);
    stbi_set_flip_vertically_on_load(true);
    int width, height, channels;
    int desired_channels;
    switch (source.format.format) {
    case GL_RGBA:
      desired_channels = STBI_rgb_alpha;
      break;
    case GL_RGB:
      desired_channels = STBI_rgb;
      break;
    case GL_R:
      desired_channels = STBI_grey;
      break;
    case GL_RG:
      desired_channels = STBI_grey_alpha;
      break;
    default:
      throw std::runtime_error("Unsupported texture format specified");
    }
    void *data = nullptr;
    if (source.data.has_value()) {
      switch (source.format.type) {
      case GL_UNSIGNED_BYTE:
        data = stbi_load_from_memory(
            reinterpret_cast<const unsigned char *>(source.data->data()),
            source.data->size(), &width, &height, &channels, desired_channels);
        break;
      case GL_UNSIGNED_SHORT:
        data = stbi_load_16_from_memory(
            reinterpret_cast<const unsigned char *>(source.data->data()),
            source.data->size(), &width, &height, &channels, desired_channels);
        break;
      case GL_FLOAT:
        data = stbi_loadf_from_memory(
            reinterpret_cast<const unsigned char *>(source.data->data()),
            source.data->size(), &width, &height, &channels, desired_channels);
        break;
      default:
        throw std::runtime_error("Unsupported texture type specified");
      }
    } else {
      switch (source.format.type) {
      case GL_UNSIGNED_BYTE:
        data = stbi_load(source.filename.c_str(), &width, &height, &channels,
                         desired_channels);
        break;
      case GL_UNSIGNED_SHORT:
        data = stbi_load_16(source.filename.c_str(), &width, &height, &channels,
                            desired_channels);
        break;
      case GL_FLOAT:
        data = stbi_loadf(source.filename.c_str(), &width, &height, &channels,
                          desired_channels);
        break;
      default:
        throw std::runtime_error("Unsupported texture type specified");
      }
    }
    if (data == nullptr) {
      throw std::runtime_error(stbi_failure_reason());
    }
    glTexImage2D(pTarget, 0, source.format.internalFormat, width, height, 0,
                 source.format.format, source.format.type, data);
    stbi_image_free(data);
    pOptions.width = width;
    pOptions.height = height;
    DEBUG("Texture {} target {:x} upload from image {} ({} x {})",
          this->mTexture, pTarget, source.filename, width, height);
  }
  /*
  if (auto err = glGetError()) {
    // This does not work correctly for some reason
    throw std::runtime_error(
        std::format("texture upload failed ({})",
                    reinterpret_cast<const char *>(gluErrorString(err))));
  }
  */
}

const texture_options &texture::options() const { return this->mOptions; }

void texture::options(const texture_options &pOptions) {
  this->mOptions = pOptions;
  this->mIsValid = false;
}

void texture::set_options(int pTarget, const texture_options &pOptions) {
  DEBUG("Texture {} target {:x} setting options", this->mTexture, pTarget);
  glTexParameteri(pTarget, GL_TEXTURE_MAG_FILTER, pOptions.magFilter);
  glTexParameteri(pTarget, GL_TEXTURE_MIN_FILTER, pOptions.minFilter);
  glTexParameteri(pTarget, GL_TEXTURE_WRAP_S, pOptions.wrapS);
  glTexParameteri(pTarget, GL_TEXTURE_WRAP_T, pOptions.wrapT);
}

void texture::generate_mipmap(int pTarget) {
  DEBUG("Texture {} generating mipmap", this->mTexture);
  glGenerateMipmap(pTarget);
}

bool texture::is_valid() { return this->mIsValid; }

texture_2d::texture_2d() {}
texture_2d::texture_2d(const texture_2d &pValue)
    : texture(pValue), mSource(pValue.mSource) {}
texture_2d::texture_2d(texture_2d &&pValue)
    : texture(pValue), mSource(pValue.mSource) {}
texture_2d::texture_2d(const texture_source &pSource) : mSource(pSource) {}
texture_2d::texture_2d(texture_source &&pSource) : mSource(pSource) {}
texture_2d::texture_2d(const texture_source &pSource,
                       const texture_options &pOptions)
    : texture(pOptions), mSource(pSource) {}
texture_2d::texture_2d(texture_source &&pSource,
                       const texture_options &pOptions)
    : texture(pOptions), mSource(pSource) {}

texture_2d::~texture_2d() {}

texture_2d &texture_2d::operator=(const texture &pValue) {
  texture::operator=(pValue);
  if (const texture_2d *derived = dynamic_cast<const texture_2d *>(&pValue)) {
    this->mSource = derived->mSource;
    this->mIsValid = derived->mIsValid;
  }
  return *this;
}

texture_2d &texture_2d::operator=(texture &&pValue) {
  texture::operator=(pValue);
  if (const texture_2d *derived = dynamic_cast<const texture_2d *>(&pValue)) {
    this->mSource = derived->mSource;
    this->mIsValid = derived->mIsValid;
  }
  return *this;
}

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

int texture_2d::type() { return GL_TEXTURE_2D; }

texture_cube::texture_cube() {}
texture_cube::texture_cube(const texture_cube_source &pSource)
    : mSource(pSource) {}
texture_cube::texture_cube(texture_cube_source &&pSource) : mSource(pSource) {}
texture_cube::texture_cube(const texture_cube_source &pSource,
                           const texture_options &pOptions)
    : texture(pOptions), mSource(pSource) {}
texture_cube::texture_cube(texture_cube_source &&pSource,
                           const texture_options &pOptions)
    : texture(pOptions), mSource(pSource) {}

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
  this->upload(GL_TEXTURE_CUBE_MAP_POSITIVE_X, this->mSource.right,
               this->mOptions);
  this->upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, this->mSource.left,
               this->mOptions);
  this->upload(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, this->mSource.up,
               this->mOptions);
  this->upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, this->mSource.down,
               this->mOptions);
  this->upload(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, this->mSource.front,
               this->mOptions);
  this->upload(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, this->mSource.back,
               this->mOptions);
  this->set_options(GL_TEXTURE_CUBE_MAP, this->mOptions);
  if (this->mOptions.mipmap) {
    this->generate_mipmap(GL_TEXTURE_CUBE_MAP);
  }
}

int texture_cube::type() { return GL_TEXTURE_CUBE_MAP; }

texture_buffer::texture_buffer(
    const std::shared_ptr<gl_texture_buffer> &pBuffer, int pInternalFormat)
    : mBuffer(pBuffer), mInternalFormat(pInternalFormat) {}

texture_buffer::~texture_buffer() {}

void texture_buffer::init() {
  this->mBuffer->bind();
  glTexBuffer(GL_TEXTURE_BUFFER, mInternalFormat, mBuffer->mBuffer);
}

int texture_buffer::type() { return GL_TEXTURE_BUFFER; }
