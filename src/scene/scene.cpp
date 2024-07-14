#include "scene.hpp"
#include "scene_bunchoftest.hpp"
#include <memory>

using namespace platformer;

scene::scene() {}
scene::~scene() {}
void scene::init(application &pApplication, game &pGame) {}
void scene::update(application &pApplication, game &pGame, float pDelta) {}
void scene::dispose() {}

std::map<std::string, std::shared_ptr<scene>> scene_registry::mScenes = {};

std::map<std::string, std::shared_ptr<scene>> &scene_registry::getScenes() {
  auto &scenes = platformer::scene_registry::mScenes;
  if (scenes.empty()) {
    scenes.emplace("bunchoftest", std::make_shared<scene_bunchoftest>());
  }
  return scenes;
}
