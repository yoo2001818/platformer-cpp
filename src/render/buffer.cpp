#include "buffer.hpp"
#include <GL/glew.h>

using namespace platformer;

buffer::buffer(int pType, int pUsage) : mType(pType), mUsage(pUsage) {}
buffer::~buffer() { this->dispose(); }

unsigned int buffer::length() { return this->mByteLength / this->mDataSize; }

void buffer::bind() {}

void buffer::dispose() {
  if (this->mBuffer != -1) {
    glDeleteBuffers(1, &this->mBuffer);
    this->mBuffer = -1;
  }
}

void buffer::set(void *pData, unsigned int pLength, unsigned int pDataSize) {
  this->mDataSize = pDataSize;
  this->buffer_data(pData, pLength);
}

void buffer::buffer_data(void *pData, unsigned int pLength) {
  this->bind();
  glBufferData(this->mType, pLength, pData, this->mUsage);
}

void buffer::buffer_sub_data(void *pData, unsigned int pOffset,
                             unsigned int pLength) {
  this->bind();
  glBufferSubData(this->mType, pOffset, pLength, pData);
}
