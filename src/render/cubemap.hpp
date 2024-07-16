#ifndef __CUBEMAP_HPP__
#define __CUBEMAP_HPP__
#include "framebuffer.hpp"
#include "texture.hpp"

namespace platformer {
class cubemap {
public:
  cubemap();
  ~cubemap();

  texture_cube &getTexture();

  void render();

protected:
  void renderSide(int pTarget);

  texture_cube mTexture;
  framebuffer mFramebuffer;
};
} // namespace platformer
#endif
