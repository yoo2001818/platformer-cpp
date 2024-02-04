#include "entt/entity/fwd.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "SDL_video.h"
#include "application.hpp"
#include "game.hpp"
#include "mesh.hpp"
#include "transform.hpp"
#include <GL/glew.h>
#include <algorithm>
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <memory>
#include <numbers>

using namespace platformer;

game::game() : mRegistry() {}

void game::init(application &pApplication) {
  SDL_GetWindowSize(pApplication.window(), &(this->mWindowWidth),
                    &(this->mWindowHeight));
  this->mMovement.init(*this);
  for (int i = 0; i < 10; i++) {
    auto cube = this->mRegistry.create();
    auto &trans = this->mRegistry.emplace<transform>(cube);
    trans.position(glm::vec3(cos(i / 10.0f * 3.14f * 2) * 5.0f, 0.0f,
                             sin(i / 10.0f * 3.14f * 2) * 5.0f));

    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back({std::make_shared<material>(),
                      std::make_shared<geometry>(geometry::make_box())});

    this->mRegistry.emplace<mesh>(cube, std::move(meshes));
  }
  {
    auto cam = this->mRegistry.create();
    auto &transformVal = this->mRegistry.emplace<transform>(cam);
    transformVal.position(glm::vec3(0.0, 0.0, 5.0));
    // transformVal.look_at(glm::vec3(0.0));
    auto &cameraVal = this->mRegistry.emplace<camera>(cam);
    cameraVal.type = camera::PERSPECTIVE;
    cameraVal.near = 0.1f;
    cameraVal.far = 100.0f;
    cameraVal.fov = glm::radians(90.0f);
    this->mRegistry.emplace<movement>(cam);
    this->mCamera = cam;
    this->mMovement.controlling_entity(cam);
  }
}

void game::update(application &pApplication, float pDelta) {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  this->mMovement.update(*this, pDelta);

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
  this->mMovement.handle_event(*this, pEvent);
}

entt::registry &game::registry() { return this->mRegistry; }
