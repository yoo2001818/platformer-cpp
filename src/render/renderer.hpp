#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "entt/entity/fwd.hpp"
#include "render/render.hpp"
#include "render/render_state.hpp"
#include <entt/entt.hpp>
#include <vector>

namespace platformer {
class game;
class renderer {
public:
  renderer(game &pGame);
  std::vector<render_light> get_lights(game &pGame);
  void apply_render_state(const render_state &to);

  void init();
  void clear();
  void render();

  entt::entity camera() const;
  void camera(entt::entity pValue);

  int width() const;
  void width(int pValue);
  int height() const;
  void height(int pValue);

private:
  render_state mRenderState;
  asset_manager mAssetManager{};
  game &mGame;
  entt::registry &mRegistry;
  entt::entity mCamera;
  int mWidth;
  int mHeight;
};
} // namespace platformer

#endif // __RENDERER_HPP__
