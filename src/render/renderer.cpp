#include "render/renderer.hpp"
#include "game.hpp"
#include "render/pipeline.hpp"
#include "render/render.hpp"
#include <memory>

using namespace platformer;

renderer::renderer(platformer::game &pGame)
    : mGame(pGame), mRegistry(pGame.registry()),
      mPipeline(std::make_unique<pipeline>(*this)) {}

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
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
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
