#include "scene_instancing.hpp"
#include "application.hpp"
#include "game.hpp"
#include "geometry/geometry.hpp"
#include "material/material.hpp"
#include "render/cubemap.hpp"
#include "render/pbr.hpp"
#include "render/shader.hpp"
#include "render/texture.hpp"
#include "scenegraph/light.hpp"
#include "scenegraph/mesh.hpp"
#include "util/file.hpp"
#include <cmath>
#include <memory>

using namespace platformer;

void scene_instancing::init(application &pApplication, game &pGame) {
  auto &registry = pGame.registry();
  auto imageTexture = std::make_shared<texture_2d>(
      texture_source_image({.filename = "res/uv.png"}));
  cubemap_equirectangular cubemapVal{
      std::make_shared<texture_2d>(
          texture_source_image({.format =
                                    {
                                        .internalFormat = GL_RGB32F,
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
      {.format = GL_RGB, .internalFormat = GL_RGB16F, .type = GL_HALF_FLOAT}};
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
                             .internalFormat = GL_RGB16F,
                             .type = GL_HALF_FLOAT,
                         }};
  auto brdfMap = std::make_shared<texture_brdf>(texture_brdf(
      {.minFilter = GL_LINEAR, .width = 256, .height = 256, .mipmap = false}));

  auto box = std::make_shared<geometry>(geometry::make_box());
  auto imgMaterial =
      std::make_shared<standard_material>(imageTexture, 0.5f, 0.0f);
  auto boxMesh =
      std::make_shared<mesh>(std::vector<mesh::mesh_pair>{{imgMaterial, box}});

  for (int i = 0; i < 100; i++) {
    auto cube = registry.create();
    auto &trans = registry.emplace<transform>(cube);
    trans.position(
        glm::vec3(((i % 10) - 5) * 0.5f, 0.0f, std::floor(i / 10 - 5) * 0.5f));
    trans.scale(glm::vec3(0.2));

    registry.emplace<mesh_component>(cube, boxMesh);
    registry.emplace<collision>(cube);
    registry.emplace<name>(cube, "cube");
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

    registry.emplace<mesh_component>(cube,
                                     std::make_shared<mesh>(std::move(meshes)));
    registry.emplace<collision>(cube);
    registry.emplace<name>(cube, "skybox");
  }
}
void scene_instancing::update(application &pApplication, game &pGame,
                              float pDelta) {}
void scene_instancing::dispose() {}

const std::string scene_instancing::get_name() { return "instancing"; }
