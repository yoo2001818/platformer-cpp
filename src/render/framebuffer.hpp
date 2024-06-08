#ifndef __RENDER_FRAMEBUFFER_HPP__
#define __RENDER_FRAMEBUFFER_HPP__

#include "render/texture.hpp"
#include <memory>
#include <optional>
#include <variant>
namespace platformer {

struct renderbuffer_options {
  int width;
  int height;
  int format;
  int samples = 0;
};

class renderbuffer {
public:
  renderbuffer(const renderbuffer_options &pOptions);

  ~renderbuffer();

  void prepare();
  void dispose();

  void options(const renderbuffer_options &pOptions);
  const renderbuffer_options &options() const;

private:
  unsigned int mRenderbuffer;
  renderbuffer_options mOptions;
};

struct framebuffer_target {
  std::variant<std::shared_ptr<renderbuffer>, std::shared_ptr<texture>> texture;
  int target = 0;
  int level = 0;
};

struct framebuffer_options {
  std::vector<framebuffer_target> colors{};
  std::optional<framebuffer_target> depth = std::nullopt;
  std::optional<framebuffer_target> stencil = std::nullopt;
  std::optional<framebuffer_target> depthStencil = std::nullopt;
};

class framebuffer {
public:
  framebuffer();
  framebuffer(const framebuffer_options &pOptions);

  ~framebuffer();

  void bind();
  void unbind();
  void dispose();

  void options(const framebuffer_options &pOptions);
  const framebuffer_options &options() const;

private:
  unsigned int mFramebuffer = -1;
  framebuffer_options mOptions;
  int mWidth;
  int mHeight;
};
}; // namespace platformer

#endif
