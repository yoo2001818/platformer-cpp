#include "entt/entity/fwd.hpp"
#include "name.hpp"
#include "physics.hpp"
#include "render/geometry.hpp"
#include "render/renderer.hpp"
#include "render/shader.hpp"
#include "scene/scene.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/geometric.hpp>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include "SDL_events.h"
#include "SDL_video.h"
#include "application.hpp"
#include "debug_ui.hpp"
#include "game.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
#include "transform.hpp"
#include <GL/glew.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <memory>

using namespace platformer;

game::game() : mRegistry() {}

void game::init(application &pApplication) {
  this->mApplication = &pApplication;
  SDL_GL_GetDrawableSize(pApplication.window(), &(this->mWindowWidth),
                         &(this->mWindowHeight));
  this->mName.init(this->mRegistry);
  this->mTransform.init(this->mRegistry);
  this->mMovement.init(*this);

  this->change_scene(scene_registry::getScenes()[0]);
}

void game::update(application &pApplication, float pDelta) {
  glViewport(0, 0, this->mWindowWidth, this->mWindowHeight);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  if (this->mScene != nullptr) {
    this->mScene->update(pApplication, *this, pDelta);
  }

  this->mMovement.update(*this, pDelta);
  this->mPhysics.update(*this, pDelta);
  this->mDebugUi.update(*this, pDelta);
  this->mRenderer.render(*this, this->mCamera,
                         static_cast<float>(this->mWindowWidth) /
                             static_cast<float>(this->mWindowHeight));
}

void game::dispose() {}

void game::handle_event(application &pApplication, SDL_Event &pEvent) {
  if (pEvent.type == SDL_WINDOWEVENT &&
      pEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_GL_GetDrawableSize(pApplication.window(), &(this->mWindowWidth),
                           &(this->mWindowHeight));
    glViewport(0, 0, this->mWindowWidth, this->mWindowHeight);
  }
  this->mMovement.handle_event(*this, pEvent);
}

entt::registry &game::registry() { return this->mRegistry; }

void game::make_player() {
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
    this->mRegistry.emplace<name>(cam, "camera");
    this->mMovement.camera_entity(cam);
  }
  {
    auto player = this->mRegistry.create();
    {
      auto &transformVal = this->mRegistry.emplace<transform>(player);
      transformVal.position(glm::vec3(0.0, 5.0, 0.0));

      auto playerBody = this->mRegistry.create();
      auto &transformBodyVal = this->mRegistry.emplace<transform>(playerBody);
      transformBodyVal.scale(glm::vec3(0.3f, 1.0f, 0.3f));
      transformBodyVal.parent(player);
      std::vector<mesh::mesh_pair> meshes{};
      meshes.push_back({std::make_shared<standard_material>(
                            glm::vec3(1.0f, 0.1f, 0.1f), 0.5f, 0.0f),
                        std::make_shared<geometry>(geometry::make_box())});

      this->mRegistry.emplace<mesh>(playerBody, std::move(meshes));
      this->mRegistry.emplace<name>(playerBody, "playerBody");
      this->mMovement.body_mesh_entity(playerBody);

      this->mRegistry.emplace<fps_movement>(player);
      // this->mRegistry.emplace<physics>(player);
      this->mRegistry.emplace<collision>(player, glm::vec3(-0.15, -1.0, -0.15),
                                         glm::vec3(0.15, 1.0, 0.15));
      this->mRegistry.emplace<name>(player, "player");
      this->mMovement.body_entity(player);
      this->mPlayer = player;
    }

    auto playerHead = this->mRegistry.create();
    {
      auto &transformVal = this->mRegistry.emplace<transform>(playerHead);
      transformVal.scale(glm::vec3(0.3f, 0.3f, 0.3f));
      transformVal.position(glm::vec3(0.0, 1.0, 0.0));
      transformVal.parent(player);
      std::vector<mesh::mesh_pair> meshes{};
      meshes.push_back({std::make_shared<standard_material>(
                            glm::vec3(0.1f, 1.0f, 0.1f), 0.5f, 0.0f),
                        std::make_shared<geometry>(geometry::make_box())});

      this->mRegistry.emplace<mesh>(playerHead, std::move(meshes));
      this->mRegistry.emplace<fps_movement>(playerHead);
      this->mRegistry.emplace<name>(playerHead, "playerHead");
      this->mMovement.head_entity(playerHead);
      this->mPlayerHead = playerHead;
    }
  }
}

void game::change_scene(std::shared_ptr<scene> &pScene) {
  if (this->mScene != nullptr) {
    this->mScene->dispose();
  }
  this->mRegistry.clear();
  this->make_player();
  this->mScene = pScene;
  if (this->mScene != nullptr) {
    this->mScene->init(*(this->mApplication), *this);
  }
}

const std::shared_ptr<scene> &game::current_scene() const {
  return this->mScene;
}
