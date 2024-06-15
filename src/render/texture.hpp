#ifndef __RENDER_TEXTURE_HPP__
#define __RENDER_TEXTURE_HPP__

#include <GL/glew.h>
#include <cstdint>
#include <string>
#include <vector>

namespace platformer {
class framebuffer;

struct texture_options {
  int magFilter = GL_LINEAR;
  int minFilter = GL_LINEAR_MIPMAP_LINEAR;
  int wrapS = GL_REPEAT;
  int wrapT = GL_REPEAT;
  int width = 0;
  int height = 0;
  bool mipmap = true;
  // int format = GL_RGBA;
  // int type = GL_UNSIGNED_BYTE;
};

class texture {
public:
  texture();
  texture(const texture &pValue);
  texture(texture &&pValue);

  texture &operator=(const texture &pValue);
  texture &operator=(texture &&pValue);

  virtual ~texture();

  void prepare(int pSlot);
  void dispose();

  const texture_options &options() const;
  void options(const texture_options &pOptions);

protected:
  virtual void init();

private:
  texture_options mOptions;
  unsigned int mTexture = -1;
  friend framebuffer;
};

class buffer_texture : public texture {
public:
  buffer_texture();

  virtual ~buffer_texture();

protected:
  virtual void init() override;

private:
  unsigned int mWidth;
  unsigned int mHeight;
  std::vector<std::uint8_t> mBuffer;
};

class image_texture : public texture {
public:
  image_texture(const std::string &pSource);

  virtual ~image_texture();

protected:
  virtual void init() override;

private:
  std::string mSource;
};
} // namespace platformer

#endif
