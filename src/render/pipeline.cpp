#include "render/pipeline.hpp"
#include "game.hpp"
#include "render/mesh.hpp"
#include "render/render.hpp"
#include "render/renderer.hpp"

using namespace platformer;

pipeline::pipeline(renderer &pRenderer) : mRenderer(pRenderer) {}

void pipeline::render() {
  auto &registry = this->mRenderer.game().registry();
  auto cameraEntity = this->mRenderer.camera();
  auto &cameraTransform = registry.get<transform>(cameraEntity);
  auto &cameraCamera = registry.get<platformer::camera>(cameraEntity);
  std::vector<render_light> lights = this->collect_lights();
  render_context_root renderContextRoot{
      .registry = registry,
      .asset_manager = this->mRenderer.asset_manager(),
      .aspect = static_cast<float>(this->mRenderer.width()) /
                static_cast<float>(this->mRenderer.height()),
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

std::vector<render_light> pipeline::collect_lights() {
  auto &registry = this->mRenderer.game().registry();
  std::vector<render_light> lights;
  auto view = registry.view<transform, light>();
  for (auto entity : view) {
    auto &lightVal = registry.get<light>(entity);
    auto &transformVal = registry.get<transform>(entity);
    render_light renderLight{
        .position = glm::vec4(transformVal.position_world(registry), 1.0f),
        .color = lightVal.color,
        .range = glm::vec3(lightVal.power / std::numbers::pi, lightVal.radius,
                           lightVal.range)};
    lights.push_back(renderLight);
  }
  return lights;
}
