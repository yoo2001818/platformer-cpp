#ifndef __SCENE_HPP__
#define __SCENE_HPP__
#include <memory>
#include <string>
#include <vector>
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

  virtual const std::string get_name();
};

class scene_registry {
public:
  static std::vector<std::shared_ptr<scene>> &getScenes();

private:
  static std::vector<std::shared_ptr<scene>> mScenes;
};
} // namespace platformer
#endif
