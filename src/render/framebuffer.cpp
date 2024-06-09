#include "render/framebuffer.hpp"

using namespace platformer;

renderbuffer::renderbuffer(const renderbuffer_options &pOptions)
    : mOptions(pOptions) {}
renderbuffer::~renderbuffer() {}

void renderbuffer::prepare() {}
void renderbuffer::dispose() {}

void renderbuffer::options(const renderbuffer_options &pOptions) {}
const renderbuffer_options &renderbuffer::options() const {}

framebuffer::framebuffer() {}
framebuffer::framebuffer(const framebuffer_options &pOptions)
    : mOptions(pOptions) {}
framebuffer::~framebuffer() {}

void framebuffer::bind() {}
void framebuffer::unbind() {}
void framebuffer::dispose() {}

void framebuffer::options(const framebuffer_options &pOptions) {}
const framebuffer_options &framebuffer::options() const {}
