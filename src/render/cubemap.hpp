#ifndef __CUBEMAP_HPP__
#define __CUBEMAP_HPP__
#include "framebuffer.hpp"
#include "geometry.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <memory>

namespace platformer {
class cubemap {
public:
  cubemap(const texture_options &pTextureOptions);
  cubemap(const texture_options &pTextureOptions,
          const texture_format &pTextureFormat);
  virtual ~cubemap();

  std::shared_ptr<texture_cube> &get_texture();

  virtual void render();

protected:
  virtual void render_side(int pTarget, int pLevel,
                           const glm::mat4 &pProjection,
                           const glm::mat4 &pView);
  virtual bool is_rendered_per_mipmap_level();

  texture_options mTextureOptions;
  texture_format mTextureFormat;
  std::shared_ptr<texture_cube> mTexture;
  std::shared_ptr<framebuffer> mFramebuffer;
};

class cubemap_quad : public cubemap {
public:
  cubemap_quad(const std::shared_ptr<shader> &pShader,
               const texture_options &pTextureOptions);
  cubemap_quad(std::shared_ptr<shader> &&pShader,
               const texture_options &pTextureOptions);
  cubemap_quad(const std::shared_ptr<shader> &pShader,
               const texture_options &pTextureOptions,
               const texture_format &pTextureFormat);
  cubemap_quad(std::shared_ptr<shader> &&pShader,
               const texture_options &pTextureOptions,
               const texture_format &pTextureFormat);
  virtual ~cubemap_quad();

protected:
  virtual void render_side(int pTarget, int pLevel,
                           const glm::mat4 &pProjection,
                           const glm::mat4 &pView) override;
  virtual void set_uniforms(int pTarget, int pLevel,
                            const glm::mat4 &pProjection,
                            const glm::mat4 &pView);

  geometry mQuad;
  std::shared_ptr<shader> mShader;
};

class cubemap_equirectangular : public cubemap_quad {
public:
  cubemap_equirectangular(const std::shared_ptr<texture> &pOriginalTexture,
                          const texture_options &pTextureOptions);
  cubemap_equirectangular(const std::shared_ptr<texture> &pOriginalTexture,
                          const texture_options &pTextureOptions,
                          const texture_format &pTextureFormat);

protected:
  virtual void set_uniforms(int pTarget, int pLevel,
                            const glm::mat4 &pProjection,
                            const glm::mat4 &pView) override;
  std::shared_ptr<texture> mOriginalTexture;
};

} // namespace platformer
#endif
