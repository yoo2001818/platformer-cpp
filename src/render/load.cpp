#include "render/load.hpp"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/texture.h"
#include "assimp/types.h"
#include "debug.hpp"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include "name.hpp"
#include "render/armature.hpp"
#include "render/geometry.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
#include "render/texture.hpp"
#include "transform.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cstdint>
#include <filesystem>
#include <glm/fwd.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

using namespace platformer;

glm::vec3 convert_ai_to_glm(aiVector3D pValue) {
  return glm::vec3(pValue.x, pValue.y, pValue.z);
}

glm::vec3 convert_ai_to_glm(aiColor3D pValue) {
  return glm::vec3(pValue.r, pValue.g, pValue.b);
}

glm::vec4 convert_ai_to_glm(aiColor4D pValue) {
  return glm::vec4(pValue.r, pValue.g, pValue.b, pValue.a);
}

glm::mat4 convert_ai_to_glm(const aiMatrix4x4 &from) {
  glm::mat4 to;
  to[0][0] = from.a1;
  to[1][0] = from.a2;
  to[2][0] = from.a3;
  to[3][0] = from.a4;
  to[0][1] = from.b1;
  to[1][1] = from.b2;
  to[2][1] = from.b3;
  to[3][1] = from.b4;
  to[0][2] = from.c1;
  to[1][2] = from.c2;
  to[2][2] = from.c3;
  to[3][2] = from.c4;
  to[0][3] = from.d1;
  to[1][3] = from.d2;
  to[2][3] = from.d3;
  to[3][3] = from.d4;
  return to;
}

mesh platformer::load_file_to_mesh(const std::string &pFilename) {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
      pFilename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                     aiProcess_GenNormals | aiProcess_CalcTangentSpace);
  if (scene == nullptr) {
    throw std::runtime_error(importer.GetErrorString());
  }

  // Normally, we would use mNode (the root node) to traverse and copy the
  // nodes into the registry. However, since we want to just read one mesh
  // from the scene, let's not do that, but instead read mMeshes to simplify
  // the implementation.
  std::vector<mesh::mesh_pair> pairs;
  std::shared_ptr<material> mat = std::make_shared<standard_material>();
  for (int i = 0; i < scene->mNumMeshes; i += 1) {
    auto &mesh = scene->mMeshes[i];
    auto &material = scene->mMaterials[mesh->mMaterialIndex];
    std::shared_ptr<geometry> geom = std::make_shared<geometry>();

    std::vector<glm::vec3> positions(mesh->mNumVertices);
    for (int j = 0; j < mesh->mNumVertices; j += 1) {
      positions[j] = convert_ai_to_glm(mesh->mVertices[j]);
    }
    geom->positions(std::move(positions));

    std::vector<glm::vec3> normals(mesh->mNumVertices);
    for (int j = 0; j < mesh->mNumVertices; j += 1) {
      normals[j] = convert_ai_to_glm(mesh->mNormals[j]);
    }
    geom->normals(std::move(normals));

    if (mesh->mTextureCoords[0] != nullptr) {
      std::vector<glm::vec2> texCoords(mesh->mNumVertices);
      for (int j = 0; j < mesh->mNumVertices; j += 1) {
        texCoords[j] = convert_ai_to_glm(mesh->mTextureCoords[0][j]);
      }
      geom->texCoords(std::move(texCoords));
    }

    std::vector<unsigned int> indices;
    for (int j = 0; j < mesh->mNumFaces; j += 1) {
      auto &face = mesh->mFaces[j];
      for (int k = 0; k < face.mNumIndices; k += 1) {
        // We expect that the data is triangulated
        indices.push_back(face.mIndices[k]);
      }
    }
    geom->indices(std::move(indices));

    pairs.push_back({mat, geom});
  }

  return mesh(pairs);
}

entity_loader::entity_loader(const std::string &pFilename,
                             entt::registry &pRegistry)
    : mFilename(pFilename), mRegistry(pRegistry) {}

