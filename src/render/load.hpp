#ifndef __RENDER_LOAD_HPP__
#define __RENDER_LOAD_HPP__
#include "entt/entity/fwd.hpp"
#include "render/mesh.hpp"
#include <glm/glm.hpp>
#include <string>

namespace platformer {
// This should be enough to experiment with assimp
mesh load_file_to_mesh(const std::string &pFilename);

void load_file_to_entity(const std::string &pFilename,
                         entt::registry &pRegistry);
} // namespace platformer

#endif
