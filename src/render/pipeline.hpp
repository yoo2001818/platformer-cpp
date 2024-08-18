#ifndef __PIPELINE_HPP__
#define __PIPELINE_HPP__

#include "entt/entity/fwd.hpp"
#include "render/shader.hpp"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace platformer {
class renderer;
struct shader_block {
  std::string id;
  std::vector<std::string> vertex_dependencies;
  std::string vertex_body;
  std::vector<std::string> fragment_dependencies;
  std::string fragment_header;
  std::string fragment_body;
};
/**
 * Pipeline coordinates rendering process, like ordering, buffer management.
 */
class pipeline {
public:
  pipeline(platformer::renderer &pRenderer);
  virtual ~pipeline();
  virtual void render() = 0;
  platformer::renderer &renderer() const;

protected:
  platformer::renderer &mRenderer;
};
/**
 * Subpipeline handles rendering objects using designated buffer.
 */
class subpipeline {
public:
  subpipeline(platformer::renderer &pRenderer, platformer::pipeline &pPipeline);
  virtual ~subpipeline();

  virtual std::shared_ptr<shader>
  get_shader(const std::string &pShaderId,
             const std::function<shader_block()> &pExec) = 0;
  virtual void prepare_shader(std::shared_ptr<shader> &pShader) = 0;
  platformer::pipeline &pipeline() const;
  platformer::renderer &renderer() const;

protected:
  platformer::pipeline &mPipeline;
  platformer::renderer &mRenderer;
};

class forward_forward_subpipeline : public subpipeline {
public:
  forward_forward_subpipeline(platformer::renderer &pRenderer,
                              platformer::pipeline &pPipeline);

  virtual std::shared_ptr<shader>
  get_shader(const std::string &pShaderId,
             const std::function<shader_block()> &pExec) override;
  virtual void prepare_shader(std::shared_ptr<shader> &pShader) override;
  void prepare_lights();

private:
  std::unordered_map<std::string, std::vector<entt::entity>> mLights = {};
  std::vector<shader_block> mLightShaderBlocks = {};
  std::string mLightShaderIds;
};

class forward_pipeline : public pipeline {
public:
  forward_pipeline(platformer::renderer &pRenderer);
  virtual void render() override;

private:
  forward_forward_subpipeline mForwardSubpipeline;
};
} // namespace platformer

#endif
