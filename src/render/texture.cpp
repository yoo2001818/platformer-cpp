#include "render/texture.hpp"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_surface.h"
#include <GL/glew.h>
#include <stdexcept>

using namespace platformer;

texture::texture() {}
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

void buffer_texture::init() {
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->mWidth, this->mHeight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, this->mBuffer.data());
}

void image_texture::init() {
  SDL_Surface *surface = IMG_Load(this->mSource.c_str());
  if (surface == nullptr) {
    throw std::runtime_error(IMG_GetError());
  }
  SDL_Surface *new_surface =
      SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA8888, 0);
  if (new_surface == nullptr) {
    SDL_FreeSurface(surface);
    throw std::runtime_error(SDL_GetError());
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, new_surface->w, new_surface->h, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, new_surface->pixels);
  SDL_FreeSurface(new_surface);
  SDL_FreeSurface(surface);
}