std::shared_ptr<texture> entity_loader::read_texture(std::string pFilename) {
  auto tex = mScene->GetEmbeddedTexture(pFilename.data());
  if (tex != nullptr) {
    // Try to read texture from it
    if (tex->mHeight == 0) {
      std::byte *byteData =
          static_cast<std::byte *>(static_cast<void *>(tex->pcData));
      auto texture =
          std::make_shared<texture_2d>(texture_source(texture_source_image{
              .format = {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE},
              .filename = std::string(tex->mFilename.C_Str()),
              .data = std::vector<std::byte>(byteData, byteData + tex->mWidth),
          }));
      return texture;
    } else {
      // Not supported
      return nullptr;
    }
  } else {
    // Read from the file (which is already provided as a path)
    std::filesystem::path path(this->mFilename);
    auto dir = path.parent_path();
    auto texPath = dir / tex->mFilename.C_Str();
    auto texture =
        std::make_shared<texture_2d>(texture_source(texture_source_image{
            .format = {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE},
            .filename = std::string(texPath),
        }));
    return texture;
  }
}

std::shared_ptr<material> entity_loader::read_material(int pIndex) {
  if (this->mMaterials[pIndex] != nullptr) {
    return this->mMaterials[pIndex];
  }

  auto material = this->mScene->mMaterials[pIndex];
  DEBUG("Read material {} ({:x})", pIndex, (uintptr_t)material);

  for (int i = 0; i < material->mNumProperties; i += 1) {
    auto prop = material->mProperties[i];
    switch (prop->mType) {
    case aiPTI_Float:
      DEBUG("Material prop (float) {}, {} bytes",
            std::string(prop->mKey.C_Str()), prop->mDataLength);
      break;
    case aiPTI_Double:
      DEBUG("Material prop (double) {}, {} bytes",
            std::string(prop->mKey.C_Str()), prop->mDataLength);
      break;
    case aiPTI_String:
      DEBUG("Material prop (string) {}, {} bytes",
            std::string(prop->mKey.C_Str()), prop->mDataLength);
      break;
    case aiPTI_Integer:
      DEBUG("Material prop (int) {}, {} bytes", std::string(prop->mKey.C_Str()),
            prop->mDataLength);
      break;
    case aiPTI_Buffer:
      DEBUG("Material prop (buf) {}, {} bytes", std::string(prop->mKey.C_Str()),
            prop->mDataLength);
      break;
    default:
      break;
    }
  }

  std::shared_ptr<standard_material> mat =
      std::make_shared<standard_material>();
  aiColor3D base;
  float metalic;
  float roughness;

  material->Get(AI_MATKEY_BASE_COLOR, base);
  material->Get(AI_MATKEY_METALLIC_FACTOR, metalic);
  material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
  DEBUG("Metalic {}, Roughness {}", metalic, roughness);
  mat->color = convert_ai_to_glm(base);
  mat->metalic = metalic;
  mat->roughness = roughness;

  if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
    aiString texPath;
    // Disregard texture op and stuff (it's invalid in GLTF anyway)
    material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath);
    mat->diffuseTexture = this->read_texture(texPath.C_Str());
  }

  if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
    aiString texPath;
    // Disregard texture op and stuff (it's invalid in GLTF anyway)
    material->GetTexture(aiTextureType_NORMALS, 0, &texPath);
    mat->normalTexture = this->read_texture(texPath.C_Str());
  }

  this->mMaterials[pIndex] = mat;

  return mat;
}

