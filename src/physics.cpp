#include "physics.hpp"

using namespace platformer;

collision::collision(const glm::vec3 &pMin, const glm::vec3 &pMax)
    : mMin(pMin), mMax(pMax) {}

glm::vec3 &collision::min() { return this->mMin; }
void collision::min(const glm::vec3 &pValue) { this->mMin = pValue; }
glm::vec3 &collision::max() { return this->mMax; }
void collision::max(const glm::vec3 &pValue) { this->mMax = pValue; }
void collision::expand(glm::vec3 &pValue) {
  this->mMin = glm::min(this->mMin, pValue);
  this->mMax = glm::max(this->mMax, pValue);
}
void collision::empty() {
  this->mMin = glm::vec3(0.0f);
  this->mMax = glm::vec3(0.0f);
}

glm::vec3 &physics::force() { return this->mForce; }
glm::vec3 &physics::velocity() { return this->mVelocity; }

void physics_system::update(game &pGame, float pDelta) {
  // Update all entities with physics, collision, transform, while testing for
  // collisions with collision, transform entities
}
