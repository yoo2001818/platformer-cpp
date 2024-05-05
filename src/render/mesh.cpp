#include "render/mesh.hpp"

using namespace platformer;

mesh::mesh() {}
mesh::mesh(const std::vector<mesh_pair> &pMeshes) : mMeshes(pMeshes) {}
mesh::mesh(std::vector<mesh_pair> &&pMeshes) : mMeshes(pMeshes) {}

std::vector<mesh::mesh_pair> &mesh::meshes() { return this->mMeshes; }

void mesh::render(entt::registry &pRegistry, const entt::entity &pEntity,
                  render_context_root &pRenderContextRoot) {
  auto &transformVal = pRegistry.get<transform>(pEntity);
  int index = 0;
  for (auto &[materialVal, geometryVal] : this->mMeshes) {
    materialVal->render({
        .registry = pRegistry,
        .asset_manager = pRenderContextRoot.asset_manager,
        .aspect = pRenderContextRoot.aspect,
        .entity = pEntity,
        .transform = transformVal,
        .mesh = *this,
        .geometry = *geometryVal,
        .camera_entity = pRenderContextRoot.camera_entity,
        .camera_transform = pRenderContextRoot.camera_transform,
        .camera_camera = pRenderContextRoot.camera_camera,
        .lights = pRenderContextRoot.lights,
    });
  }
}
