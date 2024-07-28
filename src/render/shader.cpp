#include "render/shader_preprocessor.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "debug.hpp"
#include "render/shader.hpp"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

using namespace platformer;

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

void shader::set(const std::string &pName, int pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniform1i(pos, pValue);
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

void shader::set(const std::string &pName, int pOffset, int pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniform1i(pos + pOffset, pValue);
}
void shader::set(const std::string &pName, int pOffset, float pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniform1f(pos + pOffset, pValue);
}
void shader::set(const std::string &pName, int pOffset,
                 const glm::vec2 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniform2fv(pos + pOffset, 1, glm::value_ptr(pValue));
}
void shader::set(const std::string &pName, int pOffset,
                 const glm::vec3 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniform3fv(pos + pOffset, 1, glm::value_ptr(pValue));
}
void shader::set(const std::string &pName, int pOffset,
                 const glm::vec4 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniform4fv(pos + pOffset, 1, glm::value_ptr(pValue));
}
void shader::set(const std::string &pName, int pOffset,
                 const glm::mat2 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniformMatrix2fv(pos + pOffset, 1, false, glm::value_ptr(pValue));
}
void shader::set(const std::string &pName, int pOffset,
                 const glm::mat3 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniformMatrix3fv(pos + pOffset, 1, false, glm::value_ptr(pValue));
}
void shader::set(const std::string &pName, int pOffset,
                 const glm::mat4 &pValue) {
  auto pos = glGetUniformLocation(this->mProgramId, pName.c_str());
  if (pos == -1)
    return;
  glUniformMatrix4fv(pos + pOffset, 1, false, glm::value_ptr(pValue));
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
    DEBUG("Shader {} prepared", this->mProgramId);
  }
  glUseProgram(this->mProgramId);
}

void shader::dispose() {
  if (this->mProgramId != -1) {
    DEBUG("Shader {} destroyed", this->mProgramId);
    glDeleteProgram(this->mProgramId);
    this->mProgramId = -1;
  }
}

file_shader::file_shader(const std::string &pVertexPath,
                         const std::string &pFragmentPath)
    : shader() {
  shader_preprocessor vertex(pVertexPath);
  shader_preprocessor fragment(pFragmentPath);
  this->vertex(vertex.get());
  this->fragment(fragment.get());
}
