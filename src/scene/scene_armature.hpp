#ifndef __SCENE_ARMATURE__
#define __SCENE_ARMATURE__
#include "scene.hpp"
namespace platformer {
class scene_armature : public scene {
public:
  scene_armature() {};
  virtual ~scene_armature() override {};

  virtual void init(application &pApplication, game &pGame) override;
  virtual void update(application &pApplication, game &pGame,
                      float pDelta) override;
  virtual void dispose() override;
  virtual const std::string get_name() override;
};
} // namespace platformer
#endif
