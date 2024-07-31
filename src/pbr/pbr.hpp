#ifndef __PBR_HPP__
#define __PBR_HPP__
#include "render/texture_generated.hpp"
namespace platformer {
class texture_brdf : public texture_generated {
public:
  using texture_generated::texture_generated;
  void render_image() override;
};
} // namespace platformer
#endif
