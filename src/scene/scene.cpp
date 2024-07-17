#include "scene.hpp"
#include "scene/scene_ibl.hpp"
#include "scene_bunchoftest.hpp"
#include <memory>

using namespace platformer;

scene::scene() {}
scene::~scene() {}
void scene::init(application &pApplication, game &pGame) {}
void scene::update(application &pApplication, game &pGame, float pDelta) {}
void scene::dispose() {}
const std::string scene::get_name() { return "scene"; }

std::vector<std::shared_ptr<scene>> scene_registry::mScenes = {};

std::vector<std::shared_ptr<scene>> &scene_registry::getScenes() {
  auto &scenes = platformer::scene_registry::mScenes;
  if (scenes.empty()) {
    scenes.push_back(std::make_shared<scene_bunchoftest>());
    scenes.push_back(std::make_shared<scene_ibl>());
  }
  return scenes;
}