#include "mesh.hpp"
#include "entt/entity/fwd.hpp"
#include <GL/glew.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
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
    return this->mPositions.size();
  } else {
    return this->mIndices.size();
  }
}

void geometry::prepare(shader &pShader) {
  if (this->mVbo == -1) {
    glGenBuffers(1, &(this->mVbo));
  }
  if (this->mEbo == -1) {
    glGenBuffers(1, &(this->mEbo));
  }
  if (this->mVao == -1) {
    // FIXME: VAO is unique to each geometry/shader pair, so it shouldn't be
    // managed in here...
    glGenVertexArrays(1, &(this->mVao));
  }
  if (this->mIsDirty) {
    {
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
        byteSize += sizeof(glm::vec4) * size;
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
        glBufferSubData(GL_ARRAY_BUFFER, pos, sizeof(glm::vec4) * size,
                        mTangents.data());
        pos += sizeof(glm::vec4) * size;
      }
    }
    if (!this->mIndices.empty()) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mEbo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   sizeof(unsigned int) * this->mIndices.size(),
                   this->mIndices.data(), GL_STATIC_DRAW);
    }
    glBindVertexArray(this->mVao);
    {
      auto pos = 0;
      auto size = this->mPositions.size();
      this->setAttribute(pShader, "aPosition", 3, GL_FLOAT, GL_FALSE,
                         sizeof(glm::vec3), pos);
      pos += sizeof(glm::vec3) * size;
      if (!this->mTexCoords.empty()) {
        this->setAttribute(pShader, "aTexCoord", 2, GL_FLOAT, GL_FALSE,
                           sizeof(glm::vec2), pos);
        pos += sizeof(glm::vec2) * size;
      }
      if (!this->mNormals.empty()) {
        this->setAttribute(pShader, "aNormal", 3, GL_FLOAT, GL_FALSE,
                           sizeof(glm::vec3), pos);
        pos += sizeof(glm::vec3) * size;
      }
      if (!this->mTangents.empty()) {
        this->setAttribute(pShader, "aTangent", 4, GL_FLOAT, GL_FALSE,
                           sizeof(glm::vec4), pos);
        pos += sizeof(glm::vec4) * size;
      }
    }
    this->mIsDirty = false;
  } else {
    glBindVertexArray(this->mVao);
    if (!this->mIndices.empty()) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->mEbo);
    }
  }
}

void geometry::setAttribute(shader &pShader, const std::string &pName,
                            int pSize, int pType, bool pNormalized, int pStride,
                            size_t pPointer) {
  auto index = glGetAttribLocation(pShader.mProgramId, pName.c_str());
  if (index == -1)
    return;
  glVertexAttribPointer(index, pSize, pType, pNormalized, pStride,
                        (void *)pPointer);
  glEnableVertexAttribArray(index);
}

void geometry::render() {
  if (this->mIndices.empty()) {
    glDrawArrays(GL_TRIANGLES, 0, this->mPositions.size());
  } else {
    glDrawElements(GL_TRIANGLES, this->mIndices.size(), GL_UNSIGNED_INT, 0);
  }
}

void geometry::dispose() {
  if (this->mVao != -1) {
    glDeleteVertexArrays(1, &(this->mVao));
    this->mVao = -1;
  }
  if (this->mVbo != -1) {
    glDeleteBuffers(1, &(this->mVbo));
    this->mVbo = -1;
  }
  if (this->mEbo != -1) {
    glDeleteBuffers(1, &(this->mEbo));
    this->mEbo = -1;
  }
  this->mIsDirty = true;
}

void geometry::calc_normals(geometry &pGeometry) {
  auto &normals = pGeometry.mNormals;
  auto &pos = pGeometry.mPositions;
  auto &indices = pGeometry.mIndices;
  normals.clear();
  normals.resize(pos.size());
  for (int vertId = 0; vertId < normals.size(); vertId += 1) {
    normals[vertId] = glm::vec3(.0f);
  }
  for (int faceId = 0; faceId < indices.size(); faceId += 3) {
    auto v1 = indices[faceId];
    auto v2 = indices[faceId + 1];
    auto v3 = indices[faceId + 2];
    auto &origin = pos[v1];
    auto p1 = pos[v2] - origin;
    auto p2 = pos[v3] - origin;
    glm::vec3 uv = glm::cross(p1, p2);
    normals[v1] += uv;
    normals[v2] += uv;
    normals[v3] += uv;
  }
  // Normalize vertices
  for (int vertId = 0; vertId < normals.size(); vertId += 1) {
    normals[vertId] = glm::normalize(normals[vertId]);
  }
}

