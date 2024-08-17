#include "render/pipeline.hpp"
#include "game.hpp"
#include "render/camera.hpp"
#include "render/mesh.hpp"
#include "render/renderer.hpp"
#include "render/shader.hpp"
#include "render/shader_preprocessor.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

using namespace platformer;

pipeline::pipeline(renderer &pRenderer) : mRenderer(pRenderer) {}

void pipeline::render() {
  auto &registry = this->mRenderer.game().registry();
  auto cameraEntity = this->mRenderer.camera();
  auto &cameraTransform = registry.get<transform>(cameraEntity);
  auto &cameraCamera = registry.get<platformer::camera>(cameraEntity);
  this->collect_lights();
  auto view = registry.view<transform, mesh>();
  for (auto entity : view) {
    auto &meshVal = registry.get<mesh>(entity);
    meshVal.render(this->mRenderer, entity);
  }
}

const std::vector<render_light> &pipeline::get_lights() const {
  return this->mLights;
}

std::shared_ptr<shader>
pipeline::get_shader(const std::string &pShaderId,
                     const std::function<shader_block()> &pExec) {
  // This assumes the following structure for the fragment body:
  // void material(out MaterialInfo mInfo) {
  //  ...
  // }
  // It is free to read any information from uniforms, varying, etc, otherwise.
  auto &assetManager = this->mRenderer.asset_manager();
  return assetManager.get<std::shared_ptr<shader>>(
      "shader~" + pShaderId, [&]() {
        // FIXME: Any light routines are omitted for now, but needs to be
        // implemented
        auto shaderBlock = pExec();
        std::stringstream vertex;
        vertex << "#version 330 core\n";
        for (auto &file : shaderBlock.vertex_dependencies) {
          vertex << "#include " << file << "\n";
        }
        vertex << shaderBlock.vertex_body;

        std::stringstream fragment;
        fragment << "#version 330 core\n";
        fragment << "#include \"res/shader/pbr.glsl\"\n";
        for (auto &file : shaderBlock.fragment_dependencies) {
          fragment << "#include " << file << "\n";
        }
        fragment << shaderBlock.fragment_header << "\n";
        fragment << "out vec4 FragColor;\n"
                    "void main() {\n"
                    "  MaterialInfo mInfo;\n"
                    "  {\n";
        fragment << shaderBlock.fragment_body << "\n";
        fragment << "  }\n"
                    "  vec3 result = vec3(0.0);\n"
                    "  result += 0.5 * mInfo.albedo;\n"
                    "  vec3 tonemappedColor = pow(result, vec3(1.0 / 2.2));\n"
                    "  FragColor = vec4(tonemappedColor, 1.0);\n"
                    "}\n";

        shader_preprocessor vertexProc(vertex.str());
        shader_preprocessor fragmentProc(fragment.str());

        return std::make_shared<shader>(vertexProc.get(), fragmentProc.get());
      });
}

void pipeline::prepare_shader(std::shared_ptr<shader> &pShader) {
  pShader->prepare();
  // Well, it should set the renderer state, framebuffer, etc, but we don't
  // have any of that in forward rendering
  camera_handle camHandle(this->mRenderer);
  pShader->set("uView", camHandle.view());
  pShader->set("uViewPos", camHandle.view_pos());
  pShader->set("uProjection", camHandle.projection());
}

void pipeline::collect_lights() {
  auto &registry = this->mRenderer.game().registry();
  this->mLights.clear();
  auto view = registry.view<transform, light>();
  for (auto entity : view) {
    auto &lightVal = registry.get<light>(entity);
    auto &transformVal = registry.get<transform>(entity);
    render_light renderLight{
        .position = glm::vec4(transformVal.position_world(registry), 1.0f),
        .color = lightVal.color,
        .range = glm::vec3(lightVal.power / std::numbers::pi, lightVal.radius,
                           lightVal.range)};
    this->mLights.push_back(renderLight);
  }
}
