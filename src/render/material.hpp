#ifndef __RENDER_MATERIAL_HPP__
#define __RENDER_MATERIAL_HPP__
#include "entt/entt.hpp"
#include "render/geometry.hpp"
#include "render/shader.hpp"
#include "render/texture.hpp"
#include <any>
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace platformer {
class renderer;
class subpipeline;
class material {
public:
  material();
  virtual ~material();

  virtual void render(subpipeline &pSubpipeline, geometry &pGeometry,
                      std::vector<entt::entity> &pEntities) = 0;
  virtual void dispose() = 0;
};

class shader_material : public material {
public:
  shader_material(std::string pVertex, std::string pFragment);

  virtual void render(subpipeline &pSubpipeline, geometry &pGeometry,
                      std::vector<entt::entity> &pEntities) override;
  virtual void dispose() override;

  std::map<std::string, std::any> &uniforms();

private:
  shader mShader;
  std::map<std::string, std::any> mUniforms;
};

class standard_material : public material {
public:
  standard_material();
  standard_material(glm::vec3 pColor, float pRoughness, float pMetalic);
  standard_material(std::shared_ptr<texture> diffuseTexture, float pRoughness,
                    float pMetalic);

  virtual void render(subpipeline &pSubpipeline, geometry &pGeometry,
                      std::vector<entt::entity> &pEntities) override;
  virtual void dispose() override;

  float roughness;
  float metalic;
  glm::vec3 color;
  std::shared_ptr<texture> diffuseTexture = nullptr;
  std::shared_ptr<texture> normalTexture = nullptr;
};

} // namespace platformer
#endif
