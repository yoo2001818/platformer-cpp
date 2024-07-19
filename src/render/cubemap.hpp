#ifndef __CUBEMAP_HPP__
#define __CUBEMAP_HPP__
#include "framebuffer.hpp"
#include "geometry.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace platformer {
class cubemap {
public:
  cubemap(const texture_options &pTextureOptions);
  virtual ~cubemap();

  std::shared_ptr<texture_cube> &get_texture();

  virtual void render();

protected:
  virtual void render_side(int pTarget, int pLevel, glm::mat4 &pProjection,
                           glm::mat4 &pView);
  virtual bool is_rendered_per_mipmap_level();

  texture_options mTextureOptions;
  std::shared_ptr<texture_cube> mTexture;
  std::shared_ptr<framebuffer> mFramebuffer;
};

class cubemap_quad : public cubemap {
public:
  cubemap_quad(const texture_options &pTextureOptions);
  virtual ~cubemap_quad();

protected:
  virtual void render_side(int pTarget, int pLevel, glm::mat4 &pProjection,
                           glm::mat4 &pView) override;

  geometry mQuad;
  shader mShader;
};
} // namespace platformer
#endif
