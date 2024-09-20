#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include "entt/entity/fwd.hpp"
#include "transform.hpp"
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/matrix.hpp>
#include <optional>

using namespace platformer;

void transform_system::init(entt::registry &pRegistry) {
  pRegistry.on_construct<transform>().connect<&transform_system::on_construct>(
      *this);
  pRegistry.on_update<transform>().connect<&transform_system::on_update>(*this);
  pRegistry.on_destroy<transform>().connect<&transform_system::on_destroy>(
      *this);
};

int transform_system::global_version() const { return this->mGlobalVersion; }

void transform_system::update_global_version() { this->mGlobalVersion += 1; }

void transform_system::on_construct(entt::registry &pRegistry,
                                    entt::entity pEntity) {
  auto &transformVal = pRegistry.get<transform>(pEntity);
  transformVal.register_registry(&pRegistry);
  this->handle_change(pRegistry, pEntity);
}
void transform_system::on_update(entt::registry &pRegistry,
                                 entt::entity pEntity) {
  this->handle_change(pRegistry, pEntity);
}
void transform_system::on_destroy(entt::registry &pRegistry,
                                  entt::entity pEntity) {
  auto &transformVal = pRegistry.get<transform>(pEntity);
  transformVal.mParent = std::nullopt;
  this->handle_change(pRegistry, pEntity);
}
void transform_system::handle_change(entt::registry &pRegistry,
                                     entt::entity pEntity) {
  auto &transformVal = pRegistry.get<transform>(pEntity);
  auto prev_parent = transformVal.mPreviousParent;
  auto parent = transformVal.parent();
  if (prev_parent != parent) {
    if (prev_parent != std::nullopt) {
      auto prevTransform = pRegistry.try_get<transform>(prev_parent.value());
      if (prevTransform != nullptr) {
        prevTransform->mChildren.erase(
            std::remove(prevTransform->mChildren.begin(),
                        prevTransform->mChildren.end(), pEntity));
      }
    }
    if (parent != std::nullopt) {
      auto parentTransform = pRegistry.try_get<transform>(parent.value());
      if (parentTransform != nullptr) {
        parentTransform->mChildren.push_back(pEntity);
      }
    }
    transformVal.mark_parent_indexed();
  }
}

transform::transform() {};
transform::transform(const entt::entity &pParent) : mParent(pParent) {};
transform::transform(const entt::entity &pParent, const glm::mat4 &pMatrix)
    : mParent(pParent), mMatrix(pMatrix), mMatrixVersion(1) {};
transform::transform(const glm::mat4 &pMatrix)
    : mMatrix(pMatrix), mMatrixVersion(1) {};

const glm::vec3 &transform::position() {
  this->update_component();
  return this->mPosition;
}

void transform::position(const glm::vec3 &pValue) {
  this->update_component();
  this->mPosition = pValue;
  this->mark_component_changed();
}

const glm::vec3 &transform::scale() {
  this->update_component();
  return this->mScale;
}

void transform::scale(const glm::vec3 &pValue) {
  this->update_component();
  this->mScale = pValue;
  this->mark_component_changed();
}

const glm::quat &transform::rotation() {
  this->update_component();
  return this->mRotation;
}

void transform::rotation(const glm::quat &pValue) {
  this->update_component();
  this->mRotation = pValue;
  this->mark_component_changed();
}

const glm::mat4 &transform::matrix_local() {
  this->update_matrix();
  return this->mMatrix;
}

void transform::matrix_local(const glm::mat4 &pValue) {
  // TODO: This is unnecessary, as it doesn't need to recalculate matrix values
  this->update_matrix();
  this->mMatrix = pValue;
  this->mark_matrix_changed();
}

const std::optional<entt::entity> &transform::parent() const {
  return this->mParent;
}

void transform::parent(const std::optional<entt::entity> &pParent) {
  this->mParent = pParent;
  // Reset parent versions to force invalidation
  this->mWorldParentVersion = -1;
}

const std::vector<entt::entity> &transform::children() const {
  return this->mChildren;
}

const glm::mat4 &transform::matrix_world(entt::registry &pRegistry) {
  this->update_world_matrix(pRegistry);
  return this->mMatrixWorld;
}

void transform::matrix_world(entt::registry &pRegistry,
                             const glm::mat4 &pValue) {
  auto &parentMat = this->matrix_world_inverse_parent(pRegistry);
  this->matrix_local(parentMat * pValue);
}

