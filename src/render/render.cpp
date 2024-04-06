#include "render/render.hpp"

using namespace platformer;

glm::mat4 camera::getProjection(float pAspect) {
  switch (this->type) {
  case ORTHOGRAPHIC:
    return glm::ortho(-1.0f, 1.0f, -1.0f / pAspect, 1.0f / pAspect, this->near,
                      this->far);
  case PERSPECTIVE:
    return glm::perspective(this->fov, pAspect, this->near, this->far);
  default:
    return glm::mat4();
  }
}
