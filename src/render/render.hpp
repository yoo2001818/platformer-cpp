#ifndef __RENDER_RENDER_HPP__
#define __RENDER_RENDER_HPP__
#include "render/geometry.hpp"
#include "transform.hpp"
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
namespace platformer {
class mesh;

class camera {
public:
  // FIXME: This is against the convention; it could and should be improved
  // in the future
  enum { PERSPECTIVE, ORTHOGRAPHIC } type;
  float near;
  float far;
  float fov;

  glm::mat4 getProjection(float pAspect);
};

class light {
public:
  glm::vec3 color;
  float power;
  float radius;
  float range;
};

struct render_light {
  glm::vec4 position;
  glm::vec3 color;
  glm::vec3 range;
};

struct render_context {
  // This contains all the information needed to render the object -
  // For example, the entity, the framebuffer to render to, the list of lights,
  // and so more.
  // Material will read this data and set it as uniforms. (Consider using UBOs,
  // or even better, instancing)
  // The current structure necessitiates for material to be aware of armatures,
  // but that is simply unavoidable at this point.
  entt::registry &registry;
  float aspect;

  const entt::entity &entity;
  platformer::transform &transform;
  platformer::mesh &mesh;
  platformer::geometry &geometry;

  const entt::entity &camera_entity;
  platformer::transform &camera_transform;
  platformer::camera &camera_camera;

  std::vector<render_light> &lights;
};

struct render_context_root {
  // Root-level render context.
  entt::registry &registry;
  float aspect;

  const entt::entity &camera_entity;
  platformer::transform &camera_transform;
  platformer::camera &camera_camera;

  std::vector<render_light> &lights;
};
} // namespace platformer

#endif