void geometry::calc_tangents(geometry &pGeometry) {
  auto &pos = pGeometry.mPositions;
  auto &texCoords = pGeometry.mTexCoords;
  auto &tangents = pGeometry.mTangents;
  auto &indices = pGeometry.mIndices;
  tangents.clear();
  tangents.resize(pos.size());
  for (int faceId = 0; faceId < indices.size(); faceId += 3) {
    auto v1 = indices[faceId];
    auto v2 = indices[faceId + 1];
    auto v3 = indices[faceId + 2];
    auto &origin = pos[v1];
    auto p1 = pos[v2] - origin;
    auto p2 = pos[v3] - origin;
    auto &texOrigin = texCoords[v1];
    auto texP1 = texCoords[v2] - texOrigin;
    auto texP2 = texCoords[v3] - texOrigin;
    auto f = 1.0f / (texP1.x * texP2.y - texP2.x * texP1.y);
    auto &tangent = tangents[v1];
    tangent.x = f * (texP2.y * p1.x - texP1.y * p2.x);
    tangent.y = f * (texP2.y * p1.y - texP1.y * p2.y);
    tangent.z = f * (texP2.y * p1.z - texP1.y * p2.z);
    // Calculate bi-tangent. To save vertex array, it can be calculated in
    // vertex shader; however we have to specify the cross order to get right
    // result. This can be done by using a modifier... I think.
    // To calculate modifier, we have to calculate dot product with
    // bi-tangent from vertex shader and bi-tangent we calculated.
    auto normal = glm::cross(p1, p2);
    normal = glm::normalize(normal);
    auto leftBitangent = glm::cross(glm::vec3(tangent), normal);
    glm::vec3 bitangent;
    bitangent.x = f * (texP2.x * p1.x - texP1.x * p2.x);
    bitangent.y = f * (texP2.x * p1.y - texP1.x * p2.y);
    bitangent.z = f * (texP2.x * p1.z - texP1.x * p2.z);
    auto modifier = glm::dot(bitangent, leftBitangent);
    tangent.w = modifier < 0.0f ? -1.0f : 1.0f;
    tangents[v2] = tangent;
    tangents[v3] = tangent;
  }
}

