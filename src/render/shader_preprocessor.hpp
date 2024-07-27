#ifndef __SHADER_PREPROCESS_HPP__
#define __SHADER_PREPROCESS_HPP__

#include <string>
namespace platformer {
class shader_preprocessor {
public:
  shader_preprocessor(const std::string &pPath);

  std::string get();

private:
  std::string mPath;
  std::string mCode;
};
} // namespace platformer

#endif
