#include "render/pipeline.hpp"
#include "game.hpp"
#include "render/camera.hpp"
#include "render/light.hpp"
#include "render/mesh.hpp"
#include "render/renderer.hpp"
#include "render/shader.hpp"
#include "render/shader_preprocessor.hpp"
#include <functional>
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
  this->prepare_lights();
  auto view = registry.view<transform, mesh>();
  for (auto entity : view) {
    auto &meshVal = registry.get<mesh>(entity);
    meshVal.render(this->mRenderer, entity);
  }
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
      "shader~" + pShaderId + this->mLightShaderIds, [&]() {
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
          fragment << "#include \"" << file << "\"\n";
        }
        for (auto &entry : this->mLightShaderBlocks) {
          for (auto &file : entry.fragment_dependencies) {
            fragment << "#include \"" << file << "\"\n";
          }
        }

        fragment << shaderBlock.fragment_header << "\n";
        for (auto &entry : this->mLightShaderBlocks) {
          fragment << entry.fragment_header << "\n";
        }

        fragment << "uniform vec3 uViewPos;\n"
                    "out vec4 FragColor;\n"
                    "void main() {\n"
                    "  MaterialInfo mInfo;\n"
                    "  {\n";
        fragment << shaderBlock.fragment_body << "\n";
        fragment << "  }\n"
                    "  vec3 result = vec3(0.0);\n";
        for (auto &entry : this->mLightShaderBlocks) {
          fragment << entry.fragment_body << "\n";
        }
        fragment << "  vec3 tonemappedColor = pow(result, vec3(1.0 / 2.2));\n"
                    "  FragColor = vec4(tonemappedColor, 1.0);\n"
                    "}\n";

        shader_preprocessor vertexProc(vertex.str());
        shader_preprocessor fragmentProc(fragment.str());

        return std::make_shared<shader>(vertexProc.get(), fragmentProc.get());
      });
}

void pipeline::prepare_shader(std::shared_ptr<shader> &pShader) {
  auto &registry = this->mRenderer.game().registry();
  pShader->prepare();
  // Well, it should set the renderer state, framebuffer, etc, but we don't
  // have any of that in forward rendering
  camera_handle camHandle(this->mRenderer);
  pShader->set("uView", camHandle.view());
  pShader->set("uViewPos", camHandle.view_pos());
  pShader->set("uProjection", camHandle.projection());
  // Set uniforms for lights
  for (auto &entry : this->mLights) {
    auto &entities = entry.second;
    auto &light = registry.get<light_component>(entities[0]).light;
    light->set_uniforms(*pShader, entities, this->mRenderer);
  }
}

void pipeline::prepare_lights() {
  auto &registry = this->mRenderer.game().registry();
  auto view = registry.view<transform, light_component>();
  this->mLights.clear();
  // Group all lights in the separate vector
  for (auto entity : view) {
    auto &light = registry.get<light_component>(entity).light;
    auto lightType = std::string(light->type());
    auto current = this->mLights.find(lightType);
    if (current != this->mLights.end()) {
      current->second.push_back(entity);
    } else {
      this->mLights.insert({lightType, {entity}});
    }
  }
  this->mLightShaderBlocks.clear();
  this->mLightShaderIds = "";
  // Prepare lights and generate light shader blocks
  for (auto &entry : this->mLights) {
    auto &entities = entry.second;
    auto &light = registry.get<light_component>(entities[0]).light;
    light->prepare(entities, this->mRenderer);
    auto shaderBlock =
        light->get_shader_block(entities.size(), this->mRenderer);
    this->mLightShaderBlocks.push_back(shaderBlock);
    this->mLightShaderIds +=
        "~" + std::string(entry.first) + "-" + shaderBlock.id;
  }
}
