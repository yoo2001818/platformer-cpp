#include "gizmo.hpp"
#include "render/buffer.hpp"
#include "render/camera.hpp"
#include "render/geometry.hpp"
#include "render/mesh.hpp"
#include "render/renderer.hpp"
#include "render/shader.hpp"
#include "transform.hpp"
#include <memory>

using namespace platformer;

gizmo::gizmo(platformer::renderer &pRenderer) : mRenderer(pRenderer) {}
gizmo::~gizmo() {}
void gizmo::render() {}
platformer::renderer &gizmo::renderer() const { return this->mRenderer; }

entity_gizmo::entity_gizmo(platformer::renderer &pRenderer)
    : gizmo(pRenderer),
      mGeometry(std::make_shared<geometry>(geometry::make_quad())),
      mShader(std::make_shared<file_shader>("res/shader/gizmo.vert",
                                            "res/shader/gizmo.frag")),
      mBuffer(GL_STREAM_DRAW) {}
void entity_gizmo::render() {
  auto &registry = this->renderer().registry();
  auto view = registry.view<transform>(entt::exclude<mesh_component>);
  auto &shader = this->mShader;
  auto width = this->renderer().width();
  auto height = this->renderer().height();
  std::vector<glm::vec2> positions;

  camera_handle camHandle(this->mRenderer);
  auto viewMat = camHandle.view();
  auto projectionMat = camHandle.projection();
  auto projectionViewMat = projectionMat * viewMat;

  for (auto entity : view) {
    auto &transformVal = registry.get<transform>(entity);
    auto &modelMat = transformVal.matrix_world(registry);
    glm::vec4 pos =
        (projectionViewMat * modelMat) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    pos /= pos.w;
    if (pos.z >= -1.0f && pos.z <= 1.0f) {
      positions.push_back(glm::vec2(pos.x, pos.y));
    }
  }

  if (positions.size() > 0) {
    this->renderer().apply_render_state({
        .depthEnabled = false,
    });
    this->mBuffer.set(positions);
    shader->prepare();
    mGeometry->prepare(*(shader));
    this->mBuffer.bind();
    shader->set_attribute("aCenter", 0, 2, GL_FLOAT, GL_FLOAT,
                          sizeof(glm::vec2), 0, 1);
    shader->set("uScale", glm::vec2(6.0f / width, 6.0f / height));
    mGeometry->render(positions.size());
  }
}
