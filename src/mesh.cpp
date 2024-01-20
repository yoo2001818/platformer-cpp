#include "mesh.hpp"
#include "entt/entity/fwd.hpp"
#include <utility>
#include <vector>

using namespace platformer;

geometry::geometry() {}

geometry::geometry(const geometry &pValue)
    : mPositions(pValue.mPositions), mTexCoords(pValue.mTexCoords),
      mNormals(pValue.mNormals), mTangents(pValue.mTangents),
      mIndices(pValue.mIndices) {}

geometry::geometry(geometry &&pValue)
    : mPositions(std::move(pValue.mPositions)),
      mTexCoords(std::move(pValue.mTexCoords)),
      mNormals(std::move(pValue.mNormals)),
      mTangents(std::move(pValue.mTangents)),
      mIndices(std::move(pValue.mIndices)) {}

geometry &geometry::operator=(const geometry &pValue) {
  this->mPositions = pValue.mPositions;
  this->mTexCoords = pValue.mTexCoords;
  this->mNormals = pValue.mNormals;
  this->mTangents = pValue.mTangents;
  this->mIndices = pValue.mIndices;
  this->mIsDirty = true;
  return *this;
}

geometry &geometry::operator=(geometry &&pValue) {
  this->mPositions = std::move(pValue.mPositions);
  this->mTexCoords = std::move(pValue.mTexCoords);
  this->mNormals = std::move(pValue.mNormals);
  this->mTangents = std::move(pValue.mTangents);
  this->mIndices = std::move(pValue.mIndices);
  this->mIsDirty = true;
  return *this;
}

geometry::~geometry() {
  // FIXME: Delete buffers and remove from it
}

const std::vector<glm::vec3> &geometry::positions() const {
  return this->mPositions;
}

void geometry::positions(const std::vector<glm::vec3> &pValue) {
  this->mPositions = pValue;
}

void geometry::positions(std::vector<glm::vec3> &&pValue) {
  this->mPositions = std::move(pValue);
}

const std::vector<glm::vec2> &geometry::texCoords() const {
  return this->mTexCoords;
}

void geometry::texCoords(const std::vector<glm::vec2> &pValue) {
  this->mTexCoords = pValue;
}

void geometry::texCoords(std::vector<glm::vec2> &&pValue) {
  this->mTexCoords = std::move(pValue);
}

const std::vector<glm::vec3> &geometry::normals() const {
  return this->mNormals;
}

void geometry::normals(const std::vector<glm::vec3> &pValue) {
  this->mNormals = pValue;
}

void geometry::normals(std::vector<glm::vec3> &&pValue) {
  this->mNormals = std::move(pValue);
}

const std::vector<glm::vec4> &geometry::tangents() const {
  return this->mTangents;
}

void geometry::tangents(const std::vector<glm::vec4> &pValue) {
  this->mTangents = pValue;
}

void geometry::tangents(std::vector<glm::vec4> &&pValue) {
  this->mTangents = std::move(pValue);
}

const std::vector<unsigned int> &geometry::indices() const {
  return this->mIndices;
}

void geometry::indices(const std::vector<unsigned int> &pValue) {
  this->mIndices = pValue;
}

void geometry::indices(std::vector<unsigned int> &&pValue) {
  this->mIndices = std::move(pValue);
}

int geometry::size() {}

void geometry::render() {}

void geometry::dispose() {}

void geometry::calc_normals(geometry &pGeometry) {}

void geometry::calc_tangents(geometry &pGeometry) {}

geometry geometry::load_obj(std::string pFilename) {}

geometry geometry::make_box() {}

shader::shader() {}

shader::shader(const std::string &pVertex, const std::string &pFragment) {}

shader::shader(const shader &pValue) {}

shader::shader(shader &&pValue) {}

shader &shader::operator=(const shader &pValue) {}

shader &shader::operator=(shader &&pValue) {}

const std::string &shader::vertex() const { return this->mVertex; }

void shader::vertex(const std::string &pCode) {
  this->mVertex = pCode;
  this->mIsDirty = true;
}

void shader::vertex(std::string &&pCode) {
  this->mVertex = std::move(pCode);
  this->mIsDirty = true;
}

const std::string &shader::fragment() const { return this->mFragment; }

void shader::fragment(const std::string &pCode) {
  this->mFragment = pCode;
  this->mIsDirty = true;
}

void shader::fragment(std::string &&pCode) {
  this->mFragment = std::move(pCode);
  this->mIsDirty = true;
}

void shader::set(const std::string &pName, float pValue) {}
void shader::set(const std::string &pName, const glm::vec2 &pValue) {}
void shader::set(const std::string &pName, const glm::vec3 &pValue) {}
void shader::set(const std::string &pName, const glm::vec4 &pValue) {}
void shader::set(const std::string &pName, const glm::mat2 &pValue) {}
void shader::set(const std::string &pName, const glm::mat3 &pValue) {}
void shader::set(const std::string &pName, const glm::mat4 &pValue) {}

void shader::prepare() {}

void shader::dispose() {}

material::material() {}

void material::prepare() {}

void material::dispose() {}

mesh::mesh() {}

mesh::mesh(std::vector<mesh::mesh_pair> &pMeshes) {}

void mesh::render(entt::registry &pRegistry, const entt::entity &pEntity) {}
