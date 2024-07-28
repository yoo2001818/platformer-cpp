#include "shader_preprocessor.hpp"
#include "file.hpp"
#include <optional>
#include <regex>
#include <vector>

using namespace platformer;

shader_preprocessor::shader_preprocessor(const std::string &pPath)
    : mPath(pPath) {}

shader_preprocessor::shader_preprocessor(
    const std::string &pPath, const std::vector<std::string> &pDefines)
    : mPath(pPath), mDefines(pDefines) {}

shader_preprocessor::shader_preprocessor(const std::string &pPath,
                                         std::vector<std::string> &&pDefines)
    : mPath(pPath), mDefines(pDefines) {}

std::string &shader_preprocessor::get() {
  if (this->mCode != std::nullopt) {
    return this->mCode.value();
  }
  auto input = read_file_str(this->mPath);
  std::string output = "";
  std::string::size_type pos = 0;
  std::string::size_type prev = 0;

  std::regex includePattern("^\\s*#include\\s+\"([^\"]+)\"\\s*$");
  std::regex versionPattern("^\\s*#version.+$");

  int lineCount = 0;
  while ((pos = input.find('\n', prev)) != std::string::npos) {
    auto line = input.substr(prev, pos - prev);
    prev = pos + 1;
    lineCount += 1;

    // TODO: This is a mess, but it still satisfies its requirements.
    std::smatch match;
    if (std::regex_match(line, match, includePattern)) {
      auto file_path = match[1];
      // Since the included script won't contain a version directive, add the
      // line here
      output += "#line 1 1\n";
      shader_preprocessor included(file_path, this->mDefines);
      output += included.get() + '\n';
      output += "#line " + std::to_string(lineCount) + " 0\n";
    } else if (std::regex_match(line, match, versionPattern)) {
      output += line + '\n';
      // If there are any defined defines, add it right below #version
      for (auto &define : this->mDefines) {
        output += "#define " + define + "\n";
      }
      output += "#line " + std::to_string(lineCount) + " 0\n";
    } else {
      output += line + '\n';
    }
  }
  this->mCode = std::move(output);
  return this->mCode.value();
}
