#include "render/texture_generated.hpp"

using namespace platformer;

texture_generated::texture_generated(const texture_options &pTextureOptions)
    : mTextureOptions(pTextureOptions),
      mTexture(std::make_shared<texture_2d>()),
      mFramebuffer(std::make_shared<framebuffer>()) {}
texture_generated::texture_generated(const texture_options &pTextureOptions,
                                     const texture_format &pTextureFormat)
    : mTextureOptions(pTextureOptions), mTextureFormat(pTextureFormat),
      mTexture(std::make_shared<texture_2d>()),
      mFramebuffer(std::make_shared<framebuffer>()) {}
texture_generated::~texture_generated() {}

std::shared_ptr<texture_2d> &texture_generated::get_texture() {
  if (!this->mTexture->is_valid()) {
    this->render();
  }
  return this->mTexture;
}

void texture_generated::render() {
  auto width = this->mTextureOptions.width;
  auto height = this->mTextureOptions.height;
  if (width <= 0 || height <= 0) {
    throw std::runtime_error(
        "texture_generated width/height must be present in texture options");
  }
  auto &format = this->mTextureFormat;
  texture_source_buffer texSource{
      .format = format,
      .width = width,
      .height = height,
  };
  this->mTexture->options(this->mTextureOptions);
  this->mTexture->source(texSource);
  this->mTexture->prepare(0);
  auto &fb = this->mFramebuffer;
  fb->options({.colors = {{.texture = this->mTexture}}});
  fb->bind();
  this->render_image();
  fb->unbind();
  if (this->mTextureOptions.mipmap) {
    this->mTexture->prepare(0);
    this->mTexture->generate_mipmaps();
  }
}

void texture_generated::render_image() {}
