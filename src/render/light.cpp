#include "light.hpp"
#include "pipeline.hpp"
#include "transform.hpp"
#include <memory>
#include <string>

using namespace platformer;

point_light::point_light() {}

point_light::point_light(const point_light_options &pOptions)
    : mOptions(pOptions) {}

point_light::~point_light() {}

shader_block point_light::get_shader_block(int pNumLights,
                                           renderer &pRenderer) {
  return {.id = std::to_string(pNumLights),
          .vertex_dependencies = {},
          .vertex_body = "",
          .fragment_dependencies = {"res/shader/light.glsl"},
          .fragment_header =
              "#define POINT_LIGHTS_SIZE " + std::to_string(pNumLights) +
              "\n"
              "uniform vec3 uPointLightPositions[POINT_LIGHTS_SIZE];\n"
              "uniform vec3 uPointLightColors[POINT_LIGHTS_SIZE];\n"
              "uniform vec3 uPointLightRanges[POINT_LIGHTS_SIZE];\n"
              "uniform int uPointLightCount;\n",
          .fragment_body =
              "for (int i = 0; i < POINT_LIGHTS_SIZE; i += 1) {\n"
              "  if (i > uPointLightCount) break;\n"
              "  PointLight light;\n"
              "  light.position = uPointLightPositions[i].xyz;\n"
              "  light.color = uPointLightColors[i];\n"
              "  light.intensity = uLightRanges[i];\n"
              "  vec3 L;\n"
              "  vec3 V = normalize(uViewPos - mInfo.position);\n"
              "  vec3 N = mInfo.normal;\n"
              "  result += calcPointLight(L, V, N, mInfo.position, light) * "
              "calcBRDF(L, V, N, mInfo);\n"
              "}\n"};
}

void point_light::set_uniforms(shader &pShader,
                               std::vector<entt::entity> pEntities,
                               renderer &pRenderer) {
  auto &registry = pRenderer.registry();
  int pos = 0;
  for (auto &light : pEntities) {
    auto &transformVal = registry.get<transform>(light);
    auto &lightVal = registry.get<light_component>(light);
    auto pointLightVal = std::static_pointer_cast<point_light>(lightVal.light);
    auto &options = pointLightVal->options();
    pShader.set("uPointLightPositions", pos, transformVal.position());
    pShader.set("uPointLightColors", pos, options.color);
    pShader.set("uPointLightRanges", pos,
                glm::vec3(options.power / std::numbers::pi, options.radius,
                          options.range));
    pos += 1;
  }
}

const point_light_options &point_light::options() { return this->mOptions; }

void point_light::options(const point_light_options &pOptions) {
  this->mOptions = pOptions;
}

envmap_light::envmap_light() {}

envmap_light::envmap_light(const envmap_light_options &pOptions)
    : mOptions(pOptions) {}

envmap_light::~envmap_light() {}

shader_block envmap_light::get_shader_block(int pNumLights,
                                            renderer &pRenderer) {
  // Implementation of get_shader_block for envmap_light
}

void envmap_light::set_uniforms(shader &pShader,
                                std::vector<entt::entity> pEntities,
                                renderer &pRenderer) {
  // Implementation of set_uniforms for envmap_light
}
