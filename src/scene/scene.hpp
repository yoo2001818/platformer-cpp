#ifndef __SCENE_HPP__
#define __SCENE_HPP__
#include <map>
#include <memory>
namespace platformer {
class game;
class application;
class scene {
public:
  scene();
  virtual ~scene();

  virtual void init(application &pApplication, game &pGame);
  virtual void update(application &pApplication, game &pGame, float pDelta);
  virtual void dispose();
};

class scene_registry {
public:
  static std::map<std::string, std::shared_ptr<scene>> &getScenes();

private:
  static std::map<std::string, std::shared_ptr<scene>> mScenes;
};
} // namespace platformer
#endif
