#ifndef __GIZMO_HPP__
#define __GIZMO_HPP__

#include "render/buffer.hpp"
#include "render/geometry.hpp"
#include "render/shader.hpp"
#include <memory>

namespace platformer {
class renderer;
class gizmo {
public:
  gizmo(platformer::renderer &pRenderer);
  virtual ~gizmo();
  virtual void render() = 0;
  platformer::renderer &renderer() const;

protected:
  platformer::renderer &mRenderer;
};

class entity_gizmo : public gizmo {
public:
  entity_gizmo(platformer::renderer &pRenderer);
  void render() override;

private:
  std::shared_ptr<geometry> mGeometry;
  std::shared_ptr<shader> mShader;
  gl_array_buffer mBuffer;
};

} // namespace platformer

#endif
