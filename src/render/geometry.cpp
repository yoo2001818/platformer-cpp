#include <glm/gtc/constants.hpp>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include "debug.hpp"
#include "render/geometry.hpp"
#include "render/shader.hpp"
#include <GL/glew.h>
#include <fstream>

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

const std::vector<glm::vec2> &geometry::texCoords2() const {
  return this->mTexCoords2;
}

void geometry::texCoords2(const std::vector<glm::vec2> &pValue) {
  this->mTexCoords2 = pValue;
  this->mIsDirty = true;
}

void geometry::texCoords2(std::vector<glm::vec2> &&pValue) {
  this->mTexCoords2 = std::move(pValue);
  this->mIsDirty = true;
}

const std::vector<glm::vec4> &geometry::colors() const { return this->mColors; }

void geometry::colors(const std::vector<glm::vec4> &pValue) {
  this->mColors = pValue;
  this->mIsDirty = true;
}

void geometry::colors(std::vector<glm::vec4> &&pValue) {
  this->mColors = std::move(pValue);
  this->mIsDirty = true;
}

const std::vector<glm::ivec4> &geometry::boneIds() const {
  return this->mBoneIds;
}

void geometry::boneIds(const std::vector<glm::ivec4> &pValue) {
  this->mBoneIds = pValue;
  this->mIsDirty = true;
}

void geometry::boneIds(std::vector<glm::ivec4> &&pValue) {
  this->mBoneIds = std::move(pValue);
  this->mIsDirty = true;
}

const std::vector<glm::vec4> &geometry::boneWeights() const {
  return this->mBoneWeights;
}

void geometry::boneWeights(const std::vector<glm::vec4> &pValue) {
  this->mBoneWeights = pValue;
  this->mIsDirty = true;
}

