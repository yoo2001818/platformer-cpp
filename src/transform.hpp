#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include "entt/entity/fwd.hpp"
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <optional>

namespace platformer {
class transform {
public:
  transform();
  transform(const entt::entity &pParent);
  transform(const entt::entity &pParent, const glm::mat4 &pMatrix);
  transform(const glm::mat4 &pMatrix);

  glm::vec3 position();
  void position(const glm::vec3 &pValue);
  glm::vec3 scale();
  void scale(const glm::vec3 &pValue);
  glm::quat rotation();
  void rotation(const glm::quat &pValue);
  glm::mat4 matrix_local();
  void matrix_local(const glm::mat4 &pValue);
  const std::optional<entt::entity> &parent() const;
  void parent(const std::optional<entt::entity> &pParent);
  // FIXME: Is this a good idea? Arguably this is better than storing a pointer
  // to the registry.
  glm::mat4 matrix_world(entt::registry &pRegistry);
  void matrix_world(entt::registry &pRegistry, const glm::mat4 &pValue);
  glm::vec3 position_world(entt::registry &pRegistry);
  void position_world(entt::registry &pRegistry, const glm::vec3 &pValue);
  glm::vec3 scale_world(entt::registry &pRegistry);
  void scale_world(entt::registry &pRegistry, const glm::vec3 &pValue);
  glm::quat rotation_world(entt::registry &pRegistry);
  void rotation_world(entt::registry &pRegistry, const glm::quat &pValue);
  glm::mat4 matrix_inverse_world(entt::registry &pRegistry);
  void matrix_inverse_world(entt::registry &pRegistry, const glm::mat4 &pValue);

  void apply_matrix(const glm::mat4 &pValue);
  void translate(const glm::vec3 &pValue);
  void rotate(const glm::quat &pValue);
  void rotate_x(float pRad);
  void rotate_y(float pRad);
  void rotate_z(float pRad);
  void rotate_axis(const glm::vec3 &pAxis, float pRad);

  void look_at(const glm::vec3 &pTarget);

private:
  glm::vec3 mPosition{};
  glm::vec3 mScale{};
  glm::vec3 mRotation{};
  glm::mat4 mMatrix{1.0};
  int mComponentVersion;
  int mMatrixVersion;
  int mParentVersion;
  int mInverseVersion;
  glm::mat4 mWorldMatrix{1.0};
  glm::mat4 mWorldInverseMatrix{1.0};
  std::optional<entt::entity> mParent;
};
} // namespace platformer

#endif // __TRANSFORM_HPP__
