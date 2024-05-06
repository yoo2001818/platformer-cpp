#include "render/load.hpp"
#include "entt/entity/entity.hpp"
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

using namespace platformer;

glm::vec3 convert_ai_to_glm(aiVector3D pValue) {
  return glm::vec3(pValue.x, pValue.y, pValue.z);
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

void iterate_entity(const aiScene *pScene, entt::registry &pRegistry,
                    aiNode *pNode, entt::entity pParent) {
  auto entity = pRegistry.create();
  pRegistry.emplace<name>(entity, pNode->mName);
  if (pParent != entt::null) {
    pRegistry.emplace<transform>(entity, pParent, pNode->mTransformation);
  } else {
    pRegistry.emplace<transform>(entity, pNode->mTransformation);
  }

  if (pNode->mNumMeshes > 0) {
    // TODO: Create mesh, and attach it
    for (int i = 0; i < pNode->mNumMeshes; i += 1) {
    }
  }

  // TODO: scan animation / lights / camera and attach it (perhaps it'd be
  // better to do it afterwards, as the index is already formed after creating
  // the tree)

  for (int i = 0; i < pNode->mNumChildren; i += 1) {
    auto childNode = pNode->mChildren[i];
    iterate_entity(pScene, pRegistry, childNode, entity);
  }
}

void load_file_to_entity(const std::string &pFilename,
                         entt::registry &pRegistry) {
  Assimp::Importer importer;

  const aiScene *scene = importer.ReadFile(
      pFilename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
                     aiProcess_GenNormals | aiProcess_CalcTangentSpace);
  if (scene == nullptr) {
    throw std::runtime_error(importer.GetErrorString());
  }

  iterate_entity(scene, pRegistry, scene->mRootNode, entt::null);
}
