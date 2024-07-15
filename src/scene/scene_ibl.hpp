#ifndef __SCENE_IBL__
#define __SCENE_IBL__
#include "scene.hpp"
namespace platformer {
class scene_ibl : public scene {
public:
  scene_ibl(){};
  virtual ~scene_ibl() override{};

  virtual void init(application &pApplication, game &pGame) override;
  virtual void update(application &pApplication, game &pGame,
                      float pDelta) override;
  virtual void dispose() override;
  virtual const std::string get_name() override;
};
} // namespace platformer
#endif
