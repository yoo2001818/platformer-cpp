#include "armature.hpp"
#include "entt/entt.hpp"
#include "transform.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <vector>

using namespace platformer;

std::vector<glm::mat4>
armature_component::bone_matrices(entt::registry &pRegistry) {
  auto rootTransform = pRegistry.try_get<transform>(this->root);
  glm::mat4 invMatrix;
  if (rootTransform != nullptr) {
    invMatrix = rootTransform->matrix_world_inverse(pRegistry);
  } else {
    invMatrix = glm::identity<glm::mat4>();
  }

  std::vector<glm::mat4> matrices;
  matrices.reserve(this->bones.size());
  for (auto &bone : this->bones) {
    auto transformVal = pRegistry.try_get<transform>(bone.joint);
    if (transformVal != nullptr) {
      matrices.push_back(invMatrix * transformVal->matrix_world(pRegistry));
    } else {
      matrices.push_back(glm::identity<glm::mat4>());
    }
  }

  return matrices;
}
