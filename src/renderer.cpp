#include "renderer.hpp"
#include "game.hpp"

using namespace platformer;

renderer::renderer() {}

std::vector<render_light> renderer::get_lights(game &pGame) { return {}; }

void renderer::render(game &pGame, const entt::entity pCamera, float pAspect) {
  auto &registry = pGame.registry();
  auto &cameraEntity = pCamera;
  auto &cameraTransform = registry.get<transform>(cameraEntity);
  auto &cameraCamera = registry.get<camera>(cameraEntity);
  std::vector<render_light> lights = this->get_lights(pGame);
  render_context_root renderContextRoot{
      .registry = registry,
      .aspect = pAspect,
      .camera_entity = cameraEntity,
      .camera_transform = cameraTransform,
      .camera_camera = cameraCamera,
      .lights = lights,
  };
  auto view = registry.view<transform, mesh>();
  for (auto entity : view) {
    auto &meshVal = registry.get<mesh>(entity);
    meshVal.render(registry, entity, renderContextRoot);
  }
}
