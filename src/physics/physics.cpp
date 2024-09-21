#include "physics/physics.hpp"
#include "game.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include "scenegraph/transform.hpp"

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
int &physics::on_ground() { return this->mOnGround; }

physics_system::physics_system()
    : mGravity(glm::vec3(0.0f, -20.0f, 0.0f)),
      mFriction(glm::vec3(0.1f, 0.1f, 0.1f)) {}

void physics_system::update(game &pGame, float pDelta) {
  // Update all entities with physics, collision, transform, while testing for
  // collisions with collision, transform entities
  auto &registry = pGame.registry();
  auto physicsView = registry.view<transform, collision, physics>();
  auto collisionView = registry.view<transform, collision>();

  // Update force, velocity of objects first
  for (auto entity : physicsView) {
    auto &physicsVal = registry.get<physics>(entity);
    auto &transformVal = registry.get<transform>(entity);
    auto &velocity = physicsVal.velocity();
    auto &force = physicsVal.force();
    force += this->mGravity * pDelta;
    force -=
        glm::sign(velocity) * velocity * velocity * this->mFriction * pDelta;
    velocity += force;
    force = glm::vec3(0.0f);
    physicsVal.on_ground() += 1;
    // FIXME: This should operate on world positions
    transformVal.translate(physicsVal.velocity() * pDelta);
  }

  // Check for collisions and resolve it
  for (auto entity : physicsView) {
    auto &physicsVal = registry.get<physics>(entity);
    auto &transformVal = registry.get<transform>(entity);
    auto &collisionVal = registry.get<collision>(entity);
    glm::vec3 aPoint = transformVal.matrix_world(registry) *
                       glm::vec4(collisionVal.min(), 1.0f);
    glm::vec3 bPoint = transformVal.matrix_world(registry) *
                       glm::vec4(collisionVal.max(), 1.0f);
    glm::vec3 minPoint = glm::min(aPoint, bPoint);
    glm::vec3 maxPoint = glm::max(aPoint, bPoint);

    for (auto target : collisionView) {
      if (entity == target) {
        continue;
      }
      // FIXME: Couldn't it be calculated beforehand? It's expensive
      auto &transformTarget = registry.get<transform>(target);
      auto &collisionTarget = registry.get<collision>(target);
      glm::vec3 aTarget = transformTarget.matrix_world(registry) *
                          glm::vec4(collisionTarget.min(), 1.0f);
      glm::vec3 bTarget = transformTarget.matrix_world(registry) *
                          glm::vec4(collisionTarget.max(), 1.0f);
      glm::vec3 minTarget = glm::min(aTarget, bTarget);
      glm::vec3 maxTarget = glm::max(aTarget, bTarget);

      bool collided = true;
      for (int i = 0; i < 3; ++i) {
        if (minTarget[i] > maxPoint[i] || minPoint[i] > maxTarget[i]) {
          collided = false;
          break;
        }
      }
      if (!collided) {
        continue;
      }
      // Collision occurred
      physicsVal.velocity() = glm::vec3(0.0f);
      physicsVal.on_ground() = 0;
      float offset = maxTarget.y - minPoint.y;
      transformVal.translate(glm::vec3(0.0f, offset, 0.0f));
    }
  }
}
