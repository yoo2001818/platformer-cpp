#include "gizmo.hpp"
#include "render/renderer.hpp"

using namespace platformer;

gizmo::gizmo(platformer::renderer &pRenderer) : mRenderer(pRenderer) {}
gizmo::~gizmo() {}
void gizmo::render() {}
platformer::renderer &gizmo::renderer() const { return this->mRenderer; }

entity_gizmo::entity_gizmo(platformer::renderer &pRenderer)
    : gizmo(pRenderer) {}
void entity_gizmo::render() {}
