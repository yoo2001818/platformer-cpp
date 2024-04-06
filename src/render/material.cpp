#include "render/material.hpp"
#include "file.hpp"
#include "render/geometry.hpp"
#include "render/render.hpp"
#include "render/shader.hpp"

using namespace platformer;

material::material() {}

material::~material() {}

shader_material::shader_material(std::string pVertex, std::string pFragment)
    : mShader(pVertex, pFragment) {}

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
  // Issue draw call
  pContext.geometry.render();
}

void shader_material::dispose() { this->mShader.dispose(); }

standard_material::standard_material()
    : mShader(standard_material::create_shader()), roughness(0.5), metalic(0.0),
      color(glm::vec3(1.0)) {}

standard_material::standard_material(glm::vec3 pColor, float pRoughness,
                                     float pMetalic)
    : mShader(standard_material::create_shader()), roughness(pRoughness),
      metalic(pMetalic), color(pColor) {}

void standard_material::render(const render_context &pContext) {
  this->mShader.prepare();
  pContext.geometry.prepare(this->mShader);
  // Set up uniforms
  this->mShader.set("uModel",
                    pContext.transform.matrix_world(pContext.registry));
  this->mShader.set("uView", pContext.camera_transform.matrix_world_inverse(
                                 pContext.registry));
  this->mShader.set("uProjection",
                    pContext.camera_camera.getProjection(pContext.aspect));
  this->mShader.set("uColor", this->color);
  this->mShader.set("uRoughness", this->roughness);
  this->mShader.set("uMetalic", this->metalic);
  int pos = 0;
  for (auto &light : pContext.lights) {
    this->mShader.set("uLightPositions", pos, light.position);
    this->mShader.set("uLightColors", pos, light.color);
    this->mShader.set("uLightRanges", pos, light.range);
    pos += 1;
    if (pos >= 8)
      break;
  }
  this->mShader.set("uLightCount", pos);
  // Issue draw call
  pContext.geometry.render();
}

void standard_material::dispose() { this->mShader.dispose(); }

shader standard_material::create_shader() {
  auto vert = read_file_str("res/phong.vert");
  auto frag = read_file_str("res/phong.frag");
  return shader(vert, frag);
}
