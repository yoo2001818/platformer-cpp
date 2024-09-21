#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "scenegraph/transform.hpp"
#include <glm/glm.hpp>

namespace platformer {
class renderer;
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
class camera_handle {
public:
  camera_handle(renderer &pRenderer);
  glm::mat4 view();
  glm::mat4 projection();
  glm::mat4 inverse_view();
  glm::mat4 inverse_projection();
  glm::vec3 view_pos();

private:
  renderer &mRenderer;
  transform *mTransform;
  camera *mCamera;
};
} // namespace platformer

#endif
