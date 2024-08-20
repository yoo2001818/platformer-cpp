#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__
#include <vector>
namespace platformer {
class buffer {
public:
  buffer(int pType, int pUsage);
  virtual ~buffer();

  unsigned int length();
  virtual void bind();
  void dispose();

  void set(void *pData, unsigned int pLength, unsigned int pDataSize);
  template <typename T> void set(const std::vector<T> &pData) {
    this->set(pData.data(), pData.size() * sizeof(T), sizeof(T));
  }
  void buffer_data(void *pData, unsigned int pLength);
  void buffer_sub_data(void *pData, unsigned int pOffset, unsigned int pLength);

private:
  int mType;
  int mUsage;
  int mDataSize = 1;
  int mByteLength = 0;
  unsigned int mBuffer = -1;
};

} // namespace platformer
#endif
