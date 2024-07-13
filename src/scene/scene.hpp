#ifndef __SCENE_HPP__
#define __SCENE_HPP__
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
} // namespace platformer
#endif
