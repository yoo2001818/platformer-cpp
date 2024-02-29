#ifndef __PHYSICS_HPP__
#define __PHYSICS_HPP__

#include <glm/fwd.hpp>
namespace platformer {
class physics {
public:
  glm::vec3 mMin();
  glm::vec3 mMax();
  glm::vec3 mVelocity();
};
} // namespace platformer

#endif // __PHYSICS_HPP__
