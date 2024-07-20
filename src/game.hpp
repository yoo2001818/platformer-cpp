#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "SDL_events.h"
#include "application.hpp"
#include "debug_ui.hpp"
#include "entt/entity/fwd.hpp"
#include "movement.hpp"
#include "name.hpp"
#include "physics.hpp"
#include "renderer.hpp"
#include "scene/scene.hpp"
#include "transform.hpp"
#include <entt/entt.hpp>
#include <memory>

namespace platformer {
class game : public application_applet {
public:
  game();
  virtual void init(application &pApplication) override;
  virtual void update(application &pApplication, float pDelta) override;
  virtual void dispose() override;
  virtual void handle_event(application &pApplication,
                            SDL_Event &pEvent) override;
  entt::registry &registry();
  void change_scene(std::shared_ptr<scene> &pScene);
  const std::shared_ptr<scene> &current_scene() const;

private:
  entt::registry mRegistry;
  entt::entity mCamera;
  entt::entity mPlayer;
  entt::entity mPlayerHead;
  int mWindowWidth;
  int mWindowHeight;
  name_system mName;
  transform_system mTransform;
  fps_movement_system mMovement;
  physics_system mPhysics;
  debug_ui_system mDebugUi;
  renderer mRenderer;
  application *mApplication;
  std::shared_ptr<scene> mScene;

  void make_player();
};
}; // namespace platformer

#endif // __GAME_HPP__
