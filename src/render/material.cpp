#include "render/material.hpp"
#include "file.hpp"
#include "render/camera.hpp"
#include "render/geometry.hpp"
#include "render/pipeline.hpp"
#include "render/renderer.hpp"
#include "render/shader.hpp"
#include "render/texture.hpp"
#include "transform.hpp"
#include <any>
#include <format>
#include <memory>
#include <stdexcept>
#include <string>

using namespace platformer;

material::material() {}

material::~material() {}

shader_material::shader_material(std::string pVertex, std::string pFragment)
    : mShader(pVertex, pFragment), mUniforms() {}

void shader_material::render(subpipeline &pSubpipeline, geometry &pGeometry,
                             entt::entity pEntity) {
  // FIXME: This is not good enough; there needs to be a way to completely skip
  // the lighting pass without referencing the renderer directly, which can
  // cause undefined behaviors
  auto &renderer = pSubpipeline.renderer();
  auto &registry = renderer.registry();
  auto &transformVal = registry.get<transform>(pEntity);
  this->mShader.prepare();
  pGeometry.prepare(this->mShader);
  // Set up uniforms
  this->mShader.set("uModel", transformVal.matrix_world(registry));
  camera_handle camHandle(renderer);
  this->mShader.set("uView", camHandle.view());
  this->mShader.set("uProjection", camHandle.projection());
  this->mShader.set("uInverseView", camHandle.inverse_view());
  this->mShader.set("uInverseProjection", camHandle.inverse_projection());
  int textureAcc = 0;
  for (auto &[key, value] : this->mUniforms) {
    auto &valueType = value.type();
    if (valueType == typeid(float)) {
      this->mShader.set(key, std::any_cast<float>(value));
    } else if (valueType == typeid(glm::vec2)) {
      this->mShader.set(key, std::any_cast<glm::vec2>(value));
    } else if (valueType == typeid(glm::vec3)) {
      this->mShader.set(key, std::any_cast<glm::vec3>(value));
    } else if (valueType == typeid(glm::vec4)) {
      this->mShader.set(key, std::any_cast<glm::vec4>(value));
    } else if (valueType == typeid(glm::mat2)) {
      this->mShader.set(key, std::any_cast<glm::mat2>(value));
    } else if (valueType == typeid(glm::mat3)) {
      this->mShader.set(key, std::any_cast<glm::mat3>(value));
    } else if (valueType == typeid(glm::mat4)) {
      this->mShader.set(key, std::any_cast<glm::mat4>(value));
    } else if (valueType == typeid(std::shared_ptr<texture>)) {
      auto textureVal = std::any_cast<std::shared_ptr<texture>>(value);
      if (textureVal != nullptr) {
        int textureId = textureAcc++;
        textureVal->prepare(textureId);
        this->mShader.set(key, textureId);
      }
    } else {
      throw std::runtime_error(std::format("Invalid type of uniform %s", key));
    }
  }
  // Issue draw call
  renderer.apply_render_state({.depthFunc = GL_LEQUAL});
  pGeometry.render();
}

void shader_material::dispose() { this->mShader.dispose(); }

std::map<std::string, std::any> &shader_material::uniforms() {
  return this->mUniforms;
}

standard_material::standard_material()
    : roughness(0.5), metalic(0.0), color(glm::vec3(1.0)) {}

standard_material::standard_material(glm::vec3 pColor, float pRoughness,
                                     float pMetalic)
    : roughness(pRoughness), metalic(pMetalic), color(pColor) {}

standard_material::standard_material(std::shared_ptr<texture> pDiffuseTexture,
                                     float pRoughness, float pMetalic)
    : roughness(pRoughness), metalic(pMetalic), color(glm::vec3(1.0)),
      diffuseTexture(pDiffuseTexture) {}

void standard_material::render(subpipeline &pSubpipeline, geometry &pGeometry,
                               entt::entity pEntity) {
  auto &renderer = pSubpipeline.renderer();
  auto &registry = renderer.registry();
  auto &transformVal = registry.get<transform>(pEntity);
  int featureFlags = 0;
  if (this->diffuseTexture != nullptr) {
    featureFlags |= 1;
  }
  auto shaderVal = pSubpipeline.get_shader(
      "standard_material~" + std::to_string(featureFlags), [&]() {
        std::string defines = "";
        if (featureFlags & 1) {
          defines += "#define USE_DIFFUSE_TEXTURE\n";
        }

        shader_block result{
            .vertex_dependencies = {},
            .vertex_body = read_file_str("res/shader/standard.vert"),
            .fragment_dependencies = {},
            .fragment_header = defines + "in vec3 vPosition;\n"
                                         "in vec3 vNormal;\n"
                                         "in vec2 vTexCoord;\n"
                                         "uniform float uRoughness;\n"
                                         "uniform float uMetalic;\n"
                                         "uniform vec3 uColor;\n"
                                         "#ifdef USE_DIFFUSE_TEXTURE\n"
                                         "uniform sampler2D uDiffuse;\n"
                                         "#endif",
            .fragment_body = "#ifdef USE_DIFFUSE_TEXTURE\n"
                             "mInfo.albedo = pow(texture(uDiffuse, "
                             "vTexCoord).rgb, vec3(2.2));\n"
                             "#else\n"
                             "mInfo.albedo = uColor;\n"
                             "#endif\n"
                             "mInfo.normal = normalize(vNormal);\n"
                             "mInfo.position = vPosition;\n"
                             "mInfo.roughness = uRoughness;\n"
                             "mInfo.metalic = uMetalic;\n",
        };
        return result;
      });
  // TODO: It could be great if this is only done once per shader
  pSubpipeline.prepare_shader(shaderVal);
  pGeometry.prepare(*shaderVal);
  shaderVal->set("uModel", transformVal.matrix_world(registry));
  shaderVal->set("uColor", this->color);
  shaderVal->set("uRoughness", this->roughness);
  shaderVal->set("uMetalic", this->metalic);
  if (this->diffuseTexture != nullptr) {
    this->diffuseTexture->prepare(0);
    shaderVal->set("uDiffuse", 0);
  }
  pGeometry.render();
}

void standard_material::dispose() {}
