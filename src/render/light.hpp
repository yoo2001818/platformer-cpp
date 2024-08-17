#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "entt/core/hashed_string.hpp"
#include "entt/entity/fwd.hpp"
#include "render/renderer.hpp"
#include "render/shader.hpp"
#include "render/texture.hpp"
#include <memory>
#include <vector>
namespace platformer {
class shader_block;
class light {
public:
  light() {};
  virtual ~light() {};
  virtual shader_block get_shader_block(int pNumLights,
                                        renderer &pRenderer) = 0;
  virtual void set_uniforms(shader &pShader,
                            std::vector<entt::entity> pEntities,
                            renderer &pRenderer) = 0;
  virtual void prepare(std::vector<entt::entity> pEntities,
                       renderer &pRenderer) = 0;
  virtual entt::hashed_string type() const = 0;
};

struct point_light_options {
  glm::vec3 color;
  float power;
  float radius;
  float range;
};

class point_light : public light {
public:
  point_light();
  point_light(const point_light_options &pOptions);
  virtual ~point_light();

  virtual shader_block get_shader_block(int pNumLights,
                                        renderer &pRenderer) override;
  virtual void set_uniforms(shader &pShader,
                            std::vector<entt::entity> pEntities,
                            renderer &pRenderer) override;
  virtual void prepare(std::vector<entt::entity> pEntities,
                       renderer &pRenderer) override;
  virtual entt::hashed_string type() const override;

  const point_light_options &options();
  void options(const point_light_options &pOptions);

private:
  point_light_options mOptions;
};

struct envmap_light_options {
  std::shared_ptr<texture> envMap;
  std::shared_ptr<texture> brdfMap;
  float power = 1.0f;
};

class envmap_light : public light {
public:
  envmap_light();
  envmap_light(const envmap_light_options &pOptions);
  virtual ~envmap_light();

  virtual shader_block get_shader_block(int pNumLights,
                                        renderer &pRenderer) override;
  virtual void set_uniforms(shader &pShader,
                            std::vector<entt::entity> pEntities,
                            renderer &pRenderer) override;
  virtual void prepare(std::vector<entt::entity> pEntities,
                       renderer &pRenderer) override;
  virtual entt::hashed_string type() const override;

  const envmap_light_options &options();
  void options(const envmap_light_options &pOptions);

private:
  envmap_light_options mOptions;
};

struct light_component {
  std::shared_ptr<platformer::light> light;
};

} // namespace platformer

#endif
