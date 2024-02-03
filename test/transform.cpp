#define GLM_ENABLE_EXPERIMENTAL
#include "transform.hpp"
#include "entt/entity/fwd.hpp"
#include <catch2/catch_test_macros.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <numbers>

TEST_CASE("Basic use cases", "[transform]") {
  platformer::transform trans{};
  trans.translate(glm::vec3(0.0, 1.0, 0.0));
  REQUIRE(trans.position() == glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 target{1.0};
  target = glm::translate(target, glm::vec3(0.0, 1.0, 0.0));
  REQUIRE(glm::to_string(trans.matrix_local()) == glm::to_string(target));
}

TEST_CASE("Object hierarchy", "[transform]") {
  entt::registry registry;
  auto parent = registry.create();
  auto &parentTransform = registry.emplace<platformer::transform>(parent);
  auto child = registry.create();
  auto &childTransform = registry.emplace<platformer::transform>(child);

  childTransform.parent(parent);
  parentTransform.rotate_y(std::numbers::pi / 2.0f);
  childTransform.translate(glm::vec3(1.0, 0.0, 0.0));
  REQUIRE(glm::to_string(childTransform.position_world(registry)) ==
          glm::to_string(glm::vec3(0.0, 0.0, -1.0)));
}
