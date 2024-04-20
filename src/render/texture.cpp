#include "render/texture.hpp"
#include <GL/glew.h>

using namespace platformer;

texture::texture() {}
texture::texture(const texture &pValue) {}
texture::texture(texture &&pValue) {}

texture &texture::operator=(const texture &pValue) {}
texture &texture::operator=(texture &&pValue) {}

texture::~texture() {}

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
  glTexImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 0, 0, nullptr);
}