std::shared_ptr<geometry> entity_loader::read_geometry(int pIndex) {
  if (this->mGeometries[pIndex] != nullptr) {
    return this->mGeometries[pIndex];
  }

  auto mesh = this->mScene->mMeshes[pIndex];
  DEBUG("Read geometry {} ({:x})", pIndex, (uintptr_t)mesh);
  // Create geometry
  std::shared_ptr<geometry> geom = std::make_shared<geometry>();

  std::vector<glm::vec3> positions(mesh->mNumVertices);
  for (int j = 0; j < mesh->mNumVertices; j += 1) {
    positions[j] = convert_ai_to_glm(mesh->mVertices[j]);
  }
  geom->positions(std::move(positions));

  std::vector<glm::vec3> normals(mesh->mNumVertices);
  for (int j = 0; j < mesh->mNumVertices; j += 1) {
    normals[j] = convert_ai_to_glm(mesh->mNormals[j]);
  }
  geom->normals(std::move(normals));

  if (mesh->mTangents != nullptr) {
    std::vector<glm::vec4> tangents(mesh->mNumVertices);
    for (int j = 0; j < mesh->mNumVertices; j += 1) {
      glm::vec3 normal = convert_ai_to_glm(mesh->mNormals[j]);
      glm::vec3 tangent = convert_ai_to_glm(mesh->mTangents[j]);
      glm::vec3 bitangent = convert_ai_to_glm(mesh->mBitangents[j]);
      auto leftBitangent = glm::cross(glm::vec3(tangent), normal);
      auto modifier = glm::dot(bitangent, tangent);
      tangents[j] = glm::vec4(tangent, modifier < 0.0f ? -1.0f : 1.0f);
    }
    geom->tangents(std::move(tangents));
  }

  if (mesh->mTextureCoords[0] != nullptr) {
    std::vector<glm::vec2> texCoords(mesh->mNumVertices);
    for (int j = 0; j < mesh->mNumVertices; j += 1) {
      texCoords[j] = convert_ai_to_glm(mesh->mTextureCoords[0][j]);
    }
    geom->texCoords(std::move(texCoords));
  }

  if (mesh->mTextureCoords[1] != nullptr) {
    std::vector<glm::vec2> texCoords2(mesh->mNumVertices);
    for (int j = 0; j < mesh->mNumVertices; j += 1) {
      texCoords2[j] = convert_ai_to_glm(mesh->mTextureCoords[1][j]);
    }
    geom->texCoords2(std::move(texCoords2));
  }

  if (mesh->mColors[0] != nullptr) {
    std::vector<glm::vec4> colors(mesh->mNumVertices);
    for (int j = 0; j < mesh->mNumVertices; j += 1) {
      colors[j] = convert_ai_to_glm(mesh->mColors[0][j]);
    }
    geom->colors(std::move(colors));
  }

  if (mesh->mNumBones > 0) {
    // This is kinda weird, given that GLTF already packs the data neatly
    std::vector<std::array<int, 4>> boneIds(mesh->mNumVertices);
    std::vector<std::array<float, 4>> boneWeights(mesh->mNumVertices);
    // Assign bone ids
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex += 1) {
      auto bone = mesh->mBones[boneIndex];
      for (int weightIndex = 0; weightIndex < bone->mNumWeights;
           weightIndex += 1) {
        auto weight = bone->mWeights[weightIndex];
        int vertexId = weight.mVertexId;
        for (int i = 0; i < 4; i += 1) {
          if (boneWeights[vertexId][i] == 0.0f) {
            boneIds[vertexId][i] = boneIndex;
            boneWeights[vertexId][i] = weight.mWeight;
            break;
          }
        }
      }
    }
    // Normalize weights
    for (int i = 0; i < mesh->mNumVertices; i += 1) {
      float total = 0.0f;
      for (int j = 0; j < 4; j += 1) {
        total += boneWeights[i][j];
      }
      if (total > 0.0f) {
        float coeff = 1.0f / total;
        for (int j = 0; j < 4; j += 1) {
          boneWeights[i][j] *= coeff;
        }
      }
    }
    std::vector<glm::ivec4> boneIdsVec(mesh->mNumVertices);
    std::vector<glm::vec4> boneWeightsVec(mesh->mNumVertices);
    for (int i = 0; i < mesh->mNumVertices; i += 1) {
      boneIdsVec[i] = glm::ivec4(boneIds[i][0], boneIds[i][1], boneIds[i][2],
                                 boneIds[i][3]);
      boneWeightsVec[i] = glm::vec4(boneWeights[i][0], boneWeights[i][1],
                                    boneWeights[i][2], boneWeights[i][3]);
    }
    geom->boneIds(boneIdsVec);
    geom->boneWeights(boneWeightsVec);

    // NOTE: If a bone data is provided, the entity with the mesh should contain
    // bone data.
  }

  std::vector<unsigned int> indices;
  for (int j = 0; j < mesh->mNumFaces; j += 1) {
    auto &face = mesh->mFaces[j];
    for (int k = 0; k < face.mNumIndices; k += 1) {
      // We expect that the data is triangulated
      indices.push_back(face.mIndices[k]);
    }
  }
  geom->indices(std::move(indices));

  if (geom->tangents().empty() && !geom->texCoords().empty()) {
    geometry::calc_tangents(*geom);
  }

  this->mGeometries[pIndex] = geom;

  return geom;
}

