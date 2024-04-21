#include "render/texture.hpp"
#include <GL/glew.h>

using namespace platformer;

texture::texture() {}
texture::texture(const texture &pValue) { this->pTexture = pValue.pTexture; }
texture::texture(texture &&pValue) {
  this->pTexture = pValue.pTexture;
  pValue.pTexture = -1;
}

texture &texture::operator=(const texture &pValue) {
  this->pTexture = pValue.pTexture;
  return *this;
}

texture &texture::operator=(texture &&pValue) {
  this->pTexture = pValue.pTexture;
  pValue.pTexture = -1;
  return *this;
}

texture::~texture() { this->dispose(); }

void texture::prepare(int pSlot) {
  if (this->pTexture == -1) {
    glCreateTextures(GL_TEXTURE_2D, 1, &(this->pTexture));
    glActiveTexture(GL_TEXTURE0 + pSlot);
    glBindTexture(GL_TEXTURE_2D, this->pTexture);
    this->init();
  }
  glActiveTexture(GL_TEXTURE0 + pSlot);
  glBindTexture(GL_TEXTURE_2D, this->pTexture);
}
void texture::dispose() {
  if (this->pTexture != -1) {
    glDeleteTextures(1, &this->pTexture);
    this->pTexture = -1;
  }
}

void texture::init() {
  uint8_t dummy[3] = {255, 0, 255};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE,
               &dummy);
}
