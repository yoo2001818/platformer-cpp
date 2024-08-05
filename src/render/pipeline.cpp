#include "render/pipeline.hpp"
#include "game.hpp"
#include "render/mesh.hpp"
#include "render/render.hpp"
#include "render/renderer.hpp"
#include <vector>

using namespace platformer;

pipeline::pipeline(renderer &pRenderer) : mRenderer(pRenderer) {}

void pipeline::render() {
  auto &registry = this->mRenderer.game().registry();
  auto cameraEntity = this->mRenderer.camera();
  auto &cameraTransform = registry.get<transform>(cameraEntity);
  auto &cameraCamera = registry.get<platformer::camera>(cameraEntity);
  this->collect_lights();
  auto view = registry.view<transform, mesh>();
  for (auto entity : view) {
    auto &meshVal = registry.get<mesh>(entity);
    meshVal.render(this->mRenderer, entity);
  }
}

const std::vector<render_light> &pipeline::get_lights() const {
  return this->mLights;
}

void pipeline::collect_lights() {
  auto &registry = this->mRenderer.game().registry();
  this->mLights.clear();
  auto view = registry.view<transform, light>();
  for (auto entity : view) {
    auto &lightVal = registry.get<light>(entity);
    auto &transformVal = registry.get<transform>(entity);
    render_light renderLight{
        .position = glm::vec4(transformVal.position_world(registry), 1.0f),
        .color = lightVal.color,
        .range = glm::vec3(lightVal.power / std::numbers::pi, lightVal.radius,
                           lightVal.range)};
    this->mLights.push_back(renderLight);
  }
}
