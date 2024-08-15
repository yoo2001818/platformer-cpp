#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "entt/entity/fwd.hpp"
#include "render/renderer.hpp"
#include <memory>
#include <vector>
namespace platformer {
class shader_block;
class light_1 {
public:
  light_1();
  virtual ~light_1();
  virtual shader_block get_shader_block(int pNumLights, renderer &pRenderer);
  virtual void set_uniforms(std::vector<entt::entity> pEntities,
                            renderer &pRenderer);
};

class point_light : public light_1 {};

struct light_component {
  std::shared_ptr<platformer::light> light;
};

} // namespace platformer

#endif
