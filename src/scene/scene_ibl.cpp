#include "scene_ibl.hpp"
#include "application.hpp"
#include "game.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
#include "render/texture.hpp"

using namespace platformer;

void scene_ibl::init(application &pApplication, game &pGame) {
  auto &registry = pGame.registry();
  auto imageTexture =
      std::make_shared<texture_2d>(texture_source_image("res/uv.png"));

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
}
void scene_ibl::update(application &pApplication, game &pGame,
                               float pDelta) {}
void scene_ibl::dispose() {}

const std::string scene_ibl::get_name() {
  return "ibl";
}