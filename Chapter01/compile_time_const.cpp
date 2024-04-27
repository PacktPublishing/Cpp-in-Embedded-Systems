#include <cstdio>

constexpr float voltage = 3300;
constexpr float current = 1000;

int main() {
  const float resistance = voltage / current;
  printf("resistance = %.2f\r\n", resistance);
  return 0;
}
