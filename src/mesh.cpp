#include "mesh.hpp"
#include "entt/entity/fwd.hpp"
#include <GL/glew.h>
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
      mIndices(std::move(pValue.mIndices)) {
  // TODO: Possibly, move the internal state too
}

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
  // TODO: Possibly, move the internal state too
  return *this;
}

geometry::~geometry() { this->dispose(); }

const std::vector<glm::vec3> &geometry::positions() const {
  return this->mPositions;
}

void geometry::positions(const std::vector<glm::vec3> &pValue) {
  this->mPositions = pValue;
  this->mIsDirty = true;
}

void geometry::positions(std::vector<glm::vec3> &&pValue) {
  this->mPositions = std::move(pValue);
  this->mIsDirty = true;
}

const std::vector<glm::vec2> &geometry::texCoords() const {
  return this->mTexCoords;
}

void geometry::texCoords(const std::vector<glm::vec2> &pValue) {
  this->mTexCoords = pValue;
  this->mIsDirty = true;
}

void geometry::texCoords(std::vector<glm::vec2> &&pValue) {
  this->mTexCoords = std::move(pValue);
  this->mIsDirty = true;
}

const std::vector<glm::vec3> &geometry::normals() const {
  return this->mNormals;
}

void geometry::normals(const std::vector<glm::vec3> &pValue) {
  this->mNormals = pValue;
  this->mIsDirty = true;
}

void geometry::normals(std::vector<glm::vec3> &&pValue) {
  this->mNormals = std::move(pValue);
  this->mIsDirty = true;
}

const std::vector<glm::vec4> &geometry::tangents() const {
  return this->mTangents;
}

void geometry::tangents(const std::vector<glm::vec4> &pValue) {
  this->mTangents = pValue;
  this->mIsDirty = true;
}

void geometry::tangents(std::vector<glm::vec4> &&pValue) {
  this->mTangents = std::move(pValue);
  this->mIsDirty = true;
}

const std::vector<unsigned int> &geometry::indices() const {
  return this->mIndices;
}

void geometry::indices(const std::vector<unsigned int> &pValue) {
  this->mIndices = pValue;
  this->mIsDirty = true;
}

void geometry::indices(std::vector<unsigned int> &&pValue) {
  this->mIndices = std::move(pValue);
  this->mIsDirty = true;
}

int geometry::size() {
  // Instead of using std::optional, let's just use empty vector as empty
  if (this->mIndices.empty()) {
    return this->mPositions.size() / 3;
  } else {
    return this->mIndices.size() / 3;
  }
}

void geometry::prepare(shader &pShader) {
  if (this->mVbo == -1) {
    glGenBuffers(1, &(this->mVbo));
    glBindBuffer(GL_ARRAY_BUFFER, this->mVbo);
    // Estimate total size of attributes
    auto size = this->mPositions.size();
    auto byteSize = sizeof(glm::vec3) * size;
    if (!this->mTexCoords.empty()) {
      byteSize += sizeof(glm::vec2) * size;
    }
    if (!this->mNormals.empty()) {
      byteSize += sizeof(glm::vec3) * size;
    }
    if (!this->mTangents.empty()) {
      byteSize += sizeof(glm::vec3) * size;
    }
    // Upload each buffer
    glBufferData(GL_ARRAY_BUFFER, byteSize, nullptr, GL_STATIC_DRAW);
    auto pos = 0;
    glBufferSubData(GL_ARRAY_BUFFER, pos, sizeof(glm::vec3) * size,
                    mPositions.data());
    pos += sizeof(glm::vec3) * size;
    if (!this->mTexCoords.empty()) {
      glBufferSubData(GL_ARRAY_BUFFER, pos, sizeof(glm::vec2) * size,
                      mTexCoords.data());
      pos += sizeof(glm::vec2) * size;
    }
    if (!this->mNormals.empty()) {
      glBufferSubData(GL_ARRAY_BUFFER, pos, sizeof(glm::vec3) * size,
                      mNormals.data());
      pos += sizeof(glm::vec3) * size;
    }
    if (!this->mTangents.empty()) {
      glBufferSubData(GL_ARRAY_BUFFER, pos, sizeof(glm::vec3) * size,
                      mTangents.data());
      pos += sizeof(glm::vec3) * size;
    }
  }
  if (this->mEbo == -1 && !this->mIndices.empty()) {
    glGenBuffers(1, &(this->mEbo));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mEbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * this->mIndices.size(),
                 this->mIndices.data(), GL_STATIC_DRAW);
  }
  if (this->mVao == -1) {
    // FIXME: VAO is unique to each geometry/shader pair, so it shouldn't be
    // managed in here...
    glGenVertexArrays(1, &(this->mVao));
    glBindVertexArray(this->mVao);
    glBindBuffer(GL_ARRAY_BUFFER, this->mVbo);
    auto pos = 0;
    auto size = this->mPositions.size();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void *)0);
    glEnableVertexAttribArray(0);
    pos += sizeof(glm::vec3) * size;
    if (!this->mTexCoords.empty()) {
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2),
                            (void *)pos);
      glEnableVertexAttribArray(1);
      pos += sizeof(glm::vec2) * size;
    }
    if (!this->mNormals.empty()) {
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                            (void *)pos);
      glEnableVertexAttribArray(2);
      pos += sizeof(glm::vec3) * size;
    }
    if (!this->mTangents.empty()) {
      glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                            (void *)pos);
      glEnableVertexAttribArray(2);
      pos += sizeof(glm::vec3) * size;
    }
  }
}

void geometry::render() {}

void geometry::dispose() {}

void geometry::calc_normals(geometry &pGeometry) {}

void geometry::calc_tangents(geometry &pGeometry) {}

geometry geometry::load_obj(std::string pFilename) {}

geometry geometry::make_box() {}

shader::shader() {}

shader::shader(const std::string &pVertex, const std::string &pFragment)
    : mVertex(pVertex), mFragment(pFragment) {}

shader::shader(const shader &pValue)
    : mVertex(pValue.mVertex), mFragment(pValue.mFragment) {}

shader::shader(shader &&pValue)
    : mVertex(pValue.mVertex), mFragment(pValue.mFragment) {
  // TODO: Possibly, move the internal state too
}

shader &shader::operator=(const shader &pValue) {
  this->mVertex = pValue.mVertex;
  this->mFragment = pValue.mFragment;
  this->mIsDirty = true;
  return *this;
}

shader &shader::operator=(shader &&pValue) {
  this->mVertex = std::move(pValue.mVertex);
  this->mFragment = std::move(pValue.mFragment);
  this->mIsDirty = true;
  // TODO: Possibly, move the internal state too
  return *this;
}

shader::~shader() { this->dispose(); }

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