glm::vec3 transform::position_world(entt::registry &pRegistry) {
  this->update_world_matrix(pRegistry);
  // TODO: This could be optimized.
  glm::vec3 scale;
  glm::quat rotation;
  glm::vec3 position;
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::decompose(this->mMatrixWorld, scale, rotation, position, skew,
                 perspective);
  return position;
}

void transform::position_world(entt::registry &pRegistry,
                               const glm::vec3 &pValue) {
  auto &parentMat = this->matrix_world_inverse_parent(pRegistry);
  glm::vec3 nextPos = glm::vec3(glm::vec4(pValue, 1.0) * parentMat);
  this->position(nextPos);
}

glm::vec3 transform::scale_world(entt::registry &pRegistry) {
  this->update_world_matrix(pRegistry);
  // TODO: This could be optimized.
  glm::vec3 scale;
  glm::quat rotation;
  glm::vec3 position;
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::decompose(this->mMatrixWorld, scale, rotation, position, skew,
                 perspective);
  return scale;
}

void transform::scale_world(entt::registry &pRegistry,
                            const glm::vec3 &pValue) {
  auto prevScale = this->scale_world(pRegistry);
  auto nextScale = pValue / prevScale;
  this->scale(nextScale);
}

glm::quat transform::rotation_world(entt::registry &pRegistry) {
  this->update_world_matrix(pRegistry);
  // TODO: This could be optimized.
  glm::vec3 scale;
  glm::quat rotation;
  glm::vec3 position;
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::decompose(this->mMatrixWorld, scale, rotation, position, skew,
                 perspective);
  return rotation;
}

void transform::rotation_world(entt::registry &pRegistry,
                               const glm::quat &pValue) {
  auto prevRotation = this->rotation_world(pRegistry);
  prevRotation = glm::normalize(prevRotation);
  prevRotation = glm::conjugate(prevRotation);
  auto nextRotation = prevRotation * pValue;
  this->rotation(nextRotation);
}

const glm::mat4 &transform::matrix_world_inverse(entt::registry &pRegistry) {
  this->update_world_inverse_matrix(pRegistry);
  return this->mMatrixWorldInverse;
}

void transform::matrix_world_inverse(entt::registry &pRegistry,
                                     const glm::mat4 &pValue) {
  auto &parentMat = this->matrix_world_inverse_parent(pRegistry);
  this->matrix_local(parentMat * glm::inverse(pValue));
}

void transform::apply_matrix(const glm::mat4 &pValue) {
  this->update_matrix();
  this->mMatrix = pValue * this->mMatrix;
  this->mark_matrix_changed();
}

void transform::translate(const glm::vec3 &pValue) {
  this->update_component();
  this->mPosition += pValue;
  this->mark_component_changed();
}

void transform::rotate(const glm::quat &pValue) {
  this->update_component();
  this->mRotation = pValue * this->mRotation;
  this->mark_component_changed();
}

void transform::rotate_x(float pRad) {
  this->update_component();
  this->mRotation =
      glm::rotate(this->mRotation, pRad, glm::vec3(1.0f, 0.0f, 0.0f));
  this->mark_component_changed();
}

void transform::rotate_y(float pRad) {
  this->update_component();
  this->mRotation =
      glm::rotate(this->mRotation, pRad, glm::vec3(0.0f, 1.0f, 0.0f));
  this->mark_component_changed();
}

void transform::rotate_z(float pRad) {
  this->update_component();
  this->mRotation =
      glm::rotate(this->mRotation, pRad, glm::vec3(0.0f, 0.0f, 1.0f));
  this->mark_component_changed();
}

void transform::rotate_axis(const glm::vec3 &pAxis, float pRad) {
  this->update_component();
  this->mRotation = glm::rotate(this->mRotation, pRad, pAxis);
  this->mark_component_changed();
}

void transform::look_at(const glm::vec3 &pTarget) {
  glm::vec3 to = pTarget - this->position();
  to = glm::normalize(to);

  glm::vec3 rot = glm::cross(glm::vec3(0.0, 0.0, -1.0), to);
  rot = glm::normalize(rot);
  if (glm::length(rot) == 0) {
    rot = glm::vec3(0.0, 1.0, 0.0);
  }

  auto dot = glm::dot(glm::vec3(0.0, 0.0, -1.0), to);
  auto angle = std::acos(dot);

  glm::quat out = glm::identity<glm::quat>();
  out = glm::rotate(out, angle, rot);

  this->rotation(out);
}

