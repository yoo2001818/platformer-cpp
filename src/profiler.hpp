#ifndef __PROFILER_HPP__
#define __PROFILER_HPP__

#include <map>
#include <string>
namespace platformer {
class profiler {
public:
  void start(const std::string &pName);
  void end();
  std::map<std::string, int> stats();
  void clear();
};
} // namespace platformer

#endif
