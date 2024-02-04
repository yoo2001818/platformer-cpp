#ifndef __MOVEMENT_HPP__
#define __MOVEMENT_HPP__
#include "SDL_events.h"
#include "entt/entity/fwd.hpp"
#include "transform.hpp"
#include <glm/fwd.hpp>
#include <optional>

namespace platformer {
class game;
class movement {
public:
  float pitch() const;
  void pitch(float pValue);
  float yaw() const;
  void yaw(float pValue);

  void update(transform &pTransform);
  void update_from(transform &pTransform);
  glm::vec3 eye();

private:
  float mPitch = 0.0f;
  float mYaw = 0.0f;
};

class movement_system {
public:
  void init(game &pGame);
  void update(game &pGame, float pDelta);
  void handle_event(game &pGame, SDL_Event &pEvent);

  void controlling_entity(const std::optional<entt::entity> &pEntity);
  const std::optional<entt::entity> &controlling_entity() const;

private:
  std::optional<entt::entity> mControllingEntity;
  float mMoveForward;
  float mMoveRight;
  float mMoveUp;
  bool mMouseLocked = false;
  void mouse_pan(game &pGame, int pXRel, int pYRel);
};
} // namespace platformer
#endif // __MOVEMENT_HPP__
