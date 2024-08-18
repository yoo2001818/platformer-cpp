#include "light.hpp"
#include "entt/core/hashed_string.hpp"
#include "entt/entity/fwd.hpp"
#include "pipeline.hpp"
#include "render/geometry.hpp"
#include "transform.hpp"
#include <memory>
#include <string>

using namespace platformer;

void light::prepare(renderer &pRenderer,
                    const std::vector<entt::entity> &pEntities) {}
void light::render_deferred(subpipeline &pSubpipeline,
                            const std::vector<entt::entity> &pEntities) {
  auto &renderer = pSubpipeline.renderer();
  // TODO: This is literally used everywhere. Couldn't it use global variables?
  auto quad =
      renderer.asset_manager().get<std::shared_ptr<geometry>>("quad2", [&]() {
        return std::make_shared<geometry>(geometry::make_quad());
      });
  std::string id = "quad-" + std::to_string(this->type()) + "-" +
                   std::to_string(pEntities.size());
  auto shader = pSubpipeline.get_shader(id, [&]() {
    auto shaderBlock = this->get_shader_block(renderer, pEntities.size());
    return shader_block{
        .id = "",
        .vertex_dependencies = {},
        .vertex_body = "layout(location = 0) in vec3 aPosition;\n"
                       "layout(location = 1) in vec2 aTexCoord;\n"
                       "out vec2 vPosition;\n"
                       "void main() {\n"
                       "  gl_Position = vec4(aPosition.xy, 1.0, 1.0);\n"
                       "  vPosition = aPosition.xy;\n"
                       "}\n",
        .fragment_dependencies = shaderBlock.fragment_dependencies,
        .fragment_header = shaderBlock.fragment_header,
        .fragment_body = shaderBlock.fragment_body,
    };
  });
  pSubpipeline.prepare_shader(shader);
  quad->prepare(*shader);
  this->set_uniforms(renderer, *shader, pEntities);
  quad->render();
}

point_light::point_light() {}

point_light::point_light(const point_light_options &pOptions)
    : mOptions(pOptions) {}

point_light::~point_light() {}

shader_block point_light::get_shader_block(renderer &pRenderer,
                                           int pNumLights) {
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
              "  if (i >= uPointLightCount) break;\n"
              "  PointLight light;\n"
              "  light.position = uPointLightPositions[i].xyz;\n"
              "  light.color = uPointLightColors[i];\n"
              "  light.intensity = uPointLightRanges[i];\n"
              "  vec3 L;\n"
              "  vec3 V = normalize(uViewPos - mInfo.position);\n"
              "  vec3 N = mInfo.normal;\n"
              "  result += calcPointLight(L, V, N, mInfo.position, light) * "
              "calcBRDF(L, V, N, mInfo);\n"
              "}\n"};
}

void point_light::set_uniforms(renderer &pRenderer, shader &pShader,
                               const std::vector<entt::entity> &pEntities) {
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
  pShader.set("uPointLightCount", pos);
}

entt::hashed_string point_light::type() const {
  return entt::hashed_string{"point"};
}

const point_light_options &point_light::options() { return this->mOptions; }

void point_light::options(const point_light_options &pOptions) {
  this->mOptions = pOptions;
}

envmap_light::envmap_light() {}

envmap_light::envmap_light(const envmap_light_options &pOptions)
    : mOptions(pOptions) {}

envmap_light::~envmap_light() {}

shader_block envmap_light::get_shader_block(renderer &pRenderer,
                                            int pNumLights) {
  return {
      .id = "1",
      .vertex_dependencies = {},
      .vertex_body = "",
      .fragment_dependencies = {"res/shader/light.glsl"},
      .fragment_header = "uniform sampler2D uBRDFMap;\n"
                         "uniform samplerCube uEnvironmentMap;\n"
                         "uniform vec2 uEnvironmentMapSize;\n",
      .fragment_body =
          "result += calcEnvironmentMap(uViewPos, mInfo, uBRDFMap, "
          "uEnvironmentMap, uEnvironmentMapSize.x, uEnvironmentMapSize.y);\n"};
}

void envmap_light::set_uniforms(renderer &pRenderer, shader &pShader,
                                const std::vector<entt::entity> &pEntities) {
  auto &registry = pRenderer.registry();
  auto light = pEntities[0];
  auto &transformVal = registry.get<transform>(light);
  auto &lightVal = registry.get<light_component>(light);
  auto envmapLightVal = std::static_pointer_cast<envmap_light>(lightVal.light);
  auto &options = envmapLightVal->options();
  options.envMap->prepare(1);
  options.brdfMap->prepare(2);
  int numMipmapLevels =
      1 + std::floor(std::log2(options.envMap->options().width));
  pShader.set("uEnvironmentMap", 1);
  pShader.set("uEnvironmentMapSize",
              glm::vec2(numMipmapLevels - 3, options.power));
  pShader.set("uBRDFMap", 2);
}

void envmap_light::prepare(renderer &pRenderer,
                           const std::vector<entt::entity> &pEntities) {}

entt::hashed_string envmap_light::type() const {
  return entt::hashed_string{"envmap"};
}

const envmap_light_options &envmap_light::options() { return this->mOptions; }

void envmap_light::options(const envmap_light_options &pOptions) {
  this->mOptions = pOptions;
}
