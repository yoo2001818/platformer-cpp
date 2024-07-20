#include "scene_ibl.hpp"
#include "application.hpp"
#include "file.hpp"
#include "game.hpp"
#include "render/cubemap.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
#include "render/render_state.hpp"
#include "render/shader.hpp"
#include "render/texture.hpp"
#include <memory>

using namespace platformer;

void scene_ibl::init(application &pApplication, game &pGame) {
  auto &registry = pGame.registry();
  auto imageTexture = std::make_shared<texture_2d>(
      texture_source_image({.filename = "res/uv.png"}));

  for (int i = 0; i < 10; i++) {
    auto cube = registry.create();
    auto &trans = registry.emplace<transform>(cube);
    trans.position(glm::vec3(cos(i / 10.0f * 3.14f * 2) * 5.0f, 0.0f,
                             sin(i / 10.0f * 3.14f * 2) * 5.0f));

    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back(
        {std::make_shared<standard_material>(imageTexture, 0.5f, 0.0f),
         std::make_shared<geometry>(geometry::make_box())});

    registry.emplace<mesh>(cube, std::move(meshes));
    registry.emplace<collision>(cube);
    registry.emplace<name>(cube, "cube");
  }
  {
    auto light = registry.create();
    auto &transformVal = registry.emplace<transform>(light);
    transformVal.position(glm::vec3(0.0, 10.0, 2.0));
    auto &lightVal = registry.emplace<platformer::light>(light);
    lightVal.color = glm::vec3(1.0, 1.0, 1.0);
    lightVal.power = 1.0f;
    lightVal.radius = 0.1f;
    lightVal.range = 100.0f;
    registry.emplace<name>(light, "light");
  }
  {
    auto cube = registry.create();

    auto &trans = registry.emplace<transform>(cube);
    trans.position(glm::vec3(0.0f, 1.0f, 3.0f));

    cubemap_equirectangular cubemapVal{
        std::make_shared<texture_2d>(
            texture_source_image({.format =
                                      {
                                          .type = GL_FLOAT,
                                      },
                                  .filename = "res/skybox.hdr"})),
        {.magFilter = GL_LINEAR,
         .minFilter = GL_LINEAR_MIPMAP_LINEAR,
         .wrapS = GL_CLAMP_TO_EDGE,
         .wrapT = GL_CLAMP_TO_EDGE,
         .width = 1024,
         .height = 1024,
         .mipmap = true},
        {.type = GL_FLOAT}};

    auto material = std::make_shared<shader_material>(
        read_file_str("res/skybox.vert"), read_file_str("res/skybox.frag"));
    auto &uniforms = material->uniforms();
    uniforms["uTexture"] =
        reinterpret_cast<std::shared_ptr<texture> &>(cubemapVal.get_texture());

    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back(
        {material, std::make_shared<geometry>(geometry::make_quad())});

    registry.emplace<mesh>(cube, std::move(meshes));
    registry.emplace<collision>(cube);
    registry.emplace<name>(cube, "skybox");
  }
}
void scene_ibl::update(application &pApplication, game &pGame, float pDelta) {}
void scene_ibl::dispose() {}

const std::string scene_ibl::get_name() { return "ibl"; }
