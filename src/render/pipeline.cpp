#include "render/pipeline.hpp"
#include "game.hpp"
#include "render/camera.hpp"
#include "render/framebuffer.hpp"
#include "render/geometry.hpp"
#include "render/light.hpp"
#include "render/mesh.hpp"
#include "render/renderer.hpp"
#include "render/shader.hpp"
#include "render/shader_preprocessor.hpp"
#include "render/texture.hpp"
#include <functional>
#include <memory>
#include <sstream>
#include <vector>

using namespace platformer;

pipeline::pipeline(platformer::renderer &pRenderer) : mRenderer(pRenderer) {}
pipeline::~pipeline() {}
platformer::renderer &pipeline::renderer() const { return this->mRenderer; }

subpipeline::subpipeline(platformer::renderer &pRenderer,
                         platformer::pipeline &pPipeline)
    : mRenderer(pRenderer), mPipeline(pPipeline) {}
subpipeline::~subpipeline() {}

platformer::pipeline &subpipeline::pipeline() const { return this->mPipeline; }
platformer::renderer &subpipeline::renderer() const { return this->mRenderer; }

forward_forward_subpipeline::forward_forward_subpipeline(
    platformer::renderer &pRenderer, platformer::pipeline &pPipeline)
    : subpipeline(pRenderer, pPipeline) {}

std::shared_ptr<shader> forward_forward_subpipeline::get_shader(
    const std::string &pShaderId, const std::function<shader_block()> &pExec) {
  // This assumes the following structure for the fragment body:
  // void material(out MaterialInfo mInfo) {
  //  ...
  // }
  // It is free to read any information from uniforms, varying, etc, otherwise.
  auto &assetManager = this->mRenderer.asset_manager();
  return assetManager.get<std::shared_ptr<shader>>(
      "shader~forward~" + pShaderId + this->mLightShaderIds, [&]() {
        auto shaderBlock = pExec();
        std::stringstream vertex;
        vertex << "#version 330 core\n";
        for (auto &file : shaderBlock.vertex_dependencies) {
          vertex << "#include " << file << "\n";
        }
        vertex << shaderBlock.vertex_body;

        std::stringstream fragment;
        fragment << "#version 330 core\n";
        fragment << "#include \"res/shader/pbr.glsl\"\n";
        for (auto &file : shaderBlock.fragment_dependencies) {
          fragment << "#include \"" << file << "\"\n";
        }
        for (auto &entry : this->mLightShaderBlocks) {
          for (auto &file : entry.fragment_dependencies) {
            fragment << "#include \"" << file << "\"\n";
          }
        }

        fragment << shaderBlock.fragment_header << "\n";
        for (auto &entry : this->mLightShaderBlocks) {
          fragment << entry.fragment_header << "\n";
        }

        fragment << "uniform vec3 uViewPos;\n"
                    "out vec4 FragColor;\n"
                    "void main() {\n"
                    "  MaterialInfo mInfo;\n"
                    "  {\n";
        fragment << shaderBlock.fragment_body << "\n";
        fragment << "  }\n"
                    "  vec3 result = vec3(0.0);\n";
        for (auto &entry : this->mLightShaderBlocks) {
          fragment << entry.fragment_body << "\n";
        }
        fragment << "  vec3 tonemappedColor = pow(result, vec3(1.0 / 2.2));\n"
                    "  FragColor = vec4(tonemappedColor, 1.0);\n"
                    "}\n";

        shader_preprocessor vertexProc(vertex.str());
        shader_preprocessor fragmentProc(fragment.str());

        return std::make_shared<shader>(vertexProc.get(), fragmentProc.get());
      });
}

void forward_forward_subpipeline::prepare_shader(
    std::shared_ptr<shader> &pShader) {
  auto &registry = this->mRenderer.game().registry();
  pShader->prepare();
  // Well, it should set the renderer state, framebuffer, etc, but we don't
  // have any of that in forward rendering
  camera_handle camHandle(this->mRenderer);
  pShader->set("uView", camHandle.view());
  pShader->set("uViewPos", camHandle.view_pos());
  pShader->set("uProjection", camHandle.projection());
  // Set uniforms for lights
  for (auto &entry : this->mLights) {
    auto &entities = entry.second;
    auto &light = registry.get<light_component>(entities[0]).light;
    light->set_uniforms(*pShader, entities, this->mRenderer);
  }
}

void forward_forward_subpipeline::prepare_lights() {
  auto &registry = this->mRenderer.game().registry();
  auto view = registry.view<transform, light_component>();
  this->mLights.clear();
  // Group all lights in the separate vector
  for (auto entity : view) {
    auto &light = registry.get<light_component>(entity).light;
    auto lightType = std::string(light->type());
    auto current = this->mLights.find(lightType);
    if (current != this->mLights.end()) {
      current->second.push_back(entity);
    } else {
      this->mLights.insert({lightType, {entity}});
    }
  }
  this->mLightShaderBlocks.clear();
  this->mLightShaderIds = "";
  // Prepare lights and generate light shader blocks
  for (auto &entry : this->mLights) {
    auto &entities = entry.second;
    auto &light = registry.get<light_component>(entities[0]).light;
    light->prepare(entities, this->mRenderer);
    auto shaderBlock =
        light->get_shader_block(entities.size(), this->mRenderer);
    this->mLightShaderBlocks.push_back(shaderBlock);
    this->mLightShaderIds +=
        "~" + std::string(entry.first) + "-" + shaderBlock.id;
  }
}

