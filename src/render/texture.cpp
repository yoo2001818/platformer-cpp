#include "render/texture.hpp"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_surface.h"
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

void flip_surface(SDL_Surface *surface) {
  SDL_LockSurface(surface);

  int pitch = surface->pitch;   // row size
  char *temp = new char[pitch]; // intermediate buffer
  char *pixels = (char *)surface->pixels;

  for (int i = 0; i < surface->h / 2; ++i) {
    // get pointers to the two rows to swap
    char *row1 = pixels + i * pitch;
    char *row2 = pixels + (surface->h - i - 1) * pitch;

    // swap rows
    memcpy(temp, row1, pitch);
    memcpy(row1, row2, pitch);
    memcpy(row2, temp, pitch);
  }

  delete[] temp;

  SDL_UnlockSurface(surface);
}

image_texture::image_texture(const std::string &pSource)
    : texture(), mSource(pSource) {}

image_texture::~image_texture() {}

void image_texture::init() {
  SDL_Surface *surface = IMG_Load(this->mSource.c_str());
  if (surface == nullptr) {
    throw std::runtime_error(IMG_GetError());
  }
  SDL_Surface *new_surface =
      SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
  if (new_surface == nullptr) {
    SDL_FreeSurface(surface);
    throw std::runtime_error(SDL_GetError());
  }
  flip_surface(new_surface);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, new_surface->w, new_surface->h, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, new_surface->pixels);
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
  SDL_FreeSurface(new_surface);
  SDL_FreeSurface(surface);
}
