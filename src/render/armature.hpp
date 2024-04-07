#ifndef __RENDER_ARMATURE_HPP__
#define __RENDER_ARMATURE_HPP__

#include "entt/entity/fwd.hpp"
#include <glm/glm.hpp>
#include <optional>
#include <vector>
namespace platformer {
class armature {
public:
  std::vector<glm::mat4> inverseBindMatrices;
  std::vector<entt::entity> joints;
  std::optional<entt::entity> skeleton;
};
} // namespace platformer

#endif
