#include "entt/entity/fwd.hpp"
#include "file.hpp"
#include "name.hpp"
#include "physics.hpp"
#include "render/shader.hpp"
#include "render/texture.hpp"
#include <glm/geometric.hpp>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include "SDL_events.h"
#include "SDL_video.h"
#include "application.hpp"
#include "debug_ui.hpp"
#include "game.hpp"
#include "render/load.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
#include "transform.hpp"
#include <GL/glew.h>
#include <cmath>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <memory>

using namespace platformer;

game::game() : mRegistry() {}

void game::init(application &pApplication) {
  SDL_GetWindowSize(pApplication.window(), &(this->mWindowWidth),
                    &(this->mWindowHeight));
  this->mName.init(this->mRegistry);
  this->mTransform.init(this->mRegistry);
  this->mMovement.init(*this);
  // load_file_to_entity("res/teapotset.gltf", this->mRegistry);

  auto imageTexture =
      std::make_shared<texture_2d>(texture_source_image("res/uv.png"));

  for (int i = 0; i < 10; i++) {
    auto cube = this->mRegistry.create();
    auto &trans = this->mRegistry.emplace<transform>(cube);
    trans.position(glm::vec3(cos(i / 10.0f * 3.14f * 2) * 5.0f, 0.0f,
                             sin(i / 10.0f * 3.14f * 2) * 5.0f));

    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back(
        {std::make_shared<standard_material>(imageTexture, 0.5f, 0.0f),
         std::make_shared<geometry>(geometry::make_box())});

    this->mRegistry.emplace<mesh>(cube, std::move(meshes));
    this->mRegistry.emplace<collision>(cube);
    this->mRegistry.emplace<name>(cube, "cube");
  }
  {
    auto cube = this->mRegistry.create();
    auto &trans = this->mRegistry.emplace<transform>(cube);
    trans.position(glm::vec3(0.0f, 0.0f, 0.0f));

    mesh model = load_file_to_mesh("res/bunny.gltf");
    this->mRegistry.emplace<mesh>(cube, model);
    this->mRegistry.emplace<collision>(cube);
    this->mRegistry.emplace<name>(cube, "bunny");
  }
  {
    auto cube = this->mRegistry.create();

    auto &trans = this->mRegistry.emplace<transform>(cube);
    trans.position(glm::vec3(0.0f, 0.0f, 3.0f));

    auto image = std::make_shared<texture_cube>(texture_cube_source{
        .right = texture_source_image("res/skybox/1.png"),
        .left = texture_source_image("res/skybox/2.png"),
        .up = texture_source_image("res/skybox/3.png"),
        .down = texture_source_image("res/skybox/4.png"),
        .front = texture_source_image("res/skybox/5.png"),
        .back = texture_source_image("res/skybox/6.png"),
    });

    auto material = std::make_shared<shader_material>(
        read_file_str("res/normal.vert"), read_file_str("res/normal.frag"));
    auto &uniforms = material->uniforms();
    uniforms["uTexture"] = reinterpret_cast<std::shared_ptr<texture> &>(image);

    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back(
        {material, std::make_shared<geometry>(geometry::make_box())});

    this->mRegistry.emplace<mesh>(cube, std::move(meshes));
    this->mRegistry.emplace<collision>(cube);
    this->mRegistry.emplace<name>(cube, "skyboxCube");
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
      this->mRegistry.emplace<physics>(player);
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
  {
    auto light = this->mRegistry.create();
    auto &transformVal = this->mRegistry.emplace<transform>(light);
    transformVal.position(glm::vec3(0.0, 10.0, 2.0));
    auto &lightVal = this->mRegistry.emplace<platformer::light>(light);
    lightVal.color = glm::vec3(1.0, 1.0, 1.0);
    lightVal.power = 1.0f;
    lightVal.radius = 0.1f;
    lightVal.range = 100.0f;
    this->mRegistry.emplace<name>(light, "light");
  }
}

void game::update(application &pApplication, float pDelta) {
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

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
    this->mWindowWidth = pEvent.window.data1;
    this->mWindowHeight = pEvent.window.data2;
    glViewport(0, 0, this->mWindowWidth, this->mWindowHeight);
  }
  this->mMovement.handle_event(*this, pEvent);
}

entt::registry &game::registry() { return this->mRegistry; }
