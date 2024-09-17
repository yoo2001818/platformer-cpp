#ifndef __RENDER_LOAD_HPP__
#define __RENDER_LOAD_HPP__
#include "assimp/material.h"
#include "assimp/scene.h"
#include "entt/entity/fwd.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
#include <glm/glm.hpp>
#include <string>

namespace platformer {
// This should be enough to experiment with assimp
mesh load_file_to_mesh(const std::string &pFilename);

class entity_loader {
public:
  entity_loader(const std::string &pFilename, entt::registry &pRegistry);

  void load();

private:
  std::string mFilename;
  entt::registry &mRegistry;
  const aiScene *mScene = nullptr;

  std::shared_ptr<material> read_material(aiMaterial *pMaterial);
  mesh::mesh_pair read_mesh(aiMesh *pMesh);
  void iterate_entity(aiNode *pNode, entt::entity pParent);
};

void load_file_to_entity(const std::string &pFilename,
                         entt::registry &pRegistry);
} // namespace platformer

#endif
