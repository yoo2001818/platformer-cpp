#include "scene_bunchoftest.hpp"
#include "application.hpp"
#include "file.hpp"
#include "game.hpp"
#include "render/framebuffer.hpp"
#include "render/load.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
#include "render/texture.hpp"

using namespace platformer;

void scene_bunchoftest::init(application &pApplication, game &pGame) {
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
    auto cube = registry.create();
    auto &trans = registry.emplace<transform>(cube);
    trans.position(glm::vec3(0.0f, 0.0f, 0.0f));

    mesh model = load_file_to_mesh("res/bunny.gltf");
    registry.emplace<mesh>(cube, model);
    registry.emplace<collision>(cube);
    registry.emplace<name>(cube, "bunny");
  }
  /*
  {
    auto cube = registry.create();
    auto &trans = registry.emplace<transform>(cube);
    trans.position(glm::vec3(0.5f, 3.0f, 0.5f));

    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back(
        {std::make_shared<standard_material>(glm::vec3(1.0f), 0.5f, 0.0f),
         std::make_shared<geometry>(geometry::make_box())});

    registry.emplace<mesh>(cube, std::move(meshes));
    registry.emplace<collision>(cube);
    registry.emplace<physics>(cube);
  }
  */
  // Quad test
  {
    auto ent = registry.create();
    auto &transformVal = registry.emplace<transform>(ent);
    transformVal.position(glm::vec3(0.0, 3.0, -3.0));

    auto image = std::make_shared<texture_2d>(
        texture_source_buffer{.width = 256, .height = 256},
        texture_options{
            .magFilter = GL_LINEAR, .minFilter = GL_LINEAR, .mipmap = false});
    auto shader = std::make_shared<platformer::shader>(
        read_file_str("res/quad.vert"), read_file_str("res/quad.frag"));
    auto quad = geometry::make_quad();

    // Try to bake image
    framebuffer fb({.colors = {{.texture = image}}});
    fb.bind();
    glClearColor(.5f, .5f, .5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    pApplication.gl_renderer().apply_render_state({
        .cullEnabled = false,
        .depthEnabled = false,
    });
    shader->prepare();
    quad.prepare(*shader);
    quad.render();
    fb.unbind();

    auto material = std::make_shared<shader_material>(
        read_file_str("res/texturePass.vert"),
        read_file_str("res/texturePass.frag"));
    auto &uniforms = material->uniforms();
    uniforms["uDiffuse"] = reinterpret_cast<std::shared_ptr<texture> &>(image);

    std::vector<mesh::mesh_pair> meshes{};
    meshes.push_back(
        {material, std::make_shared<geometry>(geometry::make_quad())});

    registry.emplace<mesh>(ent, std::move(meshes));
    registry.emplace<name>(ent, "quad");
  }
  {
    auto light = registry.create();
    auto &transformVal = registry.emplace<transform>(light);
    transformVal.position(glm::vec3(0.0, 10.0, 2.0));
    auto &lightVal = registry.emplace<platformer::light>(light);
    lightVal.color = glm::vec3(1.0, 1.0, 1.0);
    lightVal.power = 100.0f;
    lightVal.radius = 0.1f;
    lightVal.range = 100.0f;
    registry.emplace<name>(light, "light");
  }
}
void scene_bunchoftest::update(application &pApplication, game &pGame,
                               float pDelta) {}
void scene_bunchoftest::dispose() {}

const std::string scene_bunchoftest::get_name() { return "bunchoftest"; }
