#include <array>
#include <cstdio>
#include <cstdlib>
#include <memory_resource>
#include <new>
#include <vector>

void *operator new(std::size_t count, std::align_val_t al) {
  printf("%s, size = %ld\r\n", __PRETTY_FUNCTION__, count);
  return std::malloc(count);
}

int main() {
  using namespace std;
  using namespace std::pmr;

  constexpr size_t n_elem = 8;
  array<uint8_t, sizeof(uint8_t) * 8> buffer{0};
  monotonic_buffer_resource mbr{buffer.data(), buffer.size()};
  polymorphic_allocator<uint8_t> pa{&mbr};
  std::pmr::vector<uint8_t> vec{pa};
  // vec.reserve(n_elem);
  //
  for (uint8_t i = 0; i < n_elem; i++) {
    vec.push_back(i);
  }

  for (uint8_t data : buffer) {
    printf("%d ", data);
  }
  printf("\r\n");

  return 0;
}
