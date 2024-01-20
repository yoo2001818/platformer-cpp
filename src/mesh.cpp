#include "mesh.hpp"
#include "entt/entity/fwd.hpp"
#include <vector>

using namespace platformer;

geometry::geometry() {}

geometry::geometry(const geometry &pValue) {}

geometry::geometry(geometry &&pValue) {}

geometry &geometry::operator=(const geometry &pValue) {}

geometry &geometry::operator=(geometry &&pValue) {}

geometry::~geometry() {}

const std::vector<glm::vec3> &geometry::positions() const {}

void geometry::positions(const std::vector<glm::vec3> &pValue) {}

void geometry::positions(std::vector<glm::vec3> &&pValue) {}

const std::vector<glm::vec2> &geometry::texCoords() const {}

void geometry::texCoords(const std::vector<glm::vec2> &pValue) {}

void geometry::texCoords(std::vector<glm::vec2> &&pValue) {}

const std::vector<glm::vec3> &geometry::normals() const {}

void geometry::normals(const std::vector<glm::vec3> &pValue) {}

void geometry::normals(std::vector<glm::vec3> &&pValue) {}

const std::vector<glm::vec4> &geometry::tangents() const {}

void geometry::tangents(const std::vector<glm::vec4> &pValue) {}

void geometry::tangents(std::vector<glm::vec4> &&pValue) {}

const std::vector<unsigned int> &geometry::indices() const {}

void geometry::indices(const std::vector<unsigned int> &pValue) {}

void geometry::indices(std::vector<unsigned int> &&pValue) {}

int geometry::size() {}

void geometry::render() {}

void geometry::dispose() {}

void geometry::calc_normals(geometry &pGeometry) {}

void geometry::calc_tangents(geometry &pGeometry) {}

geometry geometry::load_obj(std::string pFilename) {}

geometry geometry::make_box() {}

const std::string &shader::vertex() const {}

void shader::vertex(const std::string &pCode) {}

void shader::vertex(std::string &&pCode) {}

const std::string &shader::fragment() const {}

void shader::fragment(const std::string &pCode) {}

void shader::fragment(std::string &&pCode) {}

void shader::prepare() {}

void shader::dispose() {}

material::material() {}

void material::prepare() {}

void material::dispose() {}

mesh::mesh() {}

mesh::mesh(std::vector<mesh::mesh_pair> &pMeshes) {}

void mesh::render(entt::registry &pRegistry, const entt::entity &pEntity) {}