glm::mat4 MAT4_IDENTITY;

const glm::mat4 &transform::matrix_world_parent(entt::registry &pRegistry) {
  transform *parent = nullptr;
  if (this->mParent != std::nullopt) {
    parent = pRegistry.try_get<transform>(this->mParent.value());
  }
  if (parent != nullptr) {
    return parent->matrix_world(pRegistry);
  }
  MAT4_IDENTITY = glm::mat4(1.0);
  return MAT4_IDENTITY;
}

const glm::mat4 &
transform::matrix_world_inverse_parent(entt::registry &pRegistry) {
  transform *parent = nullptr;
  if (this->mParent != std::nullopt) {
    parent = pRegistry.try_get<transform>(this->mParent.value());
  }
  if (parent != nullptr) {
    return parent->matrix_world_inverse(pRegistry);
  }
  MAT4_IDENTITY = glm::mat4(1.0);
  return MAT4_IDENTITY;
}

void transform::update_component() {
  if (this->mComponentVersion < this->mMatrixVersion) {
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(this->mMatrix, this->mScale, this->mRotation,
                   this->mPosition, skew, perspective);
    this->mRotation = glm::conjugate(this->mRotation);
    this->mComponentVersion = this->mMatrixVersion;
  }
}

void transform::update_matrix() {
  if (this->mMatrixVersion < this->mComponentVersion) {
    this->mMatrix = glm::mat4(1.0);
    this->mMatrix = glm::translate(this->mMatrix, this->mPosition);
    this->mMatrix *= glm::mat4_cast(this->mRotation);
    this->mMatrix = glm::scale(this->mMatrix, this->mScale);
    this->mMatrixVersion = this->mComponentVersion;
  }
}

void transform::update_world_matrix(entt::registry &pRegistry) {
  auto &transformSys = pRegistry.ctx().get<transform_system>();
  if (transformSys.global_version() == this->mGlobalVersion) {
    return;
  }
  this->update_matrix();
  transform *parent = nullptr;
  if (this->mParent != std::nullopt) {
    parent = pRegistry.try_get<transform>(this->mParent.value());
  }
  if (parent != nullptr) {
    auto &target_matrix = parent->matrix_world(pRegistry);
    auto &target_version = parent->mWorldVersion;
    if (this->mMatrixVersion != this->mWorldMatrixVersion ||
        this->mWorldParentVersion != target_version) {
      this->mMatrixWorld = target_matrix * this->mMatrix;
      this->mWorldMatrixVersion = this->mMatrixVersion;
      this->mWorldParentVersion = target_version;
      this->mWorldVersion += 1;
    }
    this->mGlobalVersion = transformSys.global_version();
  } else {
    if (this->mMatrixVersion != this->mWorldMatrixVersion ||
        this->mWorldParentVersion != 0) {
      this->mMatrixWorld = this->mMatrix;
      this->mWorldMatrixVersion = this->mMatrixVersion;
      this->mWorldParentVersion = 0;
      this->mWorldVersion += 1;
    }
    this->mGlobalVersion = transformSys.global_version();
  }
}

void transform::update_world_inverse_matrix(entt::registry &pRegistry) {
  this->update_world_matrix(pRegistry);
  if (this->mWorldVersion != this->mWorldInverseVersion) {
    this->mMatrixWorldInverse = glm::inverse(this->mMatrixWorld);
    this->mWorldInverseVersion = this->mWorldVersion;
  }
}

void transform::mark_component_changed() {
  this->mComponentVersion += 1;
  auto registry = this->mRegistry;
  if (registry != nullptr) {
    auto &transformSys = registry->ctx().get<transform_system>();
    transformSys.update_global_version();
  }
}

void transform::mark_matrix_changed() {
  this->mMatrixVersion += 1;
  auto registry = this->mRegistry;
  if (registry != nullptr) {
    auto &transformSys = registry->ctx().get<transform_system>();
    transformSys.update_global_version();
  }
}

void transform::mark_parent_indexed() { this->mPreviousParent = this->mParent; }

void transform::register_registry(entt::registry *pRegistry) {
  this->mRegistry = pRegistry;
}
