#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include "entt/entity/fwd.hpp"
#include "render/gizmo.hpp"
#include "render/pipeline.hpp"
#include "render/render.hpp"
#include "render/render_state.hpp"
#include <entt/entt.hpp>
#include <memory>
#include <vector>

namespace platformer {
class game;
class renderer {
public:
  renderer(platformer::game &pGame);
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

  platformer::game &game() const;
  platformer::asset_manager &asset_manager();
  entt::registry &registry();
  platformer::pipeline &pipeline();
  std::vector<std::shared_ptr<gizmo>> &gizmos();

private:
  render_state mRenderState;
  platformer::asset_manager mAssetManager{};
  platformer::game &mGame;
  std::unique_ptr<platformer::pipeline> mPipeline;
  entt::registry &mRegistry;
  entt::entity mCamera;
  std::vector<std::shared_ptr<gizmo>> mGizmos;
  int mWidth;
  int mHeight;
};
} // namespace platformer

#endif // __RENDERER_HPP__
