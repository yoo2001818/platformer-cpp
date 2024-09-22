#ifndef __RENDER_LOAD_HPP__
#define __RENDER_LOAD_HPP__
#include "animation/animation.hpp"
#include "assimp/anim.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "entt/entity/fwd.hpp"
#include "geometry/geometry.hpp"
#include "material/material.hpp"
#include "scenegraph/armature.hpp"
#include "scenegraph/mesh.hpp"
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

  std::shared_ptr<texture> read_texture(std::string pFilename);
  std::shared_ptr<material> read_material(int pIndex);
  std::shared_ptr<geometry> read_geometry(int pIndex);
  mesh::mesh_pair read_mesh(int pIndex);
  armature_component read_mesh_armature(int pIndex);
  void iterate_entity(aiNode *pNode, entt::entity pParent);
  void attach_entity(aiNode *pNode, entt::entity pEntity);
  animation_action read_animation(int pIndex);
  void read_animation_all();
};

void load_file_to_entity(const std::string &pFilename,
                         entt::registry &pRegistry);
} // namespace platformer

#endif