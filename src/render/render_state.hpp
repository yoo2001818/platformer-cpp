#ifndef __RENDER_STATE_HPP__
#define __RENDER_STATE_HPP__

#include <GL/glew.h>
#include <array>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

namespace platformer {
struct render_state {
  bool blendEnabled = false;
  glm::vec4 blendColor = glm::vec4(1.0f);
  std::array<int, 2> blendEquation = {GL_ADD, GL_ADD};
  std::array<int, 4> blendFunc = {GL_ONE, GL_ZERO, GL_ONE, GL_ZERO};

  std::array<bool, 4> colorMask = {true, true, true, true};
  bool depthMask = true;
  std::array<int, 2> stencilMask = {0xFFFF, 0xFFFF};

  bool cullEnabled = true;
  int cullFaceMode = GL_BACK;
  int frontFace = GL_CCW;

  bool depthEnabled = true;
  int depthFunc = GL_LESS;
  std::array<float, 2> depthRange = {0.0f, 1.0f};

  bool ditherEnabled = false;

  bool stencilEnabled = false;
  std::array<int, 2> stencilFunc = {GL_ALWAYS, GL_ALWAYS};
  std::array<int, 2> stencilFuncRef = {0, 0};
  std::array<int, 2> stencilFuncMask = {0xFFFF, 0xFFFF};
  std::array<int, 6> stencilFuncOp = {GL_KEEP, GL_KEEP, GL_KEEP,
                                      GL_KEEP, GL_KEEP, GL_KEEP};

  std::array<int, 4> viewport = {0, 0, 0, 0};

  bool scissorEnabled = false;
  std::array<int, 4> scissor = {0, 0, 0, 0};

  bool polygonOffsetEnabled = false;
  std::array<float, 2> polygonOffset = {0.0f, 0.0f};
};

void apply_render_state(render_state &from, const render_state &to);
} // namespace platformer

#endif
