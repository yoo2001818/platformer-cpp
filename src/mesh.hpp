#ifndef __MESH_HPP__
#define __MESH_HPP__
#include <any>
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <utility>
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
};

class shader {
public:
  shader();
  shader(const std::string &pVertex, const std::string &pFragment);
  shader(const shader &pValue);
  shader(shader &&pValue);

  shader &operator=(const shader &pValue);
  shader &operator=(shader &&pValue);

  ~shader();

  const std::string &vertex() const;
  void vertex(const std::string &pCode);
  void vertex(std::string &&pCode);
  const std::string &fragment() const;
  void fragment(const std::string &pCode);
  void fragment(std::string &&pCode);

  void set(const std::string &pName, float pValue);
  void set(const std::string &pName, const glm::vec2 &pValue);
  void set(const std::string &pName, const glm::vec3 &pValue);
  void set(const std::string &pName, const glm::vec4 &pValue);
  void set(const std::string &pName, const glm::mat2 &pValue);
  void set(const std::string &pName, const glm::mat3 &pValue);
  void set(const std::string &pName, const glm::mat4 &pValue);

  void prepare();
  void dispose();

private:
  std::string mVertex;
  std::string mFragment;
  unsigned int mVertexId = -1;
  unsigned int mFragmentId = -1;
  unsigned int mProgramId = -1;
  bool mIsDirty = true;
};

class texture {
public:
  int placeholder;
};

class material {
public:
  material();

  void prepare();
  void dispose();

private:
  shader mShader;
};

class mesh {
public:
  using mesh_pair =
      std::pair<std::shared_ptr<material>, std::shared_ptr<geometry>>;
  mesh();
  mesh(std::vector<mesh_pair> &pMeshes);

  void render(entt::registry &pRegistry, const entt::entity &pEntity);

private:
  std::vector<mesh_pair> mMeshes;
};
};     // namespace platformer
#endif // __MESH_HPP__
