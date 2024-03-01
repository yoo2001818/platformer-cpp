#ifndef __PHYSICS_HPP__
#define __PHYSICS_HPP__

#include <glm/fwd.hpp>
namespace platformer {
class game;
class collision {
public:
  glm::vec3 mMin();
  glm::vec3 mMax();
};
class physics {
public:
  glm::vec3 mVelocity();
};
class physics_system {
public:
  physics_system();
  void update(game &pGame, float pDelta);
  float mGravity;
};
} // namespace platformer

#endif // __PHYSICS_HPP__
