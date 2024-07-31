#include "render/material.hpp"
#include "render/geometry.hpp"
#include "render/render.hpp"
#include "render/shader.hpp"
#include "render/texture.hpp"
#include <any>
#include <format>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using namespace platformer;

material::material() {}

material::~material() {}

shader_material::shader_material(std::string pVertex, std::string pFragment)
    : mShader(pVertex, pFragment), mUniforms() {}

void shader_material::render(const render_context &pContext) {
  this->mShader.prepare();
  pContext.geometry.prepare(this->mShader);
  // Set up uniforms
  this->mShader.set("uModel",
                    pContext.transform.matrix_world(pContext.registry));
  this->mShader.set("uView", pContext.camera_transform.matrix_world_inverse(
                                 pContext.registry));
  this->mShader.set("uProjection",
                    pContext.camera_camera.getProjection(pContext.aspect));
  this->mShader.set("uInverseView",
                    pContext.camera_transform.matrix_world(pContext.registry));
  this->mShader.set(
      "uInverseProjection",
      glm::inverse(pContext.camera_camera.getProjection(pContext.aspect)));
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
  pContext.geometry.render();
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

void standard_material::render(const render_context &pContext) {
  int featureFlags = 0;
  if (this->diffuseTexture != nullptr) {
    featureFlags |= 1;
  }
  if (this->environmentTexture != nullptr) {
    featureFlags |= 2;
  }
  auto shaderVal = pContext.asset_manager.get<std::shared_ptr<shader>>(
      "standard_material:" + std::to_string(featureFlags), [&]() {
        std::vector<std::string> defines;
        if (featureFlags & 1) {
          defines.push_back("USE_DIFFUSE_TEXTURE");
        }
        if (featureFlags & 2) {
          defines.push_back("USE_ENVIRONMENT_MAP");
        }

        return std::make_shared<file_shader>("res/phong.vert", "res/phong.frag",
                                             defines);
      });
  shaderVal->prepare();
  pContext.geometry.prepare(*shaderVal);
  // Set up uniforms
  shaderVal->set("uModel", pContext.transform.matrix_world(pContext.registry));
  shaderVal->set("uView", pContext.camera_transform.matrix_world_inverse(
                              pContext.registry));
  shaderVal->set("uViewPos",
                 pContext.camera_transform.position_world(pContext.registry));
  shaderVal->set("uProjection",
                 pContext.camera_camera.getProjection(pContext.aspect));
  shaderVal->set("uColor", this->color);
  shaderVal->set("uRoughness", this->roughness);
  shaderVal->set("uMetalic", this->metalic);
  int pos = 0;
  for (auto &light : pContext.lights) {
    shaderVal->set("uLightPositions", pos, light.position);
    shaderVal->set("uLightColors", pos, light.color);
    shaderVal->set("uLightRanges", pos, light.range);
    pos += 1;
    if (pos >= 8)
      break;
  }
  shaderVal->set("uLightCount", pos);

  if (this->diffuseTexture != nullptr) {
    this->diffuseTexture->prepare(0);
    shaderVal->set("uDiffuse", 0);
  }
  if (this->environmentTexture != nullptr) {
    this->environmentTexture->prepare(1);
    shaderVal->set("uEnvironmentMap", 1);
    // FIXME: Yup, the resolution is hardcoded
    shaderVal->set("uEnvironmentMapSize", glm::vec2(7.0f, 1.0f));
    this->brdfTexture->prepare(2);
    shaderVal->set("uBRDFMap", 2);
  }
  // Issue draw call
  pContext.geometry.render();
}

void standard_material::dispose() {}
