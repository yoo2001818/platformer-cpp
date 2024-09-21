#include "animation/animation.hpp"
#include "entt/entt.hpp"
#include "game.hpp"
#include "util/debug.hpp"
#include <cmath>
#include <glm/ext/quaternion_common.hpp>
#include <glm/fwd.hpp>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace platformer;

animation_system::animation_system() {}

template <typename T>
int binary_search(const std::vector<std::pair<float, T>> &pFrames,
                  float pValue) {
  int min = 0;
  int max = pFrames.size() - 2;
  while (min < max) {
    int mid = min + (max - min + 1) / 2;
    if (pFrames[mid].first > pValue) {
      max = mid - 1;
    } else {
      min = mid;
    }
  }
  return min;
}

template <typename T>
T interpolate_step(const std::vector<std::pair<float, T>> &pFrames,
                   float pTime) {
  if (pFrames.size() <= 1) {
    return pFrames[0].second;
  }
  int pos = binary_search(pFrames, pTime);
  auto tmin = pFrames[pos];
  auto tmax = pFrames[pos + 1];
  float t = std::min(
      1.0f, std::max(0.0f, (pTime - tmin.first) / (tmax.first - tmin.first)));
  if (t >= 1) {
    return tmax.second;
  } else {
    return tmin.second;
  }
}

template <typename T>
T interpolate_linear(const std::vector<std::pair<float, T>> &pFrames,
                     float pTime) {
  if (pFrames.size() <= 1) {
    return pFrames[0].second;
  }
  int pos = binary_search(pFrames, pTime);
  auto tmin = pFrames[pos];
  auto tmax = pFrames[pos + 1];
  float t = std::min(
      1.0f, std::max(0.0f, (pTime - tmin.first) / (tmax.first - tmin.first)));
  return tmin.second + (tmax.second - tmin.second) * t;
}

glm::quat
interpolate_slerp(const std::vector<std::pair<float, glm::quat>> &pFrames,
                  float pTime) {
  if (pFrames.size() <= 1) {
    return pFrames[0].second;
  }
  int pos = binary_search(pFrames, pTime);
  auto tmin = pFrames[pos];
  auto tmax = pFrames[pos + 1];
  float t = std::min(
      1.0f, std::max(0.0f, (pTime - tmin.first) / (tmax.first - tmin.first)));
  return glm::slerp(tmin.second, tmax.second, t);
}

template <typename T>
void push_entity_animation_result(
    std::unordered_map<entt::entity, std::pair<float, T>> &pMap,
    entt::entity pEntity, float weight, const T &value) {
  auto cursor = pMap.find(pEntity);
  if (cursor == pMap.end()) {
    pMap.insert_or_assign(pEntity, std::pair<float, T>{weight, value * weight});
  } else {
    cursor->second.first += weight;
    cursor->second.second += value * weight;
  }
}

void animation_system::update(game &pGame, float pDelta) {
  auto &registry = pGame.registry();
  auto view = registry.view<animation_component>();
  std::unordered_map<entt::entity, std::pair<float, glm::vec3>> translations;
  std::unordered_map<entt::entity, std::pair<float, glm::quat>> rotations;
  std::unordered_map<entt::entity, std::pair<float, glm::vec3>> scales;
  for (auto entity : view) {
    auto &anim = registry.get<animation_component>(entity);
    int numActions = anim.actions.size();
    for (int i = 0; i < numActions; i += 1) {
      auto &action = anim.actions[i];
      auto &playback = anim.playbacks[i];
      if (playback.playing) {
        playback.current = fmodf((playback.current + pDelta), action.duration);
      }
      if (playback.weight > 0.0f) {
        for (auto &channel : action.channels) {
          if (std::holds_alternative<animation_channel_translation>(channel)) {
            auto &chan = std::get<animation_channel_translation>(channel);
            auto value = interpolate_linear(chan.frames, playback.current);
            push_entity_animation_result(translations, chan.entity,
                                         playback.weight, value);
          } else if (std::holds_alternative<animation_channel_rotation>(
                         channel)) {
            auto &chan = std::get<animation_channel_rotation>(channel);
            auto value = interpolate_slerp(chan.frames, playback.current);
            push_entity_animation_result(rotations, chan.entity,
                                         playback.weight, value);
          } else if (std::holds_alternative<animation_channel_scale>(channel)) {
            auto &chan = std::get<animation_channel_scale>(channel);
            auto value = interpolate_linear(chan.frames, playback.current);
            push_entity_animation_result(scales, chan.entity, playback.weight,
                                         value);
          }
        }
      }
    }
  }
  // Apply transform to each entity
  for (auto &pair : translations) {
    auto entity = pair.first;
    auto [weight, value] = pair.second;
    value /= weight;
    auto transVal = registry.try_get<transform>(entity);
    if (transVal != nullptr) {
      transVal->position(value);
    }
  }
  for (auto &pair : rotations) {
    auto entity = pair.first;
    auto [weight, value] = pair.second;
    value /= weight;
    auto transVal = registry.try_get<transform>(entity);
    if (transVal != nullptr) {
      transVal->rotation(glm::normalize(value));
    }
  }
  for (auto &pair : scales) {
    auto entity = pair.first;
    auto [weight, value] = pair.second;
    value /= weight;
    auto transVal = registry.try_get<transform>(entity);
    if (transVal != nullptr) {
      transVal->scale(value);
    }
  }
}
