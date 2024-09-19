#ifndef __RENDER_LOAD_HPP__
#define __RENDER_LOAD_HPP__
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "entt/entity/fwd.hpp"
#include "render/armature.hpp"
#include "render/geometry.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

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
  std::vector<std::shared_ptr<material>> mMaterials;
  std::vector<std::shared_ptr<geometry>> mGeometries;
  std::unordered_map<aiNode *, entt::entity> mEntities;
  std::unordered_map<std::string, entt::entity> mEntityByNames;

  std::shared_ptr<material> read_material(int pIndex);
  std::shared_ptr<geometry> read_geometry(int pIndex);
  mesh::mesh_pair read_mesh(int pIndex);
  armature_component read_mesh_armature(int pIndex);
  void iterate_entity(aiNode *pNode, entt::entity pParent);
  void attach_entity(aiNode *pNode, entt::entity pEntity);
};

void load_file_to_entity(const std::string &pFilename,
                         entt::registry &pRegistry);
} // namespace platformer

#endif
