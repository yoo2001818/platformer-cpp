#ifndef __MOVEMENT_HPP__
#define __MOVEMENT_HPP__
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "entt/entity/fwd.hpp"
#include "transform.hpp"
#include <bitset>
#include <glm/fwd.hpp>
#include <optional>

namespace platformer {
class game;
class fps_movement {
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

class fps_movement_system {
public:
  void init(game &pGame);
  void update(game &pGame, float pDelta);
  void handle_event(game &pGame, SDL_Event &pEvent);

  void body_entity(const std::optional<entt::entity> &pEntity);
  const std::optional<entt::entity> &body_entity() const;

  void body_mesh_entity(const std::optional<entt::entity> &pEntity);
  const std::optional<entt::entity> &body_mesh_entity() const;

  void head_entity(const std::optional<entt::entity> &pEntity);
  const std::optional<entt::entity> &head_entity() const;

  void camera_entity(const std::optional<entt::entity> &pEntity);
  const std::optional<entt::entity> &camera_entity() const;

private:
  std::optional<entt::entity> mBodyMeshEntity;
  std::optional<entt::entity> mBodyEntity;
  std::optional<entt::entity> mHeadEntity;
  std::optional<entt::entity> mCameraEntity;
  std::bitset<7> mMovePressed;
  bool mMouseLocked = false;
  bool mThirdPerson = false;
  void update_movedir(game &pGame, float pDelta);
  void update_jump(game &pGame, float pDelta);
  void mouse_pan(game &pGame, int pXRel, int pYRel);
  void sync_camera(game &pGame);
  void handle_key(SDL_Keycode &pKey, bool pState);
};
} // namespace platformer
#endif // __MOVEMENT_HPP__
