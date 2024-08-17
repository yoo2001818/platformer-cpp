#include "scene_ibl.hpp"
#include "application.hpp"
#include "file.hpp"
#include "game.hpp"
#include "pbr/pbr.hpp"
#include "render/cubemap.hpp"
#include "render/geometry.hpp"
#include "render/light.hpp"
#include "render/load.hpp"
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
      {.format = GL_RGB, .internalFormat = GL_RGB, .type = GL_HALF_FLOAT}};
  cubemap_pbr cubemapPbr{cubemapVal.get_texture(),
                         {.magFilter = GL_LINEAR,
                          .minFilter = GL_LINEAR_MIPMAP_LINEAR,
                          .wrapS = GL_CLAMP_TO_EDGE,
                          .wrapT = GL_CLAMP_TO_EDGE,
                          .width = 1024,
                          .height = 1024,
                          .mipmap = true},
                         {
                             .format = GL_RGB,
                             .internalFormat = GL_RGB,
                             .type = GL_HALF_FLOAT,
                         }};
  auto brdfMap = std::make_shared<texture_brdf>(texture_brdf(
      {.minFilter = GL_LINEAR, .width = 256, .height = 256, .mipmap = false}));

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
    registry.emplace<light_component>(
        light, std::make_shared<point_light>(point_light_options({
                   .color = glm::vec3(1.0),
                   .power = 100.0f,
                   .radius = 0.1f,
                   .range = 100.0f,
               })));
    registry.emplace<name>(light, "light");
  }
  {
    auto light = registry.create();
    auto &transformVal = registry.emplace<transform>(light);
    transformVal.position(glm::vec3(0.0, 10.0, 2.0));
    registry.emplace<light_component>(
        light, std::make_shared<envmap_light>(envmap_light_options({
                   .envMap = cubemapPbr.get_texture(),
                   .brdfMap = brdfMap->get_texture(),
               })));
    registry.emplace<name>(light, "envmap");
  }
  {
    auto cube = registry.create();

    auto &trans = registry.emplace<transform>(cube);
    trans.position(glm::vec3(0.0f, 1.0f, 3.0f));

    auto material = std::make_shared<shader_material>(
        read_file_str("res/skybox.vert"), read_file_str("res/skybox.frag"));
    auto &uniforms = material->uniforms();
    uniforms["uTexture"] =
        reinterpret_cast<std::shared_ptr<texture> &>(cubemapPbr.get_texture());

    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back(
        {material, std::make_shared<geometry>(geometry::make_quad())});

    registry.emplace<mesh>(cube, std::move(meshes));
    registry.emplace<collision>(cube);
    registry.emplace<name>(cube, "skybox");
  }
  {
    mesh model = load_file_to_mesh("res/bunny.gltf");

    for (int x = 0; x < 5; x += 1) {
      for (int y = 0; y < 5; y += 1) {
        auto cube = registry.create();
        auto &trans = registry.emplace<transform>(cube);
        trans.position(glm::vec3(x * 3.0f, y * 3.0f, -6.0f));

        auto mat = std::make_shared<standard_material>(glm::vec3(1.0f),
                                                       x / 5.0f, y / 5.0f);

        registry.emplace<mesh>(cube, mesh({{mat, model.meshes()[0].second}}));
        registry.emplace<collision>(cube);
        registry.emplace<name>(cube, "bunny");
      }
    }
  }
}
void scene_ibl::update(application &pApplication, game &pGame, float pDelta) {}
void scene_ibl::dispose() {}

const std::string scene_ibl::get_name() { return "ibl"; }
