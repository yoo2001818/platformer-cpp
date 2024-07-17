#ifndef __CUBEMAP_HPP__
#define __CUBEMAP_HPP__
#include "framebuffer.hpp"
#include "texture.hpp"
#include "glm/glm.hpp"

namespace platformer {
class cubemap {
public:
  cubemap(const texture_options& pTextureOptions);
  virtual ~cubemap();

  texture_cube &getTexture();

  virtual void render();

protected:
  virtual void renderSide(int pTarget, glm::mat4& pProjection, glm::mat4& pView);

  texture_options mTextureOptions;
  texture_cube mTexture;
  framebuffer mFramebuffer;
};

class cubemap_quad : public cubemap {
  public:
  cubemap_quad(const texture_options& pTextureOptions);
  virtual ~cubemap_quad();

protected:
  virtual void renderSide(int pTarget, glm::mat4& pProjection, glm::mat4& pView) override;
};
} // namespace platformer
#endif
