#ifndef __SCENE_INSTANCING__
#define __SCENE_INSTANCING__
#include "scene.hpp"
namespace platformer {
class scene_instancing : public scene {
public:
  scene_instancing() {};
  virtual ~scene_instancing() override {};

  virtual void init(application &pApplication, game &pGame) override;
  virtual void update(application &pApplication, game &pGame,
                      float pDelta) override;
  virtual void dispose() override;
  virtual const std::string get_name() override;
};
} // namespace platformer
#endif
