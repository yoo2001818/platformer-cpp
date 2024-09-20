#ifndef __RENDER_TEXTURE_HPP__
#define __RENDER_TEXTURE_HPP__

#include "render/buffer.hpp"
#include <GL/glew.h>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace platformer {
class framebuffer;

struct texture_options {
  int magFilter = GL_LINEAR;
  int minFilter = GL_LINEAR_MIPMAP_LINEAR;
  int wrapS = GL_REPEAT;
  int wrapT = GL_REPEAT;
  int width = 0;
  int height = 0;
  bool mipmap = true;
};

struct texture_format {
  int format = GL_RGBA;
  int internalFormat = GL_RGBA;
  int type = GL_UNSIGNED_BYTE;
};

struct texture_source_buffer {
  texture_format format;
  int width = 0;
  int height = 0;
  std::optional<std::vector<std::byte>> data;
};

struct texture_source_image {
  texture_format format;
  std::string filename;
  std::optional<std::vector<std::byte>> data;
};

typedef std::variant<texture_source_buffer, texture_source_image>
    texture_source;

struct texture_cube_source {
  texture_source right;
  texture_source left;
  texture_source up;
  texture_source down;
  texture_source front;
  texture_source back;
};

class texture {
public:
  texture();
  texture(const texture &pValue);
  texture(texture &&pValue);
  texture(const texture_options &pOptions);

  virtual texture &operator=(const texture &pValue);
  virtual texture &operator=(texture &&pValue);

  virtual ~texture();

  void prepare(int pSlot);
  void dispose();
  void generate_mipmaps();

  const texture_options &options() const;
  void options(const texture_options &pOptions);

  bool is_valid();

protected:
  texture_options mOptions{};
  virtual void init();
  virtual int type();
  void upload(int pTarget, const texture_source &pSource,
              texture_options &pOptions);
  void set_options(int pTarget, const texture_options &pOptions);
  void generate_mipmap(int pTarget);
  bool mIsValid = false;

private:
  unsigned int mTexture = -1;
  friend framebuffer;
};

class texture_2d : public texture {
public:
  texture_2d();
  texture_2d(const texture_2d &pValue);
  texture_2d(texture_2d &&pValue);
  texture_2d(const texture_source &pSource);
  texture_2d(texture_source &&pSource);
  texture_2d(const texture_source &pSource, const texture_options &pOptions);
  texture_2d(texture_source &&pSource, const texture_options &pOptions);

  virtual texture_2d &operator=(const texture &pValue) override;
  virtual texture_2d &operator=(texture &&pValue) override;

  virtual ~texture_2d();

  const texture_source &source() const;
  void source(const texture_source &pSource);
  void source(texture_source &&pSource);

protected:
  virtual void init() override;
  virtual int type() override;

private:
  texture_source mSource{};
};

class texture_cube : public texture {
public:
  texture_cube();
  texture_cube(const texture_cube_source &pSource);
  texture_cube(texture_cube_source &&pSource);
  texture_cube(const texture_cube_source &pSource,
               const texture_options &pOptions);
  texture_cube(texture_cube_source &&pSource, const texture_options &pOptions);

  virtual ~texture_cube();

  const texture_cube_source &source() const;
  void source(const texture_cube_source &pSource);
  void source(texture_cube_source &&pSource);

protected:
  virtual void init() override;
  virtual int type() override;

private:
  texture_cube_source mSource{};
};

class texture_buffer : public texture {
public:
  texture_buffer(const std::shared_ptr<gl_texture_buffer> &pBuffer,
                 int pInternalFormat);

  virtual ~texture_buffer();

protected:
  virtual void init() override;
  virtual int type() override;

private:
  std::shared_ptr<gl_texture_buffer> mBuffer;
  int mInternalFormat;
};
} // namespace platformer

#endif
