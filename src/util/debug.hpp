#ifndef __DEBUG_HPP__
#define __DEBUG_HPP__

#include <format>
#include <iostream>
#include <string.h>

#define __FILENAME__                                                           \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DEBUG(...)                                                             \
  std::cout << "DEBUG: "                                                       \
            << "[" << __FILENAME__ << "][" << __FUNCTION__ << "][Line "        \
            << __LINE__ << "] " << std::format(__VA_ARGS__) << std::endl;

#endif
