#include "render/camera.hpp"
#include "render/renderer.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/glm.hpp>
#include <stdexcept>

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

camera_handle::camera_handle(renderer &pRenderer) : mRenderer(pRenderer) {
  auto entity = pRenderer.camera();
  auto &registry = pRenderer.registry();
  this->mTransform = registry.try_get<transform>(entity);
  this->mCamera = registry.try_get<camera>(entity);
  if (this->mTransform == nullptr || this->mCamera == nullptr) {
    throw std::runtime_error("No valid camera object was specified");
  }
}

glm::mat4 camera_handle::view() {
  return this->mTransform->matrix_world_inverse(this->mRenderer.registry());
}

glm::mat4 camera_handle::projection() {
  float aspect = static_cast<float>(this->mRenderer.width()) /
                 static_cast<float>(this->mRenderer.height());
  return this->mCamera->getProjection(aspect);
}

glm::mat4 camera_handle::inverse_view() {
  return this->mTransform->matrix_world(this->mRenderer.registry());
}

glm::mat4 camera_handle::inverse_projection() {
  return glm::inverse(this->projection());
}

glm::vec3 camera_handle::view_pos() {
  return this->mTransform->position_world(this->mRenderer.registry());
}
