#include "transform.hpp"
#include "entt/entity/fwd.hpp"
#include <exception>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <stdexcept>

using namespace platformer;

transform::transform(){};
transform::transform(const entt::entity &pParent) : mParent(pParent){};
transform::transform(const entt::entity &pParent, const glm::mat4 &pMatrix)
    : mParent(pParent), mMatrix(pMatrix), mMatrixVersion(1){};
transform::transform(const glm::mat4 &pMatrix)
    : mMatrix(pMatrix), mMatrixVersion(1){};

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
  this->mParentVersion = -1;
  this->mInverseVersion = -1;
}

const glm::mat4 &transform::matrix_world(entt::registry &pRegistry) {
  this->update_world_matrix();
  return this->mMatrixWorld;
}

void transform::matrix_world(entt::registry &pRegistry,
                             const glm::mat4 &pValue) {
  throw std::logic_error("Not implemented yet");
}

glm::vec3 transform::position_world(entt::registry &pRegistry) {
  throw std::logic_error("Not implemented yet");
}

void transform::position_world(entt::registry &pRegistry,
                               const glm::vec3 &pValue) {
  throw std::logic_error("Not implemented yet");
}

glm::vec3 transform::scale_world(entt::registry &pRegistry) {
  throw std::logic_error("Not implemented yet");
}

void transform::scale_world(entt::registry &pRegistry,
                            const glm::vec3 &pValue) {
  throw std::logic_error("Not implemented yet");
}

glm::quat transform::rotation_world(entt::registry &pRegistry) {
  throw std::logic_error("Not implemented yet");
}

void transform::rotation_world(entt::registry &pRegistry,
                               const glm::quat &pValue) {
  throw std::logic_error("Not implemented yet");
}

const glm::mat4 &transform::matrix_world_inverse(entt::registry &pRegistry) {
  this->update_world_inverse_matrix();
  return this->mMatrixWorldInverse;
}

void transform::matrix_world_inverse(entt::registry &pRegistry,
                                     const glm::mat4 &pValue) {
  throw std::logic_error("Not implemented yet");
}

void transform::apply_matrix(const glm::mat4 &pValue) {
  throw std::logic_error("Not implemented yet");
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
  throw std::logic_error("Not implemented yet");
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

void transform::update_world_matrix() {}

void transform::update_world_inverse_matrix() {}

void transform::mark_component_changed() { this->mComponentVersion += 1; }

void transform::mark_matrix_changed() { this->mMatrixVersion += 1; }
