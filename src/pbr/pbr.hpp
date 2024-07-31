#ifndef __PBR_HPP__
#define __PBR_HPP__
#include "render/cubemap.hpp"
#include "render/texture.hpp"
#include "render/texture_generated.hpp"
namespace platformer {
class texture_brdf : public texture_generated {
public:
  using texture_generated::texture_generated;
  void render_image() override;
};

class cubemap_pbr : public cubemap_quad {
public:
  cubemap_pbr(const std::shared_ptr<texture_cube> &pOriginalTexture,
              const texture_options &pTextureOptions);
  cubemap_pbr(const std::shared_ptr<texture_cube> &pOriginalTexture,
              const texture_options &pTextureOptions,
              const texture_format &pTextureFormat);

protected:
  virtual void set_uniforms(int pTarget, int pLevel,
                            const glm::mat4 &pProjection,
                            const glm::mat4 &pView) override;
  virtual bool is_rendered_per_mipmap_level() override;
  std::shared_ptr<texture_cube> mOriginalTexture;
};
} // namespace platformer
#endif
