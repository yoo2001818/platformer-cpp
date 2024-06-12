#include "render/framebuffer.hpp"
#include <GL/glew.h>

using namespace platformer;

renderbuffer::renderbuffer(const renderbuffer_options &pOptions)
    : mOptions(pOptions) {}
renderbuffer::~renderbuffer() { this->dispose(); }

void renderbuffer::prepare() {}
void renderbuffer::dispose() {
  if (this->mRenderbuffer == -1) {
    glDeleteRenderbuffers(1, &(this->mRenderbuffer));
    this->mRenderbuffer = -1;
  }
}

void renderbuffer::options(const renderbuffer_options &pOptions) {
  this->mOptions = pOptions;
  this->mIsValid = false;
}
const renderbuffer_options &renderbuffer::options() const {
  return this->mOptions;
}

framebuffer::framebuffer() {}
framebuffer::framebuffer(const framebuffer_options &pOptions)
    : mOptions(pOptions) {}
framebuffer::~framebuffer() { this->dispose(); }

void framebuffer::bind() {}
void framebuffer::unbind() {}
void framebuffer::dispose() {
  if (this->mFramebuffer == -1) {
    glDeleteFramebuffers(1, &(this->mFramebuffer));
    this->mFramebuffer = -1;
  }
}

void framebuffer::options(const framebuffer_options &pOptions) {
  this->mOptions = pOptions;
  this->mIsValid = false;
}
const framebuffer_options &framebuffer::options() const {
  return this->mOptions;
}
