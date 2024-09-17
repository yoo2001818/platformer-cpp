#include "render/load.hpp"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/types.h"
#include "entt/entity/entity.hpp"
#include "entt/entity/fwd.hpp"
#include "entt/entt.hpp"
#include "name.hpp"
#include "render/geometry.hpp"
#include "render/material.hpp"
#include "render/mesh.hpp"
#include "transform.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
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

mesh::mesh_pair entity_loader::read_mesh(aiMesh *pMesh) {
  // Create geometry
  std::shared_ptr<geometry> geom = std::make_shared<geometry>();

  std::vector<glm::vec3> positions(pMesh->mNumVertices);
  for (int j = 0; j < pMesh->mNumVertices; j += 1) {
    positions[j] = convert_ai_to_glm(pMesh->mVertices[j]);
  }
  geom->positions(std::move(positions));

  std::vector<glm::vec3> normals(pMesh->mNumVertices);
  for (int j = 0; j < pMesh->mNumVertices; j += 1) {
    normals[j] = convert_ai_to_glm(pMesh->mNormals[j]);
  }
  geom->normals(std::move(normals));

  if (pMesh->mTangents != nullptr) {
    std::vector<glm::vec4> tangents(pMesh->mNumVertices);
    for (int j = 0; j < pMesh->mNumVertices; j += 1) {
      glm::vec3 normal = convert_ai_to_glm(pMesh->mNormals[j]);
      glm::vec3 tangent = convert_ai_to_glm(pMesh->mTangents[j]);
      glm::vec3 bitangent = convert_ai_to_glm(pMesh->mBitangents[j]);
      auto leftBitangent = glm::cross(glm::vec3(tangent), normal);
      auto modifier = glm::dot(bitangent, tangent);
      tangents[j] = glm::vec4(tangent, modifier < 0.0f ? -1.0f : 1.0f);
    }
    geom->tangents(std::move(tangents));
  }

  if (pMesh->mTextureCoords[0] != nullptr) {
    std::vector<glm::vec2> texCoords(pMesh->mNumVertices);
    for (int j = 0; j < pMesh->mNumVertices; j += 1) {
      texCoords[j] = convert_ai_to_glm(pMesh->mTextureCoords[0][j]);
    }
    geom->texCoords(std::move(texCoords));
  }

  if (pMesh->mTextureCoords[1] != nullptr) {
    std::vector<glm::vec2> texCoords2(pMesh->mNumVertices);
    for (int j = 0; j < pMesh->mNumVertices; j += 1) {
      texCoords2[j] = convert_ai_to_glm(pMesh->mTextureCoords[1][j]);
    }
    geom->texCoords2(std::move(texCoords2));
  }

  if (pMesh->mColors[0] != nullptr) {
    std::vector<glm::vec4> colors(pMesh->mNumVertices);
    for (int j = 0; j < pMesh->mNumVertices; j += 1) {
      colors[j] = convert_ai_to_glm(pMesh->mColors[0][j]);
    }
    geom->colors(std::move(colors));
  }

  if (pMesh->mNumBones > 0) {
    // This is kinda weird, given that
    auto &bone = pMesh->mBones[0];
  }

  std::vector<unsigned int> indices;
  for (int j = 0; j < pMesh->mNumFaces; j += 1) {
    auto &face = pMesh->mFaces[j];
    for (int k = 0; k < face.mNumIndices; k += 1) {
      // We expect that the data is triangulated
      indices.push_back(face.mIndices[k]);
    }
  }
  geom->indices(std::move(indices));

  if (geom->tangents().empty() && !geom->texCoords().empty()) {
    geometry::calc_tangents(*geom);
  }

  // Create material
  auto origMat = this->mScene->mMaterials[pMesh->mMaterialIndex];
  std::shared_ptr<standard_material> mat =
      std::make_shared<standard_material>();
  aiColor3D base;
  float metalic;
  float roughness;

  origMat->Get(AI_MATKEY_BASE_COLOR, base);
  origMat->Get(AI_MATKEY_METALLIC_FACTOR, metalic);
  origMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
  mat->color = convert_ai_to_glm(base);
  mat->metalic = metalic;
  mat->roughness = roughness;

  return std::make_pair(mat, geom);
}

void entity_loader::iterate_entity(aiNode *pNode, entt::entity pParent) {
  auto entity = mRegistry.create();
  mRegistry.emplace<name>(entity, std::string{pNode->mName.C_Str()});
  if (pParent != entt::null) {
    mRegistry.emplace<transform>(entity, pParent,
                                 convert_ai_to_glm(pNode->mTransformation));
  } else {
    mRegistry.emplace<transform>(entity,
                                 convert_ai_to_glm(pNode->mTransformation));
  }

  if (pNode->mNumMeshes > 0) {
    std::vector<mesh::mesh_pair> pairs;
    for (int i = 0; i < pNode->mNumMeshes; i += 1) {
      int meshIndex = pNode->mMeshes[i];
      auto origMesh = mScene->mMeshes[meshIndex];

      // Read out mesh and convert it to the mesh object
      auto mesh_pair = read_mesh(origMesh);
      pairs.push_back(mesh_pair);
    }
    mRegistry.emplace<mesh_component>(entity, std::make_shared<mesh>(pairs));
  }

  // TODO: scan animation / lights / camera and attach it (perhaps it'd be
  // better to do it afterwards, as the index is already formed after creating
  // the tree)

  for (int i = 0; i < pNode->mNumChildren; i += 1) {
    auto childNode = pNode->mChildren[i];
    iterate_entity(childNode, entity);
  }
}

void entity_loader::load() {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
      this->mFilename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                           aiProcess_GenNormals | aiProcess_CalcTangentSpace);
  if (scene == nullptr) {
    throw std::runtime_error(importer.GetErrorString());
  }
  this->mScene = scene;
  iterate_entity(scene->mRootNode, entt::null);
  this->mScene = nullptr;
  importer.FreeScene();
}

void platformer::load_file_to_entity(const std::string &pFilename,
                                     entt::registry &pRegistry) {
  entity_loader loader(pFilename, pRegistry);
  loader.load();
}
