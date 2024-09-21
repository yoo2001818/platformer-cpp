#include "scenegraph/mesh.hpp"

using namespace platformer;

mesh::mesh() {}
mesh::mesh(const std::vector<mesh_pair> &pMeshes) : mMeshes(pMeshes) {}
mesh::mesh(std::vector<mesh_pair> &&pMeshes) : mMeshes(pMeshes) {}

std::vector<mesh::mesh_pair> &mesh::meshes() { return this->mMeshes; }

bool mesh::shouldRender() const { return this->mShouldRender; }

void mesh::shouldRender(bool mValue) { this->mShouldRender = mValue; }
