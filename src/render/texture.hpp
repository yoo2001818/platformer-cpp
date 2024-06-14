#ifndef __RENDER_TEXTURE_HPP__
#define __RENDER_TEXTURE_HPP__

#include <cstdint>
#include <string>
#include <vector>

namespace platformer {
class framebuffer;
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

protected:
  virtual void init();

private:
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
