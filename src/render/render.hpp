#ifndef __RENDER_RENDER_HPP__
#define __RENDER_RENDER_HPP__
#include <any>
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <unordered_map>
namespace platformer {
template <typename T> struct asset_descriptor {
  std::string name;
  std::function<T()> exec;
};

class asset_manager {
public:
  template <typename T> T get(const asset_descriptor<T> &pDescriptor) {
    return this->get<T>(pDescriptor.name, pDescriptor.exec);
  }
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

} // namespace platformer

#endif
