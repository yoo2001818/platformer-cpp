#ifndef __LIGHT_HPP__
#define __LIGHT_HPP__

#include "entt/entity/fwd.hpp"
#include "render/renderer.hpp"
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
  virtual void set_uniforms(std::vector<entt::entity> pEntities,
                            renderer &pRenderer) = 0;
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
  virtual void set_uniforms(std::vector<entt::entity> pEntities,
                            renderer &pRenderer) override;

private:
  point_light_options mOptions;
};

struct envmap_light_options {
  std::shared_ptr<texture> envMap;
  std::shared_ptr<texture> brdfMap;
  float power;
};

class envmap_light : public light {
public:
  envmap_light();
  envmap_light(const envmap_light_options &pOptions);
  virtual ~envmap_light();

  virtual shader_block get_shader_block(int pNumLights,
                                        renderer &pRenderer) override;
  virtual void set_uniforms(std::vector<entt::entity> pEntities,
                            renderer &pRenderer) override;

private:
  envmap_light_options mOptions;
};

struct light_component {
  std::shared_ptr<platformer::light> light;
};

} // namespace platformer

#endif