void geometry::boneWeights(std::vector<glm::vec4> &&pValue) {
  this->mBoneWeights = std::move(pValue);
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
  if (this->mVao == -1) {
    // FIXME: VAO is unique to each geometry/shader pair, so it shouldn't be
    // managed in here...
    glGenVertexArrays(1, &(this->mVao));
  }
  if (this->mIsDirty) {
    glBindVertexArray(this->mVao);
    {
      this->mVbo.bind();
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
      if (!this->mTexCoords2.empty()) {
        byteSize += sizeof(glm::vec2) * size;
      }
      if (!this->mColors.empty()) {
        byteSize += sizeof(glm::vec4) * size;
      }
      if (!this->mBoneIds.empty()) {
        byteSize += sizeof(glm::ivec4) * size;
      }
      if (!this->mBoneWeights.empty()) {
        byteSize += sizeof(glm::vec4) * size;
      }
      // Upload each buffer
      this->mVbo.buffer_data(nullptr, byteSize);
      auto pos = 0;
      this->mVbo.set(mPositions, pos);
      pos += sizeof(glm::vec3) * size;
      if (!this->mTexCoords.empty()) {
        this->mVbo.set(mTexCoords, pos);
        pos += sizeof(glm::vec2) * size;
      }
      if (!this->mNormals.empty()) {
        this->mVbo.set(mNormals, pos);
        pos += sizeof(glm::vec3) * size;
      }
      if (!this->mTangents.empty()) {
        this->mVbo.set(mTangents, pos);
        pos += sizeof(glm::vec4) * size;
      }
      if (!this->mTexCoords2.empty()) {
        this->mVbo.set(mTexCoords2, pos);
        pos += sizeof(glm::vec2) * size;
      }
      if (!this->mColors.empty()) {
        this->mVbo.set(mColors, pos);
        pos += sizeof(glm::vec4) * size;
      }
      if (!this->mBoneIds.empty()) {
        this->mVbo.set(mBoneIds, pos);
        pos += sizeof(glm::ivec4) * size;
      }
      if (!this->mBoneWeights.empty()) {
        this->mVbo.set(mBoneWeights, pos);
        pos += sizeof(glm::vec4) * size;
      }
    }
    if (!this->mIndices.empty()) {
      this->mEbo.bind();
      this->mEbo.set(this->mIndices);
    }
    this->mIsDirty = false;
    DEBUG("Geometry uploaded ({} tris)", this->mIndices.size() / 3);
  }
  // TODO: VAO needs consistent layout, which we don't have right now
  // Hence we're resetting attributes every time
  {
    glBindVertexArray(this->mVao);
    this->mVbo.bind();
    auto pos = 0;
    auto size = this->mPositions.size();
    pShader.set_attribute("aPosition", 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          pos);
    pos += sizeof(glm::vec3) * size;
    if (!this->mTexCoords.empty()) {
      pShader.set_attribute("aTexCoord", 2, GL_FLOAT, GL_FALSE,
                            sizeof(glm::vec2), pos);
      pos += sizeof(glm::vec2) * size;
    }
    if (!this->mNormals.empty()) {
      pShader.set_attribute("aNormal", 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                            pos);
      pos += sizeof(glm::vec3) * size;
    }
    if (!this->mTangents.empty()) {
      pShader.set_attribute("aTangent", 4, GL_FLOAT, GL_FALSE,
                            sizeof(glm::vec4), pos);
      pos += sizeof(glm::vec4) * size;
    }
    if (!this->mTexCoords2.empty()) {
      pShader.set_attribute("aTexCoord2", 2, GL_FLOAT, GL_FALSE,
                            sizeof(glm::vec4), pos);
      pos += sizeof(glm::vec2) * size;
    }
    if (!this->mColors.empty()) {
      pShader.set_attribute("aColor", 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4),
                            pos);
      pos += sizeof(glm::vec4) * size;
    }
    if (!this->mBoneIds.empty()) {
      pShader.set_attribute("aBoneIds", 4, GL_INT, GL_FALSE, sizeof(glm::ivec4),
                            pos);
      pos += sizeof(glm::ivec4) * size;
    }
    if (!this->mBoneWeights.empty()) {
      pShader.set_attribute("aBoneWeights", 4, GL_FLOAT, GL_FALSE,
                            sizeof(glm::vec4), pos);
      pos += sizeof(glm::vec4) * size;
    }
  }
  if (!this->mIndices.empty()) {
    this->mEbo.bind();
  }
}

void geometry::render() {
  if (this->mIndices.empty()) {
    glDrawArrays(GL_TRIANGLES, 0, this->mPositions.size());
  } else {
    glDrawElements(GL_TRIANGLES, this->mIndices.size(), GL_UNSIGNED_INT, 0);
  }
}

void geometry::render(int pPrimCount) {
  if (this->mIndices.empty()) {
    glDrawArraysInstanced(GL_TRIANGLES, 0, this->mPositions.size(), pPrimCount);
  } else {
    glDrawElementsInstanced(GL_TRIANGLES, this->mIndices.size(),
                            GL_UNSIGNED_INT, 0, pPrimCount);
  }
}

