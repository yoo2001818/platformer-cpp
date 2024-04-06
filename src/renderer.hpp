#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "render/render.hpp"
#include <entt/entt.hpp>
#include <vector>

namespace platformer {
class game;
class renderer {
public:
  renderer();
  std::vector<render_light> get_lights(game &pGame);
  void render(game &pGame, const entt::entity pCamera, float pAspect);
};
} // namespace platformer

#endif // __RENDERER_HPP__
