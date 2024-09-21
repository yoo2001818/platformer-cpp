#ifndef __ANIMATION_HPP__
#define __ANIMATION_HPP__

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

struct animation_channel_base {
  entt::entity entity;
  animation_channel_interpolation intepolation;
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
  float current;
  bool playing = true;
  bool loop = true;
  float weight = 1.0;
};

class animation_controller {
public:
  std::vector<animation_action> actions;
  std::vector<animation_playback> playbacks;
};

} // namespace platformer
#endif
