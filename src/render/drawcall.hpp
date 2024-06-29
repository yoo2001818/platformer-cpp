#ifndef __DRAWCALL_HPP__
#define __DRAWCALL_HPP__

#include "render/framebuffer.hpp"
#include "render/geometry.hpp"
#include "render/shader.hpp"
#include <any>
#include <memory>
#include <string>

namespace platformer {
struct drawcall_options {
  std::shared_ptr<platformer::framebuffer> framebuffer;
  std::shared_ptr<platformer::shader> shader;
  std::shared_ptr<platformer::geometry> geometry;
  std::map<std::string, std::any> uniforms;
  // state to be here
};

void drawcall_draw(drawcall_options &pOptions);
}; // namespace platformer

#endif
