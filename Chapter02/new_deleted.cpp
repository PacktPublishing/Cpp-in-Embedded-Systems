#include <cstdio>
#include <vector>
#include <new>

void *operator new(std::size_t count) = delete;
void *operator new[](std::size_t count) = delete;

void *operator new(std::size_t count, std::align_val_t al) = delete;
void *operator new[](std::size_t count, std::align_val_t al) = delete;
void *operator new(std::size_t count, const std::nothrow_t &tag) = delete;
void *operator new[](std::size_t count, const  std::nothrow_t &tag) = delete;
void *operator new(std::size_t count, std::align_val_t al, const std::nothrow_t &) = delete;
void *operator new[](std::size_t count, std::align_val_t al, const std::nothrow_t &) = delete;

int main() {
 std::vector<int> vec;
 vec.push_back(123);
 printf("vec[0] = %d\r\n", vec[0]);
 return 0;
}

