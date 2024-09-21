#include "animation/animation.hpp"
#include "game.hpp"
#include <cmath>

using namespace platformer;

animation_system::animation_system() {}

void animation_system::update(game &pGame, float pDelta) {
  auto &registry = pGame.registry();
  auto view = registry.view<animation_component>();
  for (auto entity : view) {
    auto &anim = registry.get<animation_component>(entity);
    int numActions = anim.actions.size();
    for (int i = 0; i < numActions; i += 1) {
      auto &action = anim.actions[i];
      auto &playback = anim.playbacks[i];
      if (playback.playing) {
        playback.current += pDelta;
        playback.current = fmodf((playback.current + pDelta), action.duration);
      }
      if (playback.weight > 0.0f) {
      }
    }
  }
}
