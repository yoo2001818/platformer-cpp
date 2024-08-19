#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__
#include <cstdint>
#include <vector>
namespace platformer {
class buffer {
public:
  buffer();
  virtual ~buffer();

  unsigned int length();
  virtual void bind();
  void dispose();
  void set(const std::vector<uint8_t> &pValue);
  void buffer_data(const std::vector<uint8_t> &pValue);
  void buffer_sub_data(unsigned int offset, const std::vector<uint8_t> &pValue);

private:
  int mType;
  int mUsage;
  int mDataType;
  int mByteLength;
  int mBuffer = -1;
};

} // namespace platformer
#endif
