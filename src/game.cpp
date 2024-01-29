#include "game.hpp"
#include "SDL_events.h"
#include "SDL_video.h"
#include "application.hpp"
#include "mesh.hpp"
#include "transform.hpp"
#include <GL/glew.h>
#include <memory>

using namespace platformer;

game::game() : mRegistry() {}

void game::init(application &pApplication) {
  SDL_GetWindowSize(pApplication.window(), &(this->mWindowWidth),
                    &(this->mWindowHeight));
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
    auto &transformVal = this->mRegistry.emplace<transform>(cam);
    transformVal.position(glm::vec3(0.0, 5.0, 5.0));
    transformVal.look_at(glm::vec3(0.0));
    auto &cameraVal = this->mRegistry.emplace<camera>(cam);
    cameraVal.type = camera::PERSPECTIVE;
    cameraVal.near = 0.1f;
    cameraVal.far = 100.0f;
    cameraVal.fov = glm::radians(120.0f);
    this->mCamera = cam;
  }
}

void game::update(application &pApplication, float pDelta) {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  auto &cameraEntity = this->mCamera;
  auto &cameraTransform = this->mRegistry.get<transform>(cameraEntity);
  auto &cameraCamera = this->mRegistry.get<camera>(cameraEntity);
  render_context_root renderContextRoot{
      .registry = this->mRegistry,
      .aspect = static_cast<float>(this->mWindowWidth) /
                static_cast<float>(this->mWindowHeight),
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

void game::handle_event(application &pApplication, SDL_Event &pEvent) {
  if (pEvent.type == SDL_WINDOWEVENT &&
      pEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
    this->mWindowWidth = pEvent.window.data1;
    this->mWindowHeight = pEvent.window.data2;
    glViewport(0, 0, this->mWindowWidth, this->mWindowHeight);
  }
}
