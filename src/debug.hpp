#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__

#include <format>
#include <iostream>

#define DEBUG(...)                                                             \
  std::cout << "DEBUG: "                                                       \
            << "[" << __FILE__ << "][" << __FUNCTION__ << "][Line "            \
            << __LINE__ << "] " << std::format(__VA_ARGS__) << std::endl;

#endif
