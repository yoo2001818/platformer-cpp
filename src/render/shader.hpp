#ifndef __RENDER_SHADER_HPP__
#define __RENDER_SHADER_HPP__
#include <entt/entt.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <string>
namespace platformer {
class geometry;

class texture {
public:
  int placeholder;
};

class shader {
public:
  shader();
  shader(const std::string &pVertex, const std::string &pFragment);
  shader(const shader &pValue);
  shader(shader &&pValue);

  shader &operator=(const shader &pValue);
  shader &operator=(shader &&pValue);

  ~shader();

  const std::string &vertex() const;
  void vertex(const std::string &pCode);
  void vertex(std::string &&pCode);
  const std::string &fragment() const;
  void fragment(const std::string &pCode);
  void fragment(std::string &&pCode);

  void set(const std::string &pName, float pValue);
  void set(const std::string &pName, const glm::vec2 &pValue);
  void set(const std::string &pName, const glm::vec3 &pValue);
  void set(const std::string &pName, const glm::vec4 &pValue);
  void set(const std::string &pName, const glm::mat2 &pValue);
  void set(const std::string &pName, const glm::mat3 &pValue);
  void set(const std::string &pName, const glm::mat4 &pValue);

  void set(const std::string &pName, int pOffset, float pValue);
  void set(const std::string &pName, int pOffset, const glm::vec2 &pValue);
  void set(const std::string &pName, int pOffset, const glm::vec3 &pValue);
  void set(const std::string &pName, int pOffset, const glm::vec4 &pValue);
  void set(const std::string &pName, int pOffset, const glm::mat2 &pValue);
  void set(const std::string &pName, int pOffset, const glm::mat3 &pValue);
  void set(const std::string &pName, int pOffset, const glm::mat4 &pValue);

  void prepare();
  void dispose();

private:
  std::string mVertex;
  std::string mFragment;
  unsigned int mProgramId = -1;
  bool mIsDirty = true;
  friend geometry;
};

} // namespace platformer
#endif
