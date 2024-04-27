#include <cstdio>
#include <limits>

int foo(int x) {
  int y = x + 1;
  return y > x;
}

int main() {
  if (foo(std::numeric_limits<int>::max())) {
    printf("X is larger than X + 1\r\n");
  } else {
    printf("X is NOT larger than X + 1. Oh nooo !\r\n");
  }
  return 0;
}
