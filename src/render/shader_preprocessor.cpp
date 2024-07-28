#include "shader_preprocessor.hpp"
#include "file.hpp"
#include <optional>
#include <regex>

using namespace platformer;

shader_preprocessor::shader_preprocessor(const std::string &pPath)
    : mPath(pPath) {}

std::string &shader_preprocessor::get() {
  if (this->mCode != std::nullopt) {
    return this->mCode.value();
  }
  auto input = read_file_str(this->mPath);
  std::string output = "";
  std::string::size_type pos = 0;
  std::string::size_type prev = 0;
  while ((pos = input.find('\n', prev)) != std::string::npos) {
    auto line = input.substr(prev, pos - prev);
    prev = pos + 1;

    std::regex include_pattern("^\\s*#include\\s+\"([^\"]+)\"\\s*$");
    std::smatch match;
    if (std::regex_match(line, match, include_pattern)) {
      auto file_path = match[1];
      shader_preprocessor included(file_path);
      output += included.get() + '\n';
    } else {
      output += line + '\n';
    }
  }
  this->mCode = std::move(output);
  return this->mCode.value();
}
