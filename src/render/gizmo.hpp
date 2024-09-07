#ifndef __GIZMO_HPP__
#define __GIZMO_HPP__

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
};

} // namespace platformer

#endif
