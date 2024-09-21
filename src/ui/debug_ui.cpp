#include "debug_ui.hpp"
#include "entt/entity/fwd.hpp"
#include "game.hpp"
#include "imgui.h"
#include "scene/scene.hpp"
#include <cmath>

using namespace platformer;

debug_ui_system::debug_ui_system() {}

void debug_ui_system::init(game &pGame) {
  this->mGame = &pGame;
  this->mSceneIndex = 0;
  this->mDeltaFrames = 0;
  this->mDeltaTime = 0.0f;
  this->mFps = 0.0f;
}

void debug_ui_system::update(game &pGame, float pDelta) {
  this->mDeltaFrames += 1;
  this->mDeltaTime += pDelta;
  if (this->mDeltaTime >= 1.0f) {
    this->mFps = this->mDeltaFrames / this->mDeltaTime;
    this->mDeltaFrames = 0;
    this->mDeltaTime = fmodf(this->mDeltaTime, 1.0f);
  }
  ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_None);
  auto &registry = pGame.registry();
  for (auto entity : registry.storage<entt::entity>()) {
    auto nameVal = registry.try_get<name>(entity);
    if (nameVal != nullptr) {
      ImGui::Text("%s", nameVal->get().c_str());
    }
  }
  ImGui::End();

  ImGui::Begin("Scene");
  auto &scenes = scene_registry::getScenes();
  int index = 0;
  for (auto scene : scenes) {
    bool isSelected = pGame.current_scene() == scene;
    if (ImGui::RadioButton(scene->get_name().c_str(), isSelected)) {
      if (!isSelected) {
        pGame.change_scene(scene);
      }
    }
    index += 1;
  }
  ImGui::End();

  ImGui::Begin("Perf");
  ImGui::Text("FPS: %.2f", this->mFps);
  ImGui::End();
}
