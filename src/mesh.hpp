#ifndef __MESH_HPP__
#define __MESH_HPP__
#include "entt/entity/fwd.hpp"
#include "transform.hpp"
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

  void setAttribute(shader &pShader, const std::string &pName, int pSize,
                    int pType, bool pNormalized, int pStride, size_t pPointer);
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
  unsigned int mProgramId = -1;
  bool mIsDirty = true;
  friend geometry;
};

class texture {
public:
  int placeholder;
};

class mesh;

class camera {
public:
  // FIXME: This is against the convention; it could and should be improved
  // in the future
  enum { PERSPECTIVE, ORTHOGRAPHIC } type;
  float near;
  float far;
  float fov;

  glm::mat4 getProjection(float pAspect);
};

class light {
public:
  light();
  glm::vec3 color;
  float power;
  float radius;
  float range;
};

struct render_light {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec3 range;
};

struct render_context {
  // This contains all the information needed to render the object -
  // For example, the entity, the framebuffer to render to, the list of lights,
  // and so more.
  // Material will read this data and set it as uniforms. (Consider using UBOs,
  // or even better, instancing)
  // The current structure necessitiates for material to be aware of armatures,
  // but that is simply unavoidable at this point.
  entt::registry &registry;
  float aspect;

  const entt::entity &entity;
  platformer::transform &transform;
  platformer::mesh &mesh;
  platformer::geometry &geometry;

  const entt::entity &camera_entity;
  platformer::transform &camera_transform;
  platformer::camera &camera_camera;

  std::vector<render_light> &lights;
};

struct render_context_root {
  // Root-level render context.
  entt::registry &registry;
  float aspect;

  const entt::entity &camera_entity;
  platformer::transform &camera_transform;
  platformer::camera &camera_camera;

  std::vector<render_light> &lights;
};

class material {
public:
  material();
  virtual ~material();

  virtual void render(const render_context &pContext) = 0;
  virtual void dispose() = 0;
};

class shader_material : public material {
public:
  shader_material(std::string pVertex, std::string pFragment);

  virtual void render(const render_context &pContext) override;
  virtual void dispose() override;

private:
  shader mShader;
};

class mesh {
public:
  using mesh_pair =
      std::pair<std::shared_ptr<material>, std::shared_ptr<geometry>>;
  mesh();
  mesh(const std::vector<mesh_pair> &pMeshes);
  mesh(std::vector<mesh_pair> &&pMeshes);

  std::vector<mesh_pair> &meshes();

  void render(entt::registry &pRegistry, const entt::entity &pEntity,
              render_context_root &pRenderContextRoot);

private:
  std::vector<mesh_pair> mMeshes;
};

};     // namespace platformer
#endif // __MESH_HPP__
