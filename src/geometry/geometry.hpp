#ifndef __RENDER_GEOMETRY_HPP__
#define __RENDER_GEOMETRY_HPP__
#include "render/buffer.hpp"
#include <GL/glew.h>
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>
namespace platformer {
// Ideally, geometry and material should be platform-agonistic - that is, it
// should not store internal handles and data inside it, but a separate renderer
// should handle it.
// Since that is only a luxury at this point, and there's no real benefit of
// doing it so prematurely, it'd be better to stick with having some internal
// data inside it.
class shader;

class geometry {
public:
  geometry();
  geometry(const geometry &pValue);
  geometry(geometry &&pValue);

  geometry &operator=(const geometry &pValue);
  geometry &operator=(geometry &&pValue);

  ~geometry();

  // This is very cumbersome; it'd be great if it can use arbitrary names
  const std::vector<glm::vec3> &positions() const;
  void positions(const std::vector<glm::vec3> &pValue);
  void positions(std::vector<glm::vec3> &&pValue);

  const std::vector<glm::vec2> &texCoords() const;
  void texCoords(const std::vector<glm::vec2> &pValue);
  void texCoords(std::vector<glm::vec2> &&pValue);

  const std::vector<glm::vec3> &normals() const;
  void normals(const std::vector<glm::vec3> &pValue);
  void normals(std::vector<glm::vec3> &&pValue);

  const std::vector<glm::vec4> &tangents() const;
  void tangents(const std::vector<glm::vec4> &pValue);
  void tangents(std::vector<glm::vec4> &&pValue);

  const std::vector<glm::vec2> &texCoords2() const;
  void texCoords2(const std::vector<glm::vec2> &pValue);
  void texCoords2(std::vector<glm::vec2> &&pValue);

  const std::vector<glm::vec4> &colors() const;
  void colors(const std::vector<glm::vec4> &pValue);
  void colors(std::vector<glm::vec4> &&pValue);

  const std::vector<glm::ivec4> &boneIds() const;
  void boneIds(const std::vector<glm::ivec4> &pValue);
  void boneIds(std::vector<glm::ivec4> &&pValue);

  const std::vector<glm::vec4> &boneWeights() const;
  void boneWeights(const std::vector<glm::vec4> &pValue);
  void boneWeights(std::vector<glm::vec4> &&pValue);

  const std::vector<unsigned int> &indices() const;
  void indices(const std::vector<unsigned int> &pValue);
  void indices(std::vector<unsigned int> &&pValue);

  int size();

  // NOTE: Shader program must be prepared first
  void prepare(shader &pShader);
  void render();
  void render(int pPrimCount);
  void dispose();

  static void calc_normals(geometry &pGeometry);
  static void calc_tangents(geometry &pGeometry);
  static geometry load_obj(std::string pFilename);
  static geometry make_box();
  static geometry make_quad(int pHSlice, int pVSlice);
  static geometry make_quad();
  static geometry make_uv_sphere(int pSegments, int pRings);

private:
  gl_array_buffer mVbo{GL_STATIC_DRAW};
  gl_element_array_buffer mEbo{GL_STATIC_DRAW};
  unsigned int mVao = -1;
  std::vector<glm::vec3> mPositions{};
  std::vector<glm::vec2> mTexCoords{};
  std::vector<glm::vec3> mNormals{};
  std::vector<glm::vec4> mTangents{};
  std::vector<glm::vec2> mTexCoords2{};
  std::vector<glm::vec4> mColors{};
  std::vector<glm::ivec4> mBoneIds{};
  std::vector<glm::vec4> mBoneWeights{};
  std::vector<unsigned int> mIndices{};
  bool mIsDirty = true;

  void setAttribute(shader &pShader, const std::string &pName, int pSize,
                    int pType, bool pNormalized, int pStride, size_t pPointer);
};
} // namespace platformer
#endif
