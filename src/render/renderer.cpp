#include "render/renderer.hpp"
#include "entt/entity/fwd.hpp"
#include "game.hpp"
#include "render/pipeline.hpp"
#include "render/render.hpp"
#include <memory>
#include <vector>

using namespace platformer;

renderer::renderer(platformer::game &pGame)
    : mGame(pGame), mRegistry(pGame.registry()),
      mPipeline(std::make_unique<platformer::deferred_pipeline>(*this)) {}

void renderer::apply_render_state(const render_state &to) {
  platformer::apply_render_state(this->mRenderState, to);
}

void renderer::init() {
  SDL_GL_GetDrawableSize(this->mGame.app().window(), &(this->mWidth),
                         &(this->mHeight));
}

void renderer::clear() {
  glViewport(0, 0, this->mWidth, this->mHeight);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  this->apply_render_state({});
}

void renderer::render() { this->mPipeline->render(); }

entt::entity renderer::camera() const { return mCamera; }

void renderer::camera(entt::entity pValue) { mCamera = pValue; }

int renderer::width() const { return mWidth; }

void renderer::width(int pValue) { mWidth = pValue; }

int renderer::height() const { return mHeight; }

void renderer::height(int pValue) { mHeight = pValue; }

platformer::game &renderer::game() const { return this->mGame; }
platformer::asset_manager &renderer::asset_manager() {
  return this->mAssetManager;
}
entt::registry &renderer::registry() { return this->mRegistry; }
platformer::pipeline &renderer::pipeline() { return *this->mPipeline; }
std::vector<std::shared_ptr<gizmo>> &renderer::gizmos() {
  return this->mGizmos;
}