armature_component entity_loader::read_mesh_armature(int pIndex) {
  auto mesh = this->mScene->mMeshes[pIndex];
  // FIXME: This only supports one armature per entity, which is obviously
  // problematic. For instance, how is it possible to support multi-material
  // with this?
  if (mesh->mNumBones == 0)
    return {};
  armature_component component;
  component.bones.reserve(mesh->mNumBones);
  DEBUG("Loading bones ({} bones)", mesh->mNumBones);
  for (int boneId = 0; boneId < mesh->mNumBones; boneId += 1) {
    auto bone = mesh->mBones[boneId];
    platformer::bone targetBone;
    targetBone.inverseBindMatrix = convert_ai_to_glm(bone->mOffsetMatrix);
    targetBone.joint = this->mEntities[bone->mNode];
    component.bones.push_back(targetBone);
    component.root = this->mEntities[bone->mArmature];
  }
  return component;
}

mesh::mesh_pair entity_loader::read_mesh(int pIndex) {
  auto mesh = this->mScene->mMeshes[pIndex];
  auto geometry = this->read_geometry(pIndex);
  auto material = this->read_material(mesh->mMaterialIndex);

  return std::make_pair(material, geometry);
}

void entity_loader::iterate_entity(aiNode *pNode, entt::entity pParent) {
  auto entity = mRegistry.create();
  auto nameVal = std::string{pNode->mName.C_Str()};
  mRegistry.emplace<name>(entity, nameVal);
  this->mEntities.insert({pNode, entity});
  this->mEntityByNames.insert({nameVal, entity});
  if (pParent != entt::null) {
    mRegistry.emplace<transform>(entity, pParent,
                                 convert_ai_to_glm(pNode->mTransformation));
  } else {
    mRegistry.emplace<transform>(entity,
                                 convert_ai_to_glm(pNode->mTransformation));
  }

  for (int i = 0; i < pNode->mNumChildren; i += 1) {
    auto childNode = pNode->mChildren[i];
    iterate_entity(childNode, entity);
  }
}

void entity_loader::attach_entity(aiNode *pNode, entt::entity pEntity) {
  if (pNode->mNumMeshes > 0) {
    std::vector<mesh::mesh_pair> pairs;
    for (int i = 0; i < pNode->mNumMeshes; i += 1) {
      int meshIndex = pNode->mMeshes[i];
      auto mesh = mScene->mMeshes[meshIndex];
      auto mesh_pair = read_mesh(meshIndex);
      pairs.push_back(mesh_pair);
      if (mesh->mNumBones > 0 && i == 0) {
        mRegistry.emplace<armature_component>(pEntity,
                                              read_mesh_armature(meshIndex));
      }
    }
    mRegistry.emplace<mesh_component>(pEntity, std::make_shared<mesh>(pairs));
  }

  // TODO: scan animation / lights / camera and attach it (perhaps it'd be
  // better to do it afterwards, as the index is already formed after creating
  // the tree)
}

void entity_loader::load() {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
      this->mFilename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                           aiProcess_GenNormals | aiProcess_CalcTangentSpace |
                           aiProcess_PopulateArmatureData);
  if (scene == nullptr) {
    throw std::runtime_error(importer.GetErrorString());
  }
  this->mScene = scene;
  this->mEntities.clear();
  this->mMaterials.resize(scene->mNumMaterials, nullptr);
  this->mGeometries.resize(scene->mNumMeshes, nullptr);
  iterate_entity(scene->mRootNode, entt::null);
  // Construct armature_component for each skeleton, and assign it to meshes
  // Attach lights
  // Attach cameras
  for (auto [node, entity] : this->mEntities) {
    this->attach_entity(node, entity);
  }
  this->mScene = nullptr;
  this->mEntities.clear();
  this->mMaterials.clear();
  this->mGeometries.clear();
  importer.FreeScene();
}

void platformer::load_file_to_entity(const std::string &pFilename,
                                     entt::registry &pRegistry) {
  entity_loader loader(pFilename, pRegistry);
  loader.load();
}
