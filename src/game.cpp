#include "entt/entity/fwd.hpp"
#include "physics.hpp"
#include <fstream>
#include <glm/geometric.hpp>
#include <sstream>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_mouse.h"
#include "SDL_video.h"
#include "application.hpp"
#include "file.hpp"
#include "game.hpp"
#include "render/load.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
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
  this->mName.init(this->mRegistry);
  this->mTransform.init(this->mRegistry);
  this->mMovement.init(*this);
  for (int i = 0; i < 10; i++) {
    auto cube = this->mRegistry.create();
    auto &trans = this->mRegistry.emplace<transform>(cube);
    trans.position(glm::vec3(cos(i / 10.0f * 3.14f * 2) * 5.0f, 0.0f,
                             sin(i / 10.0f * 3.14f * 2) * 5.0f));

    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back(
        {std::make_shared<standard_material>(glm::vec3(1.0f), 0.5f, 0.0f),
         std::make_shared<geometry>(geometry::make_box())});

    this->mRegistry.emplace<mesh>(cube, std::move(meshes));
    this->mRegistry.emplace<collision>(cube);
  }
  {
    auto cube = this->mRegistry.create();
    auto &trans = this->mRegistry.emplace<transform>(cube);
    trans.position(glm::vec3(0.0f, 0.0f, 0.0f));

    mesh model = load_file_to_mesh("res/bunny.gltf");
    this->mRegistry.emplace<mesh>(cube, model);
    this->mRegistry.emplace<collision>(cube);
  }
  /*
  {
    auto cube = this->mRegistry.create();
    auto &trans = this->mRegistry.emplace<transform>(cube);
    trans.position(glm::vec3(0.5f, 3.0f, 0.5f));

    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back(
        {std::make_shared<standard_material>(glm::vec3(1.0f), 0.5f, 0.0f),
         std::make_shared<geometry>(geometry::make_box())});

    this->mRegistry.emplace<mesh>(cube, std::move(meshes));
    this->mRegistry.emplace<collision>(cube);
    this->mRegistry.emplace<physics>(cube);
  }
  */
  {
    auto cam = this->mRegistry.create();
    auto &transformVal = this->mRegistry.emplace<transform>(cam);
    transformVal.position(glm::vec3(0.0, 10.0, 0.0));
    transformVal.look_at(glm::vec3(0.0));
    auto &cameraVal = this->mRegistry.emplace<camera>(cam);
    cameraVal.type = camera::PERSPECTIVE;
    cameraVal.near = 0.1f;
    cameraVal.far = 100.0f;
    cameraVal.fov = glm::radians(90.0f);
    this->mCamera = cam;
  }
  {
    auto player = this->mRegistry.create();
    auto &transformVal = this->mRegistry.emplace<transform>(player);
    transformVal.scale(glm::vec3(0.3f, 1.0f, 0.3f));
    transformVal.position(glm::vec3(0.0, 5.0, 0.0));
    // transformVal.look_at(glm::vec3(0.0));
    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back({std::make_shared<standard_material>(
                          glm::vec3(1.0f, 0.1f, 0.1f), 0.5f, 0.0f),
                      std::make_shared<geometry>(geometry::make_box())});

    this->mRegistry.emplace<mesh>(player, std::move(meshes));
    this->mRegistry.emplace<fps_movement>(player);
    this->mRegistry.emplace<physics>(player);
    this->mRegistry.emplace<collision>(player);
    this->mMovement.controlling_entity(player);
    this->mPlayer = player;
  }
  {
    auto light = this->mRegistry.create();
    auto &transformVal = this->mRegistry.emplace<transform>(light);
    transformVal.position(glm::vec3(0.0, 10.0, 2.0));
    auto &lightVal = this->mRegistry.emplace<platformer::light>(light);
    lightVal.color = glm::vec3(1.0, 1.0, 1.0);
    lightVal.power = 1.0f;
    lightVal.radius = 0.1f;
    lightVal.range = 100.0f;
  }
}

void game::update(application &pApplication, float pDelta) {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // follow player
  {
    auto &camTransform = this->mRegistry.get<transform>(this->mCamera);
    auto &playerTransform = this->mRegistry.get<transform>(this->mPlayer);
    glm::vec3 eyeDir = glm::normalize(
        glm::vec3(glm::normalize(playerTransform.matrix_world(this->mRegistry) *
                                 glm::vec4(0.0, 0.0, 1.0, 0.0))) *
        glm::vec3(1.0f, 0.0f, 1.0f));
    if (!std::isnan(eyeDir.x)) {
      camTransform.translate((playerTransform.position() + eyeDir * 5.0f +
                              glm::vec3(0.0f, 2.0f, 0.0f) -
                              camTransform.position()) *
                             0.1f);
      glm::quat quat = glm::identity<glm::quat>();
      glm::vec3 diff = camTransform.position() - playerTransform.position();
      quat = glm::rotate(quat, std::atan2(diff.x, diff.z),
                         glm::vec3(0.0, 1.0, 0.0));
      quat = glm::rotate(quat, -0.3f, glm::vec3(1.0, 0.0, 0.0));
      camTransform.rotation(quat);
      // camTransform.look_at(playerTransform.position());
    }
  }

  this->mMovement.update(*this, pDelta);
  this->mPhysics.update(*this, pDelta);
  this->mRenderer.render(*this, this->mCamera,
                         static_cast<float>(this->mWindowWidth) /
                             static_cast<float>(this->mWindowHeight));
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
