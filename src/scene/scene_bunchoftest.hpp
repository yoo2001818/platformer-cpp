#ifndef __SCENE_BUNCHOFTEST__
#define __SCENE_BUNCHOFTEST__
#include "scene.hpp"
namespace platformer {
class scene_bunchoftest : public scene {
public:
  scene_bunchoftest(){};
  virtual ~scene_bunchoftest() override{};

  virtual void init(application &pApplication, game &pGame) override;
  virtual void update(application &pApplication, game &pGame,
                      float pDelta) override;
  virtual void dispose() override;
};
} // namespace platformer
#endif
