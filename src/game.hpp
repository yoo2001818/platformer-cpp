#ifndef __GAME_HPP__
#define __GAME_HPP__

#include "SDL_events.h"
#include "application.hpp"
#include "entt/entity/fwd.hpp"
#include "movement.hpp"
#include "physics.hpp"
#include "renderer.hpp"
#include "transform.hpp"
#include <entt/entt.hpp>

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

private:
  entt::registry mRegistry;
  entt::entity mCamera;
  entt::entity mPlayer;
  int mWindowWidth;
  int mWindowHeight;
  transform_system mTransform;
  fps_movement_system mMovement;
  physics_system mPhysics;
  renderer mRenderer;
};
}; // namespace platformer

#endif // __GAME_HPP__
