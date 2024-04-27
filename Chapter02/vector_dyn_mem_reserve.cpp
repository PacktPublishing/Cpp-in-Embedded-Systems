#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <vector>

void *operator new(std::size_t count) {
  printf("%s, size = %ld\r\n", __PRETTY_FUNCTION__, count);
  return std::malloc(count);
}

void operator delete(void *ptr) noexcept {
  printf("%s\r\n", __PRETTY_FUNCTION__);
  std::free(ptr);
}

int main() {
  std::vector<std::uint8_t> vec;
  constexpr std::size_t n_elem = 8;

  vec.reserve(n_elem);

  for (std::uint8_t i = 0; i < n_elem; i++) {
    vec.push_back(i);
  }
  const auto print_array = [](uint8_t *arr, std::size_t n) {
    for (std::size_t i = 0; i < n; i++) {
      printf("%d ", arr[i]);
    }
    printf("\r\n");
  };
  print_array(vec.data(), n_elem);
  return 0;
}
