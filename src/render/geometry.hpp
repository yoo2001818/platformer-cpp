#ifndef __RENDER_GEOMETRY_HPP__
#define __RENDER_GEOMETRY_HPP__
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

  const std::vector<unsigned int> &indices() const;
  void indices(const std::vector<unsigned int> &pValue);
  void indices(std::vector<unsigned int> &&pValue);

  int size();

  // NOTE: Shader program must be prepared first
  void prepare(shader &pShader);
  void render();
  void dispose();

  static void calc_normals(geometry &pGeometry);
  static void calc_tangents(geometry &pGeometry);
  static geometry load_obj(std::string pFilename);
  static geometry make_box();

private:
  unsigned int mVbo = -1;
  unsigned int mEbo = -1;
  unsigned int mVao = -1;
  std::vector<glm::vec3> mPositions{};
  std::vector<glm::vec2> mTexCoords{};
  std::vector<glm::vec3> mNormals{};
  std::vector<glm::vec4> mTangents{};
  std::vector<unsigned int> mIndices{};
  bool mIsDirty = true;

  void setAttribute(shader &pShader, const std::string &pName, int pSize,
                    int pType, bool pNormalized, int pStride, size_t pPointer);
};
} // namespace platformer
#endif
