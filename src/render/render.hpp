#ifndef __RENDER_RENDER_HPP__
#define __RENDER_RENDER_HPP__
#include <any>
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <unordered_map>
namespace platformer {
class mesh;

/*
class light {
public:
  glm::vec3 color;
  float power;
  float radius;
  float range;
};
*/

class asset_manager {
public:
  template <typename T>
  T get(const std::string &pName, const std::function<T()> &pExec) {
    auto cursor = mMap.find(pName);
    if (cursor == mMap.end()) {
      auto result = pExec();
      mMap.insert_or_assign(pName, result);
      return result;
    }
    return std::any_cast<T>(cursor->second);
  }

private:
  std::unordered_map<std::string, std::any> mMap;
};

struct render_light {
  glm::vec4 position;
  glm::vec3 color;
  glm::vec3 range;
};

} // namespace platformer

#endif