std::vector<std::string> string_split(std::string s, std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

geometry geometry::load_obj(std::string pFilename) {
  std::ifstream infile(pFilename);
  std::string line;
  std::vector<glm::vec3> vertPos;
  std::vector<glm::vec3> vertNormals;
  std::vector<glm::vec2> vertTexCoords;
  std::vector<glm::vec3> facePos;
  std::vector<glm::vec3> faceNormals;
  std::vector<glm::vec2> faceTexCoords;
  std::vector<unsigned int> faces;
  bool hasNormals = false;
  while (std::getline(infile, line)) {
    // Process each line
    if (line[0] == '#') {
      continue;
    }
    std::vector<std::string> words = string_split(line, " ");
    // o -> Object
    // v -> aPosition
    // vt -> aTexCoord
    // vn -> aNormal
    // vp -> Not sure
    // f -> Indices
    // mtllib
    // usemtl
    // s off -> Normal smoothing
    if (words[0] == "v") {
      // Vertex coords: v 0 0 0
      vertPos.push_back(
          {std::stof(words[1]), std::stof(words[2]), std::stof((words[3]))});
    } else if (words[0] == "vn") {
      // Normals: vn 0 0 0
      vertNormals.push_back(
          {std::stof(words[1]), std::stof(words[2]), std::stof((words[3]))});
    } else if (words[0] == "vt") {
      // TexCoords: vt 0 0
      vertTexCoords.push_back({std::stof(words[1]), std::stof(words[2])});
    } else if (words[0] == "p") {
      // Point
    } else if (words[0] == "l") {
      // Line
    } else if (words[0] == "f") {
      // Face: f 0/0/0 0/0/0 0/0/0
      // Arbitrary amount of points are possible; we must triangluate them
      int startIndex = facePos.size();
      for (int i = 1; i < words.size(); i += 1) {
        std::vector<std::string> segments = string_split(words[i], "/");
        facePos.push_back(vertPos[std::stoi(segments[0]) - 1]);
        if (segments.size() > 1 && segments[1].size() > 0) {
          faceTexCoords.push_back(vertTexCoords[std::stoi(segments[1]) - 1]);
        } else {
          faceTexCoords.push_back({});
        }
        if (segments.size() > 2 && segments[2].size() > 0) {
          hasNormals = true;
          faceNormals.push_back(vertNormals[std::stoi(segments[2]) - 1]);
        } else {
          faceNormals.push_back({});
        }
      }
      for (int i = 3; i <= words.size() - 1; i += 1) {
        faces.push_back(startIndex);
        faces.push_back(startIndex + i - 2);
        faces.push_back(startIndex + i - 1);
      }
    } else if (words[0] == "o") {
      // Finalize object if exists; otherwise specify its name
    } else if (words[0] == "usemtl") {
      // mtl file directory path; but we won't do anything for now
    } else if (words[0] == "g") {
      // Put the object in group if exists
    } else if (words[0] == "s") {
      // Smoothing: s off / s 0 / s on / s 1
    }
  }
  geometry geom;
  geom.mIndices = faces;
  geom.mPositions = facePos;
  geom.mNormals = faceNormals;
  geom.mTexCoords = faceTexCoords;
  if (!hasNormals)
    calc_normals(geom);
  calc_tangents(geom);
  return geom;
}

geometry geometry::make_box() {
  geometry geom;
  geom.positions({
      // Front
      {-1.0f, -1.0f, 1.0f},
      {1.0f, -1.0f, 1.0f},
      {1.0f, 1.0f, 1.0f},
      {-1.0f, 1.0f, 1.0f},
      // Top
      {-1.0f, 1.0f, 1.0f},
      {1.0f, 1.0f, 1.0f},
      {1.0f, 1.0f, -1.0f},
      {-1.0f, 1.0f, -1.0f},
      // Back
      {1.0f, -1.0f, -1.0f},
      {-1.0f, -1.0f, -1.0f},
      {-1.0f, 1.0f, -1.0f},
      {1.0f, 1.0f, -1.0f},
      // Bottom
      {-1.0f, -1.0f, -1.0f},
      {1.0f, -1.0f, -1.0f},
      {1.0f, -1.0f, 1.0f},
      {-1.0f, -1.0f, 1.0f},
      // Left
      {-1.0f, -1.0f, -1.0f},
      {-1.0f, -1.0f, 1.0f},
      {-1.0f, 1.0f, 1.0f},
      {-1.0f, 1.0f, -1.0f},
      // Right
      {1.0f, -1.0f, 1.0f},
      {1.0f, -1.0f, -1.0f},
      {1.0f, 1.0f, -1.0f},
      {1.0f, 1.0f, 1.0f},
  });
  geom.texCoords({
      {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},
      {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},
      {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
      {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
      {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
  });
  geom.normals({
      // Front
      {0.0f, 0.0f, 1.0f},
      {0.0f, 0.0f, 1.0f},
      {0.0f, 0.0f, 1.0f},
      {0.0f, 0.0f, 1.0f},
      // Top
      {0.0f, 1.0f, 0.0f},
      {0.0f, 1.0f, 0.0f},
      {0.0f, 1.0f, 0.0f},
      {0.0f, 1.0f, 0.0f},
      // Back
      {0.0f, 0.0f, -1.0f},
      {0.0f, 0.0f, -1.0f},
      {0.0f, 0.0f, -1.0f},
      {0.0f, 0.0f, -1.0f},
      // Bottom
      {0.0f, -1.0f, 0.0f},
      {0.0f, -1.0f, 0.0f},
      {0.0f, -1.0f, 0.0f},
      {0.0f, -1.0f, 0.0f},
      // Left
      {-1.0f, 0.0f, 0.0f},
      {-1.0f, 0.0f, 0.0f},
      {-1.0f, 0.0f, 0.0f},
      {-1.0f, 0.0f, 0.0f},
      // Right
      {1.0f, 0.0f, 0.0f},
      {1.0f, 0.0f, 0.0f},
      {1.0f, 0.0f, 0.0f},
      {1.0f, 0.0f, 0.0f},
  });
  geom.indices({0,  1,  2,  2,  3,  0,  4,  5,  6,  6,  7,  4,
                8,  9,  10, 10, 11, 8,  12, 13, 14, 14, 15, 12,
                16, 17, 18, 18, 19, 16, 20, 21, 22, 22, 23, 20});
  return geom;
}

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

void shader::set(const std::string &pName, float pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniform1f(pos, pValue);
}
void shader::set(const std::string &pName, const glm::vec2 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniform2fv(pos, 1, glm::value_ptr(pValue));
}
void shader::set(const std::string &pName, const glm::vec3 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniform3fv(pos, 1, glm::value_ptr(pValue));
}
void shader::set(const std::string &pName, const glm::vec4 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniform4fv(pos, 1, glm::value_ptr(pValue));
}
void shader::set(const std::string &pName, const glm::mat2 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniformMatrix2fv(pos, 1, false, glm::value_ptr(pValue));
}
void shader::set(const std::string &pName, const glm::mat3 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniformMatrix3fv(pos, 1, false, glm::value_ptr(pValue));
}
void shader::set(const std::string &pName, const glm::mat4 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniformMatrix4fv(pos, 1, false, glm::value_ptr(pValue));
}

void shader::prepare() {
  if (this->mIsDirty) {
    this->dispose();
    int success;
    char infoLog[512];
    auto vsSource = this->mVertex.data();
    auto vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSource, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(vs, 512, NULL, infoLog);
      throw std::runtime_error(infoLog);
    };

    auto fsSource = this->mFragment.data();
    auto fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSource, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(fs, 512, NULL, infoLog);
      throw std::runtime_error(infoLog);
    };

    this->mProgramId = glCreateProgram();
    glAttachShader(this->mProgramId, vs);
    glAttachShader(this->mProgramId, fs);
    glLinkProgram(this->mProgramId);
    glGetProgramiv(this->mProgramId, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(this->mProgramId, 512, NULL, infoLog);
      throw std::runtime_error(infoLog);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    this->mIsDirty = false;
  }
  glUseProgram(this->mProgramId);
}

void shader::dispose() {
  if (this->mProgramId != -1) {
    glDeleteProgram(this->mProgramId);
    this->mProgramId = -1;
  }
}

material::material() {}

void material::prepare() {}

void material::dispose() {}

mesh::mesh() {}

mesh::mesh(std::vector<mesh::mesh_pair> &pMeshes) {}

void mesh::render(entt::registry &pRegistry, const entt::entity &pEntity) {}
