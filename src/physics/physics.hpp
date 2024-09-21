#ifndef __PHYSICS_HPP__
#define __PHYSICS_HPP__

#include <glm/glm.hpp>

namespace platformer {
class game;
class collision {
public:
  collision(){};
  collision(const glm::vec3 &pMin, const glm::vec3 &pMax);

  glm::vec3 &min();
  void min(const glm::vec3 &pValue);
  glm::vec3 &max();
  void max(const glm::vec3 &pValue);

  void expand(glm::vec3 &pValue);
  void empty();

private:
  glm::vec3 mMin = glm::vec3(-1.0f);
  glm::vec3 mMax = glm::vec3(1.0f);
};
class physics {
public:
  physics(){};
  glm::vec3 &force();
  glm::vec3 &velocity();
  int &on_ground();

private:
  glm::vec3 mForce = glm::vec3(0.0f);
  glm::vec3 mVelocity = glm::vec3(0.0f);
  // 0 means on ground, other values means number of frames since the entity
  // started floating
  int mOnGround = 0;
};
class physics_system {
public:
  physics_system();
  void update(game &pGame, float pDelta);
  glm::vec3 mGravity;
  glm::vec3 mFriction;
};
} // namespace platformer

#endif // __PHYSICS_HPP__
