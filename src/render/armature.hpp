#ifndef __RENDER_ARMATURE_HPP__
#define __RENDER_ARMATURE_HPP__

#include "entt/entity/fwd.hpp"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <vector>
namespace platformer {
struct bone {
  glm::mat4 inverseBindMatrix;
  entt::entity joint;
};

class armature_component {
public:
  std::vector<glm::mat4> bone_matrices(entt::registry &pRegistry);

  std::vector<bone> bones;
  entt::entity root;
};

} // namespace platformer

#endif
