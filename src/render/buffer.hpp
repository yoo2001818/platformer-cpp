#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__
#include <vector>
namespace platformer {
class gl_buffer {
public:
  gl_buffer(int pType, int pUsage);
  virtual ~gl_buffer();

  unsigned int length();
  void bind();
  void dispose();

  void set(const void *pData, unsigned int pLength, unsigned int pDataSize);
  void set(const void *pData, unsigned pOffset, unsigned int pLength,
           unsigned int pDataSize);
  template <typename T> void set(const std::vector<T> &pData) {
    this->set(static_cast<const void *>(pData.data()), pData.size() * sizeof(T),
              sizeof(T));
  }
  template <typename T>
  void set(const std::vector<T> &pData, unsigned int pOffset) {
    this->set(static_cast<const void *>(pData.data()), pOffset,
              pData.size() * sizeof(T), sizeof(T));
  }
  void buffer_data(const void *pData, unsigned int pLength);
  void buffer_sub_data(const void *pData, unsigned int pOffset,
                       unsigned int pLength);

protected:
  int mType;
  int mUsage;
  int mDataSize = 1;
  int mByteLength = 0;
  unsigned int mBuffer = -1;
};

class gl_array_buffer : public gl_buffer {
public:
  gl_array_buffer(int pUsage);
};

class gl_element_array_buffer : public gl_buffer {
public:
  gl_element_array_buffer(int pUsage);
};

} // namespace platformer
#endif
