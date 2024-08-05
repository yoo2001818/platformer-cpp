#include "render/mesh.hpp"
#include "render/renderer.hpp"
#include "transform.hpp"

using namespace platformer;

mesh::mesh() {}
mesh::mesh(const std::vector<mesh_pair> &pMeshes) : mMeshes(pMeshes) {}
mesh::mesh(std::vector<mesh_pair> &&pMeshes) : mMeshes(pMeshes) {}

std::vector<mesh::mesh_pair> &mesh::meshes() { return this->mMeshes; }

bool mesh::shouldRender() const { return this->mShouldRender; }

void mesh::shouldRender(bool mValue) { this->mShouldRender = mValue; }

void mesh::render(renderer &pRenderer, entt::entity pEntity) {
  if (!this->mShouldRender)
    return;
  auto &registry = pRenderer.registry();
  auto &transformVal = registry.get<transform>(pEntity);
  int index = 0;
  for (auto &[materialVal, geometryVal] : this->mMeshes) {
    materialVal->render(pRenderer, *geometryVal, pEntity);
  }
}
