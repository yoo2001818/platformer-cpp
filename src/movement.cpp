#define GLM_ENABLE_EXPERIMENTAL
#include "movement.hpp"
#include "SDL_events.h"
#include "SDL_mouse.h"
#include "game.hpp"
#include "transform.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <optional>

using namespace platformer;

float movement::pitch() const { return this->mPitch; }

void movement::pitch(float pValue) { this->mPitch = pValue; }

float movement::yaw() const { return this->mYaw; }

void movement::yaw(float pValue) { this->mYaw = pValue; }

void movement::update(transform &pTransform) {
  glm::quat rot = glm::identity<glm::quat>();
  rot = glm::rotate(rot, -mYaw, glm::vec3(0.0f, 1.0f, 0.0f));
  rot = glm::rotate(rot, -mPitch, glm::vec3(1.0f, 0.0f, 0.0f));
  pTransform.rotation(rot);
}

void movement::update_from(transform &pTransform) {
  auto &rot = pTransform.rotation();
  auto eye = glm::rotate(rot, glm::vec3(0.0f, 0.0f, 1.0f));
  float yaw = atan2(eye.x, eye.z);
  float pitch = atan2(eye.y, std::sqrt(eye.x * eye.x + eye.z * eye.z));
  this->mPitch = pitch;
  this->mYaw = yaw;
}

glm::vec3 movement::eye() {
  // FIXME: Maybe optimize this
  glm::quat rot = glm::identity<glm::quat>();
  rot = glm::rotate(rot, -mYaw, glm::vec3(0.0f, 1.0f, 0.0f));
  rot = glm::rotate(rot, -mPitch, glm::vec3(1.0f, 0.0f, 0.0f));
  auto eye = glm::rotate(rot, glm::vec3(0.0f, 0.0f, 1.0f));
  return eye;
}

void movement_system::init(game &pGame) {
  this->mMoveForward = 0;
  this->mMoveRight = 0;
  this->mMoveUp = 0;
  this->mMouseLocked = false;
}

void movement_system::update(game &pGame, float pDelta) {}

void movement_system::mouse_pan(game &pGame, int pXRel, int pYRel) {
  if (this->mControllingEntity == std::nullopt)
    return;
  auto &entity = this->mControllingEntity.value();
  auto &registry = pGame.registry();
  auto &movementVal = registry.get<movement>(entity);
  auto &transformVal = registry.get<transform>(entity);

  auto pitch = movementVal.pitch();
  pitch += static_cast<float>(pYRel) / 400.0f * std::numbers::pi;
  pitch = std::min(static_cast<float>(std::numbers::pi) / 2.0f, pitch);
  pitch = std::max(-static_cast<float>(std::numbers::pi) / 2.0f, pitch);
  auto yaw = movementVal.yaw();
  yaw += static_cast<float>(pXRel) / 400.0f * std::numbers::pi;

  movementVal.pitch(pitch);
  movementVal.yaw(yaw);
  movementVal.update(transformVal);
}

void movement_system::handle_event(game &pGame, SDL_Event &pEvent) {
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
    }
    break;
  }
}

void movement_system::controlling_entity(
    const std::optional<entt::entity> &pEntity) {
  this->mControllingEntity = pEntity;
}

const std::optional<entt::entity> &movement_system::controlling_entity() const {
  return this->mControllingEntity;
}
