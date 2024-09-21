#ifndef __NAME_HPP__
#define __NAME_HPP__

#include "entt/entity/fwd.hpp"
#include <optional>
#include <unordered_map>
#include <vector>

namespace platformer {
class name_system {
public:
  void init(entt::registry &pRegistry);
  entt::entity get(const std::string &pName);
  const std::vector<entt::entity> &get_all(const std::string &pName);

private:
  void on_construct(entt::registry &pRegistry, entt::entity pEntity);
  void on_update(entt::registry &pRegistry, entt::entity pEntity);
  void on_destroy(entt::registry &pRegistry, entt::entity pEntity);
  std::unordered_map<std::string, std::vector<entt::entity>> mMap;
};
class name {
public:
  name();
  name(const std::string &pName);
  name(std::string &&pName);

  const std::string &get() const;
  void set(const std::string &pName);
  void set(std::string &&pName);

private:
  std::string mName;
  std::optional<std::string> mPrevName;

  void mark_indexed();
  friend name_system;
};
} // namespace platformer

#endif // __NAME_HPP__
