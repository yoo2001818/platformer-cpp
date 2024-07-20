#include "file.hpp"
#include <format>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace platformer;

std::string platformer::read_file_str(const std::string &pFilename) {
  std::ifstream file(pFilename);
  if (file.fail()) {
    throw std::runtime_error(std::format("File {} does not exist", pFilename));
  }
  std::stringstream stream;
  stream << file.rdbuf();
  return stream.str();
}
