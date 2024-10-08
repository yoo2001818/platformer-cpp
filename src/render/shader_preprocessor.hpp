#ifndef __SHADER_PREPROCESS_HPP__
#define __SHADER_PREPROCESS_HPP__

#include <optional>
#include <string>
#include <vector>

namespace platformer {
class shader_preprocessor {
public:
  shader_preprocessor(const std::string &pSource);
  shader_preprocessor(const std::string &pSource,
                      const std::vector<std::string> &pDefines);
  shader_preprocessor(const std::string &pSource,
                      std::vector<std::string> &&pDefines);

  std::string &get();

private:
  std::string mSource;
  std::vector<std::string> mDefines;
  std::optional<std::string> mCode;
};
} // namespace platformer

#endif