forward_pipeline::forward_pipeline(platformer::renderer &pRenderer)
    : pipeline(pRenderer), mForwardSubpipeline(pRenderer, *this) {}

void forward_pipeline::render() {
  auto &registry = this->mRenderer.game().registry();
  auto cameraEntity = this->mRenderer.camera();
  auto &cameraTransform = registry.get<transform>(cameraEntity);
  auto &cameraCamera = registry.get<platformer::camera>(cameraEntity);
  this->mForwardSubpipeline.prepare_lights();
  auto view = registry.view<transform, mesh>();
  for (auto entity : view) {
    auto &meshVal = registry.get<mesh>(entity);
    meshVal.render(this->mForwardSubpipeline, entity);
  }
}

deferred_forward_subpipeline::deferred_forward_subpipeline(
    platformer::renderer &pRenderer, platformer::pipeline &pPipeline,
    framebuffer &pFramebuffer)
    : subpipeline(pRenderer, pPipeline), mFramebuffer(pFramebuffer) {}

std::shared_ptr<shader> deferred_forward_subpipeline::get_shader(
    const std::string &pShaderId, const std::function<shader_block()> &pExec) {}

void deferred_forward_subpipeline::prepare_shader(
    std::shared_ptr<shader> &pShader) {}

void deferred_forward_subpipeline::prepare_lights() {}

deferred_deferred_subpipeline::deferred_deferred_subpipeline(
    platformer::renderer &pRenderer, platformer::pipeline &pPipeline,
    framebuffer &pFramebuffer)
    : subpipeline(pRenderer, pPipeline), mFramebuffer(pFramebuffer) {}

std::shared_ptr<shader> deferred_deferred_subpipeline::get_shader(
    const std::string &pShaderId, const std::function<shader_block()> &pExec) {
  auto &assetManager = this->mRenderer.asset_manager();
  return assetManager.get<std::shared_ptr<shader>>(
      "shader~deferred-deferred~" + pShaderId, [&]() {
        auto shaderBlock = pExec();
        std::stringstream vertex;
        vertex << "#version 330 core\n";
        for (auto &file : shaderBlock.vertex_dependencies) {
          vertex << "#include " << file << "\n";
        }
        vertex << shaderBlock.vertex_body;

        std::stringstream fragment;
        fragment << "#version 330 core\n";
        fragment << "#include \"res/shader/pbr.glsl\"\n";
        for (auto &file : shaderBlock.fragment_dependencies) {
          fragment << "#include \"" << file << "\"\n";
        }

        fragment << shaderBlock.fragment_header << "\n";
        fragment << "uniform vec3 uViewPos;\n"
                    "out vec4 FragColor;\n"
                    "out vec4 FragColor2;\n"
                    "void main() {\n"
                    "  MaterialInfo mInfo;\n"
                    "  {\n";
        fragment << shaderBlock.fragment_body << "\n";
        fragment << "  }\n"
                    "  vec4 vecOut[2] = packMaterialInfo(mInfo);\n"
                    "  FragColor = vecOut[0];\n"
                    "  FragColor2 = vecOut[1];\n"
                    "}\n";

        shader_preprocessor vertexProc(vertex.str());
        shader_preprocessor fragmentProc(fragment.str());

        return std::make_shared<shader>(vertexProc.get(), fragmentProc.get());
      });
}

void deferred_deferred_subpipeline::prepare_shader(
    std::shared_ptr<shader> &pShader) {
  this->mFramebuffer.bind();
  auto &registry = this->mRenderer.game().registry();
  pShader->prepare();
  camera_handle camHandle(this->mRenderer);
  pShader->set("uView", camHandle.view());
  pShader->set("uViewPos", camHandle.view_pos());
  pShader->set("uProjection", camHandle.projection());
}

deferred_pipeline::deferred_pipeline(platformer::renderer &pRenderer)
    : pipeline(pRenderer), mGBuffer0(std::make_shared<texture_2d>()),
      mGBuffer1(std::make_shared<texture_2d>()),
      mDepthBuffer(std::make_shared<texture_2d>()),
      mColorBuffer(std::make_shared<texture_2d>()),
      mMeshPassFb(framebuffer_options{
          .colors = {{this->mGBuffer0}, {this->mGBuffer1}},
          .depth = {{this->mDepthBuffer}},
      }),
      mForwardMeshPassFb(framebuffer_options{.colors = {{this->mColorBuffer}},
                                             .depth = {{this->mDepthBuffer}}}),
      mLightPassFb(framebuffer_options{
          .colors = {{this->mColorBuffer}},
          .depth = {{this->mDepthBuffer}},
      }),
      mForwardSubpipeline(pRenderer, *this, mForwardMeshPassFb),
      mDeferredSubpipeline(pRenderer, *this, mMeshPassFb) {}

