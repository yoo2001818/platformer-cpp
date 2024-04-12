#ifndef __RENDER_TEXTURE_HPP__
#define __RENDER_TEXTURE_HPP__

#include <cstdint>
#include <vector>

namespace platformer {
class texture {
public:
  texture();
  texture(const texture &pValue);
  texture(texture &&pValue);

  texture &operator=(const texture &pValue);
  texture &operator=(texture &&pValue);

  virtual ~texture();

  void prepare(int &pSlot);
  void dispose();

protected:
  virtual void init();

private:
  unsigned int pTexture;
};

class buffer_texture : public texture {
public:
  buffer_texture();

  virtual ~buffer_texture();

protected:
  virtual void init() override;

private:
  unsigned int pWidth;
  unsigned int pHeight;
  std::vector<std::uint8_t> pBuffer;
};
} // namespace platformer

#endif
