#include "scenegraph/name.hpp"
#include "entt/entt.hpp"
#include <optional>
#include <string>
#include <vector>

using namespace platformer;

void name_system::init(entt::registry &pRegistry) {
  pRegistry.on_construct<name>().connect<&name_system::on_construct>(*this);
  pRegistry.on_update<name>().connect<&name_system::on_update>(*this);
  pRegistry.on_destroy<name>().connect<&name_system::on_destroy>(*this);
}

entt::entity name_system::get(const std::string &pName) {
  auto &list = this->get_all(pName);
  if (list.size() == 0) {
    return entt::null;
  }
  return list[0];
}

const std::vector<entt::entity> &
name_system::get_all(const std::string &pName) {
  return this->mMap[pName];
}

void name_system::on_construct(entt::registry &pRegistry,
                               entt::entity pEntity) {
  auto &nameVal = pRegistry.get<name>(pEntity);
  auto &list = mMap[nameVal.mName];
  list.push_back(pEntity);
};

void name_system::on_update(entt::registry &pRegistry, entt::entity pEntity) {
  auto &nameVal = pRegistry.get<name>(pEntity);
  if (nameVal.mPrevName != std::nullopt) {
    auto iter = mMap.find(nameVal.mPrevName.value());
    if (iter != mMap.end()) {
      auto &list = iter->second;
      list.erase(std::remove(list.begin(), list.end(), pEntity));
    }
    auto &list = mMap[nameVal.mName];
    list.push_back(pEntity);
    nameVal.mark_indexed();
  }
};

void name_system::on_destroy(entt::registry &pRegistry, entt::entity pEntity) {
  auto &nameVal = pRegistry.get<name>(pEntity);
  auto iter = mMap.find(nameVal.mName);
  if (iter != mMap.end()) {
    auto &list = iter->second;
    list.erase(std::remove(list.begin(), list.end(), pEntity));
  }
};

name::name() : mName() {}
name::name(const std::string &pName) : mName(pName) {}
name::name(std::string &&pName) : mName(pName) {}

const std::string &name::get() const { return this->mName; }
void name::set(const std::string &pName) {
  if (this->mPrevName == std::nullopt && this->mName != pName) {
    this->mPrevName = this->mName;
  }
  this->mName = pName;
}
void name::set(std::string &&pName) {
  if (this->mPrevName == std::nullopt && this->mName != pName) {
    this->mPrevName = this->mName;
  }
  this->mName = pName;
}

void name::mark_indexed() { this->mPrevName = std::nullopt; }
