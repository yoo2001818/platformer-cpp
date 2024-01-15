#ifndef __TRANSFORM_HPP__
#define __TRANSFORM_HPP__

#include "entt/entity/fwd.hpp"
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <optional>

namespace platformer {
class transform {
public:
  transform();
  transform(const entt::entity &pParent);
  transform(const entt::entity &pParent, const glm::mat4 &pMatrix);
  transform(const glm::mat4 &pMatrix);

  const glm::vec3 &position();
  void position(const glm::vec3 &pValue);
  const glm::vec3 &scale();
  void scale(const glm::vec3 &pValue);
  const glm::quat &rotation();
  void rotation(const glm::quat &pValue);
  const glm::mat4 &matrix_local();
  void matrix_local(const glm::mat4 &pValue);
  const std::optional<entt::entity> &parent() const;
  void parent(const std::optional<entt::entity> &pParent);
  // FIXME: Is this a good idea? Arguably this is better than storing a pointer
  // to the registry.
  const glm::mat4 &matrix_world(entt::registry &pRegistry);
  void matrix_world(entt::registry &pRegistry, const glm::mat4 &pValue);
  glm::vec3 position_world(entt::registry &pRegistry);
  void position_world(entt::registry &pRegistry, const glm::vec3 &pValue);
  glm::vec3 scale_world(entt::registry &pRegistry);
  void scale_world(entt::registry &pRegistry, const glm::vec3 &pValue);
  glm::quat rotation_world(entt::registry &pRegistry);
  void rotation_world(entt::registry &pRegistry, const glm::quat &pValue);
  const glm::mat4 &matrix_world_inverse(entt::registry &pRegistry);
  void matrix_world_inverse(entt::registry &pRegistry, const glm::mat4 &pValue);

  void apply_matrix(const glm::mat4 &pValue);
  void translate(const glm::vec3 &pValue);
  void rotate(const glm::quat &pValue);
  void rotate_x(float pRad);
  void rotate_y(float pRad);
  void rotate_z(float pRad);
  void rotate_axis(const glm::vec3 &pAxis, float pRad);

  void look_at(const glm::vec3 &pTarget);

private:
  void update_component();
  void update_matrix();
  void update_world_matrix();
  void update_world_inverse_matrix();
  void mark_component_changed();
  void mark_matrix_changed();

  glm::vec3 mPosition{};
  glm::vec3 mScale{1.0};
  glm::quat mRotation{0.0, 0.0, 0.0, 1.0};
  glm::mat4 mMatrix{1.0};
  int mComponentVersion = 0;
  int mMatrixVersion = 0;
  int mParentVersion = 0;
  int mInverseVersion = 0;
  glm::mat4 mMatrixWorld{1.0};
  glm::mat4 mMatrixWorldInverse{1.0};
  std::optional<entt::entity> mParent = std::nullopt;
};
} // namespace platformer

#endif // __TRANSFORM_HPP__
