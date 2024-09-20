#include "render/material.hpp"
#include "file.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include "render/armature.hpp"
#include "render/buffer.hpp"
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
#include <vector>

using namespace platformer;

material::material() {}

material::~material() {}

shader_material::shader_material(std::string pVertex, std::string pFragment)
    : mShader(pVertex, pFragment), mUniforms() {}

void shader_material::render(subpipeline &pSubpipeline, geometry &pGeometry,
                             std::vector<entt::entity> &pEntities) {
  // FIXME: This is not good enough; there needs to be a way to completely skip
  // the lighting pass without referencing the renderer directly, which can
  // cause undefined behaviors
  auto &renderer = pSubpipeline.renderer();
  auto &registry = renderer.registry();
  this->mShader.prepare();
  pGeometry.prepare(this->mShader);
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
  for (auto entity : pEntities) {
    // Set up uniforms
    auto &transformVal = registry.get<transform>(entity);
    this->mShader.set("uModel", transformVal.matrix_world(registry));
    pGeometry.render();
  }
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
                               std::vector<entt::entity> &pEntities) {
  auto &renderer = pSubpipeline.renderer();
  auto &registry = renderer.registry();
  bool useInstancing = true;
  bool useArmature = false;
  // FIXME: It should be possible to render armatures without armature component
  if (!pGeometry.boneIds().empty()) {
    useInstancing = false;
    useArmature = true;
  }
  int featureFlags = 0;
  if (useInstancing) {
    featureFlags |= 1;
  }
  if (useArmature) {
    featureFlags |= 2;
  }
  if (this->diffuseTexture != nullptr) {
    featureFlags |= 4;
  }
  if (!pGeometry.colors().empty()) {
    featureFlags |= 8;
  }
  if (this->normalTexture != nullptr) {
    featureFlags |= 16;
  }
  auto shaderVal = pSubpipeline.get_shader(
      "standard_material~" + std::to_string(featureFlags), [&]() {
        std::string defines = "";
        if (featureFlags & 1) {
          defines += "#define USE_INSTANCING\n";
        }
        if (featureFlags & 2) {
          defines += "#define USE_ARMATURE\n";
        }
        if (featureFlags & 4) {
          defines += "#define USE_DIFFUSE_TEXTURE\n";
        }
        if (featureFlags & 8) {
          defines += "#define USE_VERTEX_COLOR\n";
        }
        if (featureFlags & 16) {
          defines += "#define USE_NORMAL_TEXTURE\n";
        }

        shader_block result{
            .vertex_dependencies = {},
            .vertex_body = defines + read_file_str("res/shader/standard.vert"),
            .fragment_dependencies = {"res/shader/pbr.glsl"},
            .fragment_header = defines + "in vec3 vPosition;\n"
                                         "in vec3 vNormal;\n"
                                         "in vec2 vTexCoord;\n"
                                         "#ifdef USE_NORMAL_TEXTURE\n"
                                         "in vec4 vTangent;\n"
                                         "#endif\n"
                                         "#ifdef USE_VERTEX_COLOR\n"
                                         "in vec4 vColor;\n"
                                         "#endif\n"
                                         "uniform float uRoughness;\n"
                                         "uniform float uMetalic;\n"
                                         "uniform vec3 uColor;\n"
                                         "#ifdef USE_DIFFUSE_TEXTURE\n"
                                         "uniform sampler2D uDiffuseMap;\n"
                                         "#endif\n"
                                         "#ifdef USE_NORMAL_TEXTURE\n"
                                         "uniform sampler2D uNormalMap;\n"
                                         "#endif\n",
            .fragment_body =
                "#ifdef USE_DIFFUSE_TEXTURE\n"
                "mInfo.albedo = pow(texture(uDiffuseMap, "
                "vTexCoord).rgb, vec3(2.2));\n"
                "#else\n"
                "mInfo.albedo = uColor;\n"
                "#endif\n"
                "#ifdef USE_VERTEX_COLOR\n"
                "mInfo.albedo = mInfo.albedo * vColor.rgb;\n"
                "#endif\n"
                "#ifdef USE_NORMAL_TEXTURE\n"
                "mInfo.normal = calcNormalMap(vNormal, vTangent, "
                "texture2D(uNormalMap, vTexCoord).xyz * 2.0 - 1.0);\n"
                "#else\n"
                "mInfo.normal = normalize(vNormal);\n"
                "#endif\n"
                "mInfo.position = vPosition;\n"
                "mInfo.roughness = uRoughness;\n"
                "mInfo.metalic = uMetalic;\n",
        };
        return result;
      });
  pSubpipeline.prepare_shader(shaderVal);
  pGeometry.prepare(*shaderVal);
  shaderVal->set("uColor", this->color);
  shaderVal->set("uRoughness", this->roughness);
  shaderVal->set("uMetalic", this->metalic);
  if (this->diffuseTexture != nullptr) {
    this->diffuseTexture->prepare(0);
    shaderVal->set("uDiffuseMap", 0);
  }
  if (this->normalTexture != nullptr) {
    this->normalTexture->prepare(1);
    shaderVal->set("uNormalMap", 1);
  }
  if (useInstancing) {
    gl_array_buffer buffer{GL_STREAM_DRAW};
    std::vector<glm::mat4> models;
    models.reserve(pEntities.size());
    for (auto entity : pEntities) {
      auto &transformVal = registry.get<transform>(entity);
      models.push_back(transformVal.matrix_world(registry));
    }
    buffer.set(models);
    buffer.bind();
    shaderVal->set_attribute("aModel", 0, 4, GL_FLOAT, GL_FLOAT,
                             sizeof(glm::mat4), 0, 1);
    shaderVal->set_attribute("aModel", 1, 4, GL_FLOAT, GL_FLOAT,
                             sizeof(glm::mat4), sizeof(glm::vec4), 1);
    shaderVal->set_attribute("aModel", 2, 4, GL_FLOAT, GL_FLOAT,
                             sizeof(glm::mat4), sizeof(glm::vec4) * 2, 1);
    shaderVal->set_attribute("aModel", 3, 4, GL_FLOAT, GL_FLOAT,
                             sizeof(glm::mat4), sizeof(glm::vec4) * 3, 1);
    pGeometry.render(pEntities.size());
    buffer.dispose();
  } else {
    for (auto entity : pEntities) {
      auto &transformVal = registry.get<transform>(entity);
      shaderVal->set("uModel", transformVal.matrix_world(registry));
      if (useArmature) {
        auto armatureVal = registry.try_get<armature_component>(entity);
        if (armatureVal == nullptr) {
          continue;
        }
        auto matrices = armatureVal->bone_matrices(registry);
        auto boneMatricesBuf =
            renderer.asset_manager().get<std::shared_ptr<gl_texture_buffer>>(
                "bone_matrices_buffer", []() {
                  return std::make_shared<gl_texture_buffer>(GL_STREAM_DRAW);
                });
        auto boneMatricesTex =
            renderer.asset_manager().get<std::shared_ptr<texture_buffer>>(
                "bone_matrices_texture", [&]() {
                  return std::make_shared<texture_buffer>(boneMatricesBuf,
                                                          GL_RGBA32F);
                });
        boneMatricesBuf->set(matrices);
        boneMatricesTex->prepare(5);
        shaderVal->set("uBoneMatrices", 5);
      }
      pGeometry.render();
    }
  }
}

void standard_material::dispose() {}
