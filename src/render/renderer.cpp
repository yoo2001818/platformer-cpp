#include "render/renderer.hpp"
#include "game.hpp"
#include "render/mesh.hpp"
#include "transform.hpp"
#include <numbers>
#include <vector>

using namespace platformer;

renderer::renderer(game &pGame) : mGame(pGame), mRegistry(pGame.registry()) {}

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
        .range = glm::vec3(lightVal.power / std::numbers::pi, lightVal.radius,
                           lightVal.range)};
    lights.push_back(renderLight);
  }
  return lights;
}

void renderer::apply_render_state(const render_state &to) {
  platformer::apply_render_state(this->mRenderState, to);
}

void renderer::init() {
  SDL_GL_GetDrawableSize(this->mGame.app().window(), &(this->mWidth),
                         &(this->mHeight));
}

void renderer::clear() {
  glViewport(0, 0, this->mWidth, this->mHeight);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
}

void renderer::render() {
  auto &registry = this->mRegistry;
  auto &cameraEntity = this->mCamera;
  auto &cameraTransform = registry.get<transform>(cameraEntity);
  auto &cameraCamera = registry.get<platformer::camera>(cameraEntity);
  std::vector<render_light> lights = this->get_lights(this->mGame);
  render_context_root renderContextRoot{
      .registry = registry,
      .asset_manager = mAssetManager,
      .aspect =
          static_cast<float>(this->mWidth) / static_cast<float>(this->mHeight),
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

entt::entity renderer::camera() const { return mCamera; }

void renderer::camera(entt::entity pValue) { mCamera = pValue; }

int renderer::width() const { return mWidth; }

void renderer::width(int pValue) { mWidth = pValue; }

int renderer::height() const { return mHeight; }

void renderer::height(int pValue) { mHeight = pValue; }
