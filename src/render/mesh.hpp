#ifndef __RENDER_MESH_HPP__
#define __RENDER_MESH_HPP__

#include "render/geometry.hpp"
#include "render/material.hpp"

namespace platformer {
class renderer;
class mesh {
public:
  using mesh_pair =
      std::pair<std::shared_ptr<material>, std::shared_ptr<geometry>>;
  mesh();
  mesh(const std::vector<mesh_pair> &pMeshes);
  mesh(std::vector<mesh_pair> &&pMeshes);

  std::vector<mesh_pair> &meshes();
  bool shouldRender() const;
  void shouldRender(bool mValue);

  void render(renderer &pRenderer, entt::entity pEntity);

private:
  std::vector<mesh_pair> mMeshes;
  bool mShouldRender = true;
};

}; // namespace platformer
#endif
