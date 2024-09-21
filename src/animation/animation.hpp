#ifndef __ANIMATION_HPP__
#define __ANIMATION_HPP__

#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include <glm/fwd.hpp>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace platformer {

enum class animation_channel_interpolation {
  STEP,
  LINEAR,
};

enum class animation_channel_behavior {
  DEFAULT,
  CONSTANT,
  LINEAR,
  REPEAT,
};

struct animation_channel_base {
  entt::entity entity = entt::null;
  animation_channel_interpolation intepolation =
      animation_channel_interpolation::LINEAR;
  animation_channel_behavior pre_behavior = animation_channel_behavior::DEFAULT;
  animation_channel_behavior post_behavior =
      animation_channel_behavior::DEFAULT;
};

struct animation_channel_translation : public animation_channel_base {
  std::vector<std::pair<float, glm::vec3>> frames;
};

struct animation_channel_rotation : public animation_channel_base {
  std::vector<std::pair<float, glm::quat>> frames;
};

struct animation_channel_scale : public animation_channel_base {
  std::vector<std::pair<float, glm::vec3>> frames;
};

typedef std::variant<animation_channel_translation, animation_channel_rotation,
                     animation_channel_scale>
    animation_channel;

struct animation_action {
  std::string name;
  std::vector<animation_channel> channels;
  float duration;
};

struct animation_playback {
  float current = 0.0;
  bool playing = true;
  bool loop = true;
  float weight = 1.0;
};

class animation_component {
public:
  std::vector<animation_action> actions;
  std::vector<animation_playback> playbacks;
};

class game;

class animation_system {
public:
  animation_system();
  void update(game &pGame, float pDelta);
};

} // namespace platformer
#endif
