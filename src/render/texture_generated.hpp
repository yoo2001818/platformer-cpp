#ifndef __TEXTURE_GENERATED_HPP__
#define __TEXTURE_GENERATED_HPP__

#include "render/framebuffer.hpp"
#include "render/texture.hpp"
#include <memory>

// TODO: This should be merged with cubemaps sometimes later
namespace platformer {
class texture_generated {
public:
  texture_generated(const texture_options &pTextureOptions);
  texture_generated(const texture_options &pTextureOptions,
                    const texture_format &pTextureFormat);
  virtual ~texture_generated();

  std::shared_ptr<texture_2d> &get_texture();

  virtual void render();
  virtual void render_image();

protected:
  texture_options mTextureOptions;
  texture_format mTextureFormat;
  std::shared_ptr<texture_2d> mTexture;
  std::shared_ptr<framebuffer> mFramebuffer;
};
} // namespace platformer

#endif
