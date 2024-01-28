#include "game.hpp"
#include "mesh.hpp"
#include "transform.hpp"
#include <memory>

using namespace platformer;

game::game() : mRegistry() {}

void game::init() {
  {
    auto cube = this->mRegistry.create();
    this->mRegistry.emplace<transform>(cube);

    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back({std::make_shared<material>(),
                      std::make_shared<geometry>(geometry::make_box())});

    this->mRegistry.emplace<mesh>(cube, std::move(meshes));
  }
  {
    auto cam = this->mRegistry.create();
    this->mRegistry.emplace<transform>(cam);
    this->mRegistry.emplace<camera>(cam);
    this->mCamera = cam;
  }
}

void game::update(float pDelta) {
  auto &cameraEntity = this->mCamera;
  auto &cameraTransform = this->mRegistry.get<transform>(cameraEntity);
  auto &cameraCamera = this->mRegistry.get<camera>(cameraEntity);
  render_context_root renderContextRoot{
      .registry = this->mRegistry,
      .aspect = 1.0f,
      .camera_entity = cameraEntity,
      .camera_transform = cameraTransform,
      .camera_camera = cameraCamera,
  };
  auto view = mRegistry.view<transform, mesh>();
  for (auto entity : view) {
    auto &meshVal = mRegistry.get<mesh>(entity);
    meshVal.render(this->mRegistry, entity, renderContextRoot);
  }
}

void game::dispose() {}

void game::handle_event(SDL_Event &pEvent) {}
