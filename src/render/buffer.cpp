#include "buffer.hpp"
#include "debug.hpp"
#include <GL/glew.h>

using namespace platformer;

gl_buffer::gl_buffer(int pType, int pUsage) : mType(pType), mUsage(pUsage) {}
gl_buffer::~gl_buffer() { this->dispose(); }

unsigned int gl_buffer::length() { return this->mByteLength / this->mDataSize; }

void gl_buffer::bind() {
  if (this->mBuffer == -1) {
    glCreateBuffers(1, &(this->mBuffer));
    // DEBUG("Buffer created {}", this->mBuffer);
  }
  glBindBuffer(this->mType, this->mBuffer);
}

void gl_buffer::dispose() {
  if (this->mBuffer != -1) {
    glDeleteBuffers(1, &this->mBuffer);
    this->mBuffer = -1;
  }
}

void gl_buffer::set(const void *pData, unsigned int pLength,
                    unsigned int pDataSize) {
  this->mDataSize = pDataSize;
  this->buffer_data(pData, pLength);
}

void gl_buffer::set(const void *pData, unsigned int pOffset,
                    unsigned int pLength, unsigned int pDataSize) {
  this->buffer_sub_data(pData, pOffset, pLength);
}

void gl_buffer::buffer_data(const void *pData, unsigned int pLength) {
  this->bind();
  glBufferData(this->mType, pLength, pData, this->mUsage);
  // DEBUG("Buffer data bound {} ({})", this->mBuffer, pLength);
}

void gl_buffer::buffer_sub_data(const void *pData, unsigned int pOffset,
                                unsigned int pLength) {
  this->bind();
  glBufferSubData(this->mType, pOffset, pLength, pData);
  // DEBUG("Buffer data sub bound {} ({}, {})", this->mBuffer, pLength,
  // pOffset);
}

gl_texture_buffer::gl_texture_buffer(int pUsage)
    : gl_buffer(GL_TEXTURE_BUFFER, pUsage) {}

gl_array_buffer::gl_array_buffer(int pUsage)
    : gl_buffer(GL_ARRAY_BUFFER, pUsage) {}

gl_element_array_buffer::gl_element_array_buffer(int pUsage)
    : gl_buffer(GL_ELEMENT_ARRAY_BUFFER, pUsage) {}