void deferred_pipeline::render() {
  auto &renderer = this->renderer();
  // Regenerate previous buffers if they are resized (We could technically reuse
  // them if the window is getting smaller though)
  auto &prevOptions = this->mGBuffer0->options();
  if (prevOptions.width != renderer.width() ||
      prevOptions.height != renderer.height()) {
    // TODO: This is certainly not ideal, but are there better ways to do this?
    this->mGBuffer0->source(texture_source_buffer{
        .format =
            {
                .format = GL_RGBA,
                .internalFormat = GL_RGBA,
                .type = GL_UNSIGNED_BYTE,
            },
        .width = renderer.width(),
        .height = renderer.height(),
    });
    this->mGBuffer0->options({
        .magFilter = GL_LINEAR,
        .minFilter = GL_LINEAR,
        .wrapS = GL_CLAMP,
        .wrapT = GL_CLAMP,
        .width = renderer.width(),
        .height = renderer.height(),
        .mipmap = false,
    });
    this->mGBuffer1->source(texture_source_buffer{
        .format =
            {
                .format = GL_RGBA,
                .internalFormat = GL_RGBA,
                .type = GL_UNSIGNED_BYTE,
            },
        .width = renderer.width(),
        .height = renderer.height(),
    });
    this->mGBuffer1->options({
        .magFilter = GL_LINEAR,
        .minFilter = GL_LINEAR,
        .wrapS = GL_CLAMP,
        .wrapT = GL_CLAMP,
        .width = renderer.width(),
        .height = renderer.height(),
        .mipmap = false,
    });
    this->mDepthBuffer->source(texture_source_buffer{
        .format =
            {
                .format = GL_DEPTH_COMPONENT,
                .internalFormat = GL_DEPTH_COMPONENT24,
                .type = GL_UNSIGNED_INT,
            },
        .width = renderer.width(),
        .height = renderer.height(),
    });
    this->mDepthBuffer->options({
        .magFilter = GL_LINEAR,
        .minFilter = GL_LINEAR,
        .wrapS = GL_CLAMP,
        .wrapT = GL_CLAMP,
        .width = renderer.width(),
        .height = renderer.height(),
        .mipmap = false,
    });
    this->mColorBuffer->source(texture_source_buffer{
        .format =
            {
                .format = GL_RGB,
                .internalFormat = GL_RGB,
                .type = GL_HALF_FLOAT,
            },
        .width = renderer.width(),
        .height = renderer.height(),
    });
    this->mColorBuffer->options({
        .magFilter = GL_LINEAR,
        .minFilter = GL_LINEAR,
        .wrapS = GL_CLAMP,
        .wrapT = GL_CLAMP,
        .width = renderer.width(),
        .height = renderer.height(),
        .mipmap = false,
    });
    this->mMeshPassFb.invalidate();
    this->mForwardMeshPassFb.invalidate();
    this->mLightPassFb.invalidate();
  }
  // Clear depth and color buffer
  this->mMeshPassFb.bind();
  glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDrawBuffer(GL_COLOR_ATTACHMENT1);
  glClear(GL_COLOR_BUFFER_BIT);
  this->mLightPassFb.bind();
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  // Prepare lights
  this->mForwardSubpipeline.prepare_lights();
  // Render objects
  auto &registry = this->mRenderer.game().registry();
  auto view = registry.view<transform, mesh>();
  this->mMeshPassFb.bind();
  for (auto entity : view) {
    this->mMeshPassFb.unbind();
    auto &meshVal = registry.get<mesh>(entity);
    meshVal.render(this->mDeferredSubpipeline, entity);
  }
  this->mMeshPassFb.unbind();
  // Present the G-buffer to the screen for debugging
  auto &assetManager = this->mRenderer.asset_manager();
  auto quad = assetManager.get<std::shared_ptr<geometry>>("quad", []() {
    return std::make_shared<geometry>(geometry::make_quad());
  });
  auto presentShader =
      assetManager.get<std::shared_ptr<shader>>("shader~presentShader", []() {
        return std::make_shared<shader>(
            "#version 330 core\n"
            "layout(location = 0) in vec3 aPosition;\n"
            "layout(location = 1) in vec2 aTexCoord;\n"
            "out vec2 vTexCoord;\n"
            "void main() {\n"
            "  gl_Position = vec4(aPosition.xy, 1.0, 1.0);\n"
            "  vTexCoord = aTexCoord;\n"
            "}\n",
            "#version 330 core\n"
            "in vec2 vTexCoord;\n"
            "out vec4 FragColor;\n"
            "uniform sampler2D uBuffer;\n"
            "void main() {\n"
            "  vec4 color = texture2D(uBuffer, vTexCoord);\n"
            "  FragColor = vec4(color.rgb, 1.0);\n"
            "}\n");
      });
  presentShader->prepare();
  quad->prepare(*presentShader);
  this->mGBuffer0->prepare(0);
  presentShader->set("uBuffer", 0);
  quad->render();
}
