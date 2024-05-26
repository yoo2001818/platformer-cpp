#include "debug_ui.hpp"
#include "entt/entity/fwd.hpp"
#include "game.hpp"
#include "imgui.h"

using namespace platformer;

debug_ui_system::debug_ui_system() {}

void debug_ui_system::update(game &pGame, float pDelta) {
  ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_None);
  auto &registry = pGame.registry();
  for (auto entity : registry.storage<entt::entity>()) {
    auto nameVal = registry.try_get<name>(entity);
    if (nameVal != nullptr) {
      ImGui::Text("%s", nameVal->get().c_str());
    }
  }
  ImGui::End();
}
