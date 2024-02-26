#include "file.hpp"
#include <fstream>
#include <sstream>

using namespace platformer;

std::string platformer::read_file_str(const std::string &pFilename) {
  std::ifstream file(pFilename);
  std::stringstream stream;
  stream << file.rdbuf();
  return stream.str();
}
