#include "renderer.hpp"
#include "game.hpp"
#include "render/mesh.hpp"
#include "transform.hpp"
#include <vector>

using namespace platformer;

renderer::renderer() {}

std::vector<render_light> renderer::get_lights(game &pGame) {
  auto &registry = pGame.registry();
  std::vector<render_light> lights;
  auto view = registry.view<transform, light>();
  for (auto entity : view) {
    auto &lightVal = registry.get<light>(entity);
    auto &transformVal = registry.get<transform>(entity);
    render_light renderLight{
        .position = glm::vec4(transformVal.position_world(registry), 1.0f),
        .color = lightVal.color,
        .range = glm::vec3(lightVal.range, lightVal.power, lightVal.radius)};
    lights.push_back(renderLight);
  }
  return lights;
}

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
