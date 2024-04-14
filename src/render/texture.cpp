#include "render/texture.hpp"

using namespace platformer;

texture::texture() {}
texture::texture(const texture &pValue) {}
texture::texture(texture &&pValue) {}

texture &texture::operator=(const texture &pValue) {}
texture &texture::operator=(texture &&pValue) {}

texture::~texture() {}

void texture::prepare(int &pSlot) {}
void texture::dispose() {}

void texture::init() {}
