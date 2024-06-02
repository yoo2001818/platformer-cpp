#define GLM_ENABLE_EXPERIMENTAL
#include "movement.hpp"
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "game.hpp"
#include "physics.hpp"
#include "transform.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <optional>

using namespace platformer;

float fps_movement::pitch() const { return this->mPitch; }

void fps_movement::pitch(float pValue) { this->mPitch = pValue; }

float fps_movement::yaw() const { return this->mYaw; }

void fps_movement::yaw(float pValue) { this->mYaw = pValue; }

void fps_movement::update(transform &pTransform) {
  glm::quat rot = glm::identity<glm::quat>();
  rot = glm::rotate(rot, -mYaw, glm::vec3(0.0f, 1.0f, 0.0f));
  rot = glm::rotate(rot, -mPitch, glm::vec3(1.0f, 0.0f, 0.0f));
  pTransform.rotation(rot);
}

void fps_movement::update_from(transform &pTransform) {
  auto &rot = pTransform.rotation();
  auto eye = glm::rotate(rot, glm::vec3(0.0f, 0.0f, 1.0f));
  float yaw = atan2(eye.x, eye.z);
  float pitch = atan2(eye.y, std::sqrt(eye.x * eye.x + eye.z * eye.z));
  this->mPitch = pitch;
  this->mYaw = yaw;
}

glm::vec3 fps_movement::eye() {
  // FIXME: Maybe optimize this
  glm::quat rot = glm::identity<glm::quat>();
  rot = glm::rotate(rot, -mYaw, glm::vec3(0.0f, 1.0f, 0.0f));
  rot = glm::rotate(rot, -mPitch, glm::vec3(1.0f, 0.0f, 0.0f));
  auto eye = glm::rotate(rot, glm::vec3(0.0f, 0.0f, 1.0f));
  return eye;
}

void fps_movement_system::init(game &pGame) {
  this->mMovePressed.reset();
  this->mMouseLocked = false;
}

void fps_movement_system::update(game &pGame, float pDelta) {
  this->update_movedir(pGame, pDelta);
  this->update_jump(pGame, pDelta);
}

void fps_movement_system::update_movedir(game &pGame, float pDelta) {
  glm::vec3 direction(0.0);
  for (int i = 0; i < 6; i += 1) {
    if (this->mMovePressed[i]) {
      direction[i % 3] += i < 3 ? 1.0f : -1.0f;
    }
  }
  if (glm::length(direction) < 0.01f)
    return;
  direction = glm::normalize(direction);

  if (this->mBodyEntity == std::nullopt)
    return;
  auto &entity = this->mBodyEntity.value();
  auto &registry = pGame.registry();
  auto &movementVal = registry.get<fps_movement>(entity);
  auto &transformVal = registry.get<transform>(entity);

  glm::quat rot = glm::identity<glm::quat>();
  rot = glm::rotate(rot, -movementVal.yaw(), glm::vec3(0.0f, 1.0f, 0.0f));
  auto worldDir = glm::rotate(rot, direction);
  transformVal.translate(worldDir * pDelta * 5.0f);
}

void fps_movement_system::update_jump(game &pGame, float pDelta) {
  if (!this->mMovePressed[6])
    return;
  if (this->mBodyEntity == std::nullopt)
    return;
  auto &entity = this->mBodyEntity.value();
  auto &registry = pGame.registry();
  auto &movementVal = registry.get<fps_movement>(entity);
  auto &transformVal = registry.get<transform>(entity);
  auto physicsVal = registry.try_get<physics>(entity);
  if (physicsVal == nullptr)
    return;
  if (physicsVal->on_ground() != 0)
    return;
  physicsVal->force() += glm::vec3(0.0f, 10.0f, 0.0f);
}

void fps_movement_system::mouse_pan(game &pGame, int pXRel, int pYRel) {
  if (this->mBodyEntity == std::nullopt)
    return;
  auto &entity = this->mBodyEntity.value();
  auto &registry = pGame.registry();
  auto &movementVal = registry.get<fps_movement>(entity);
  auto &transformVal = registry.get<transform>(entity);

  auto pitch = movementVal.pitch();
  pitch += static_cast<float>(pYRel) / 400.0f * std::numbers::pi;
  pitch = std::min(static_cast<float>(std::numbers::pi) / 2.0f, pitch);
  pitch = std::max(-static_cast<float>(std::numbers::pi) / 2.0f, pitch);
  auto yaw = movementVal.yaw();
  yaw += static_cast<float>(pXRel) / 400.0f * std::numbers::pi;

  // movementVal.pitch(pitch);
  movementVal.yaw(yaw);
  movementVal.update(transformVal);
}

void fps_movement_system::handle_event(game &pGame, SDL_Event &pEvent) {
  switch (pEvent.type) {
  case SDL_MOUSEMOTION:
    if (this->mMouseLocked) {
      this->mouse_pan(pGame, pEvent.motion.xrel, pEvent.motion.yrel);
    }
    break;
  case SDL_MOUSEBUTTONDOWN:
    if (pEvent.button.button == SDL_BUTTON_LEFT && !this->mMouseLocked) {
      this->mMouseLocked = true;
      SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    break;
  case SDL_KEYDOWN:
    switch (pEvent.key.keysym.sym) {
    case SDLK_ESCAPE:
      this->mMouseLocked = false;
      SDL_SetRelativeMouseMode(SDL_FALSE);
      break;
    default:
      this->handle_key(pEvent.key.keysym.sym, true);
      break;
    }
    break;
  case SDL_KEYUP:
    this->handle_key(pEvent.key.keysym.sym, false);
    break;
  }
}

void fps_movement_system::handle_key(SDL_Keycode &pKey, bool pState) {
  switch (pKey) {
  case SDLK_d:
    this->mMovePressed[0] = pState;
    break;
  case SDLK_q:
    this->mMovePressed[1] = pState;
    break;
  case SDLK_s:
    this->mMovePressed[2] = pState;
    break;
  case SDLK_a:
    this->mMovePressed[3] = pState;
    break;
  case SDLK_e:
    this->mMovePressed[4] = pState;
    break;
  case SDLK_w:
    this->mMovePressed[5] = pState;
    break;
  case SDLK_SPACE:
    this->mMovePressed[6] = pState;
  }
}

void fps_movement_system::body_entity(
    const std::optional<entt::entity> &pEntity) {
  this->mBodyEntity = pEntity;
}

const std::optional<entt::entity> &fps_movement_system::body_entity() const {
  return this->mBodyEntity;
}

void fps_movement_system::head_entity(
    const std::optional<entt::entity> &pEntity) {
  this->mHeadEntity = pEntity;
}

const std::optional<entt::entity> &fps_movement_system::head_entity() const {
  return this->mHeadEntity;
}
