#include "render/framebuffer.hpp"
#include "debug.hpp"
#include "render/texture.hpp"
#include <GL/glew.h>
#include <memory>
#include <stdexcept>
#include <variant>

using namespace platformer;

renderbuffer::renderbuffer(const renderbuffer_options &pOptions)
    : mOptions(pOptions) {}
renderbuffer::~renderbuffer() { this->dispose(); }

void renderbuffer::prepare() {
  if (this->mRenderbuffer == -1) {
    glCreateRenderbuffers(1, &(this->mRenderbuffer));
    glBindRenderbuffer(GL_RENDERBUFFER, this->mRenderbuffer);
    this->mIsValid = false;
  }
  if (!this->mIsValid) {
    glBindRenderbuffer(GL_RENDERBUFFER, this->mRenderbuffer);
    auto &options = this->mOptions;
    glRenderbufferStorage(GL_RENDERBUFFER, options.format, options.width,
                          options.height);
    this->mIsValid = true;
  } else {
    glBindRenderbuffer(GL_RENDERBUFFER, this->mRenderbuffer);
  }
}
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

void framebuffer::bind() {
  if (this->mFramebuffer == -1) {
    glCreateFramebuffers(1, &(this->mFramebuffer));
    glBindFramebuffer(GL_FRAMEBUFFER, this->mFramebuffer);
    this->mIsValid = false;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, this->mFramebuffer);
  if (!this->mIsValid) {
    auto &options = this->mOptions;
    std::vector<unsigned int> colorAttachments;
    int i = 0;
    for (auto &target : options.colors) {
      this->set_item(GL_COLOR_ATTACHMENT0 + i, target);
      colorAttachments.push_back(GL_COLOR_ATTACHMENT0 + i);
      i += 1;
    }
    if (colorAttachments.size() >= 1) {
      glDrawBuffers(colorAttachments.size(), colorAttachments.data());
    }
    if (options.depth.has_value()) {
      this->set_item(GL_DEPTH_ATTACHMENT, options.depth.value());
    }
    if (options.stencil.has_value()) {
      this->set_item(GL_STENCIL_ATTACHMENT, options.stencil.value());
    }
    if (options.depthStencil.has_value()) {
      this->set_item(GL_DEPTH_STENCIL_ATTACHMENT, options.depthStencil.value());
    }
    this->mIsValid = true;
    DEBUG("Framebuffer {} configured ({} x {})", this->mFramebuffer,
          this->mWidth, this->mHeight);
  }
  glViewport(0, 0, this->mWidth, this->mHeight);
  DEBUG("Framebuffer {} bound", this->mFramebuffer);
}
void framebuffer::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  DEBUG("Framebuffer {} unbound", this->mFramebuffer);
  // TODO: Set the viewport to match the window size
}
void framebuffer::dispose() {
  if (this->mFramebuffer == -1) {
    DEBUG("Framebuffer {} destroyed", this->mFramebuffer);
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

void framebuffer::set_item(unsigned int mBuffer,
                           const framebuffer_target &mTarget) {
  if (std::holds_alternative<std::shared_ptr<texture>>(mTarget.texture)) {
    auto &inst = std::get<std::shared_ptr<texture>>(mTarget.texture);
    inst->prepare(0);
    auto target = mTarget.target != 0 ? mTarget.target : GL_TEXTURE_2D;
    glFramebufferTexture2D(GL_FRAMEBUFFER, mBuffer, target, inst->mTexture,
                           mTarget.level);
    this->mWidth = inst->options().width;
    this->mHeight = inst->options().height;
    DEBUG("Framebuffer {} setting buffer {:x} to texture {} ({:x})",
          this->mFramebuffer, mBuffer, inst->mTexture, target);
  } else if (std::holds_alternative<std::shared_ptr<renderbuffer>>(
                 mTarget.texture)) {
    auto &inst = std::get<std::shared_ptr<renderbuffer>>(mTarget.texture);
    inst->prepare();
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, mBuffer, GL_RENDERBUFFER,
                              inst->mRenderbuffer);
    this->mWidth = inst->options().width;
    this->mHeight = inst->options().height;
    DEBUG("Framebuffer {} setting buffer {:x} to renderbuffer {}",
          this->mFramebuffer, mBuffer, inst->mRenderbuffer);
  } else {
    throw std::runtime_error("Either texture or renderbuffer must be provided");
  }
}