void geometry::dispose() {
  if (this->mVao != -1) {
    glDeleteVertexArrays(1, &(this->mVao));
    this->mVao = -1;
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

geometry geometry::make_quad(int pHSlice, int pVSlice) {
  geometry geom;
  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> texCoords;
  std::vector<unsigned int> indices;
  // Create vertices
  for (int y = 0; y <= pVSlice; y += 1) {
    int yPos = y / pVSlice * 2 - 1;
    for (int x = 0; x <= pHSlice; x += 1) {
      float xPos = (float)x / pHSlice * 2 - 1;
      positions.push_back({xPos, yPos, 0});
      texCoords.push_back({(xPos + 1) / 2.0f, (yPos + 1) / 2.0f});
    }
  }
  // Create indices
  for (int y = 0; y < pVSlice; y += 1) {
    for (int x = 0; x < pHSlice; x += 1) {
      // Vertex indices
      unsigned int tl = y * (pHSlice + 1) + x;
      unsigned int tr = y * (pHSlice + 1) + x + 1;
      unsigned int bl = (y + 1) * (pHSlice + 1) + x;
      unsigned int br = (y + 1) * (pHSlice + 1) + x + 1;
      // Actual index position
      unsigned int pos = (y * pHSlice + x) * 6;
      indices.push_back(tl);
      indices.push_back(tr);
      indices.push_back(br);
      indices.push_back(br);
      indices.push_back(bl);
      indices.push_back(tl);
    }
  }
  geom.positions(positions);
  geom.texCoords(texCoords);
  geom.indices(indices);
  return geom;
}

geometry geometry::make_quad() { return geometry::make_quad(1, 1); }

geometry geometry::make_uv_sphere(int pSegments, int pRings) {
  // A UV Sphere consists of many regular N-polygons, which serves as
  // a 'ring'. Its radius gracefully goes small, and eventually reach 0
  // at the top. A sphere is defined as x^2 + y^2 + z^2 = 1. Which converts
  // to 1 - z^2 = x^2 + y^2. We can derive that a ring's radius is
  // sqrt(1 - z^2) from that equation.
  // The sphere has ring * (segment - 2) + 2 vertices. Since radius 0 means
  // a single point, we can remove 2 rings from top and bottom.
  // However, OpenGL requires another vertices for different normals, etc,
  // we must use different vertices for each faces.
  // A UV sphere can be seperated to 3 parts - Roof, Pillar, Floor.
  // Roof consists of N triangles (N = segments), so it'd be trivial -
  // it needs 3 * N vertices.
  // Pillar, however, uses quads. However, we can share the vertices if
  // it has same normal map, texture coords, etc.
  // Pillar consists of N * (M - 3) quads (M = rings), so it uses
  // 4 * N * (M - 3) vertices.
  // Floor is exactly same as the roof, it needs 3 * N vertices.
  // Adding all, it needs N * (6 + 4 * (M - 3)) vertices total, and
  // N * (2 + 2 * (M - 3)) triangles total.
  geometry geom;
  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> texCoords;
  std::vector<unsigned int> indices;

  unsigned int verticesTotal = pSegments * (6 + 4 * (pRings - 3));
  unsigned int facesTotal = pSegments * (2 + 2 * (pRings - 3));

  positions.reserve(verticesTotal);
  texCoords.reserve(verticesTotal);
  indices.reserve(facesTotal * 3);

  // Roof
  for (unsigned int i = 0; i < pSegments; i++) {
    float theta = 1.0f / (pRings - 1) * glm::pi<float>();
    float y = std::cos(theta);
    float radius = std::sin(theta);
    float angle = i / static_cast<float>(pSegments) * glm::two_pi<float>();
    float angleNext =
        (i + 1) / static_cast<float>(pSegments) * glm::two_pi<float>();

    // Roof left
    positions.emplace_back(glm::cos(angle) * radius, y,
                           glm::sin(angle) * radius);
    // Roof top
    positions.emplace_back(0.0f, 1.0f, 0.0f);
    // Roof right
    positions.emplace_back(glm::cos(angleNext) * radius, y,
                           glm::sin(angleNext) * radius);

    // Connect it
    indices.push_back(i * 3);
    indices.push_back(i * 3 + 1);
    indices.push_back(i * 3 + 2);

    // Texture coords
    texCoords.emplace_back(1 - i / static_cast<float>(pSegments),
                           1 - 1 / static_cast<float>(pRings - 1));
    texCoords.emplace_back(1 - (i + 0.5f) / static_cast<float>(pSegments), 1);
    texCoords.emplace_back(1 - (i + 1) / static_cast<float>(pSegments),
                           1 - 1 / static_cast<float>(pRings - 1));
  }

  // Pillar
  for (unsigned int j = 0; j < pRings - 3; j++) {
    float theta = (j + 1) / static_cast<float>(pRings - 1) * glm::pi<float>();
    float y = std::cos(theta);
    float radius = std::sin(theta);
    float thetaNext =
        (j + 2) / static_cast<float>(pRings - 1) * glm::pi<float>();
    float yNext = std::cos(thetaNext);
    float radiusNext = std::sin(thetaNext);

    for (unsigned int i = 0; i < pSegments; i++) {
      unsigned int startPos = pSegments * (3 + 4 * j) + i * 4;
      unsigned int startIndices = pSegments * (3 + 6 * j) + i * 6;
      float angle = i / static_cast<float>(pSegments) * glm::two_pi<float>();
      float angleNext =
          (i + 1) / static_cast<float>(pSegments) * glm::two_pi<float>();

      // Roof left top
      positions.emplace_back(glm::cos(angle) * radius, y,
                             glm::sin(angle) * radius);
      // Roof right top
      positions.emplace_back(glm::cos(angleNext) * radius, y,
                             glm::sin(angleNext) * radius);
      // Roof right bottom
      positions.emplace_back(glm::cos(angleNext) * radiusNext, yNext,
                             glm::sin(angleNext) * radiusNext);
      // Roof left bottom
      positions.emplace_back(glm::cos(angle) * radiusNext, yNext,
                             glm::sin(angle) * radiusNext);

      // Connect it
      indices.push_back(startPos + 2);
      indices.push_back(startPos);
      indices.push_back(startPos + 1);
      indices.push_back(startPos + 3);
      indices.push_back(startPos);
      indices.push_back(startPos + 2);

      // Texture coords mapping
      texCoords.emplace_back(1 - i / static_cast<float>(pSegments),
                             1 - (j + 1) / static_cast<float>(pRings - 1));
      texCoords.emplace_back(1 - (i + 1) / static_cast<float>(pSegments),
                             1 - (j + 1) / static_cast<float>(pRings - 1));
      texCoords.emplace_back(1 - (i + 1) / static_cast<float>(pSegments),
                             1 - (j + 2) / static_cast<float>(pRings - 1));
      texCoords.emplace_back(1 - i / static_cast<float>(pSegments),
                             1 - (j + 2) / static_cast<float>(pRings - 1));
    }
  }

  // Floor
  for (unsigned int i = 0; i < pSegments; i++) {
    unsigned int startPos = pSegments * (3 + 4 * (pRings - 3)) + i * 3;
    unsigned int startIndices = pSegments * (3 + 6 * (pRings - 3)) + i * 3;
    float theta =
        (pRings - 2) / static_cast<float>(pRings - 1) * glm::pi<float>();
    float y = std::cos(theta);
    float radius = std::sin(theta);
    float angle = i / static_cast<float>(pSegments) * glm::two_pi<float>();
    float angleNext =
        (i + 1) / static_cast<float>(pSegments) * glm::two_pi<float>();

    // Floor left
    positions.emplace_back(glm::cos(angle) * radius, y,
                           glm::sin(angle) * radius);
    // Floor top
    positions.emplace_back(0.0f, -1.0f, 0.0f);
    // Floor right
    positions.emplace_back(glm::cos(angleNext) * radius, y,
                           glm::sin(angleNext) * radius);

    // Connect it
    indices.push_back(startPos + 1);
    indices.push_back(startPos);
    indices.push_back(startPos + 2);

    // Texture coords
    texCoords.emplace_back(1 - i / static_cast<float>(pSegments),
                           1 - (pRings - 2) / static_cast<float>(pRings - 1));
    texCoords.emplace_back(1 - (i + 0.5f) / static_cast<float>(pSegments), 0);
    texCoords.emplace_back(1 - (i + 1) / static_cast<float>(pSegments),
                           1 - (pRings - 2) / static_cast<float>(pRings - 1));
  }

  geom.positions(positions);
  geom.texCoords(texCoords);
  geom.indices(indices);
  geom.normals(positions);
  return geom;
}
