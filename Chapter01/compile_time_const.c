#include <stdio.h>

#define VOLTAGE 3300
#define CURRENT 1000

int main() {
  const float resistance = VOLTAGE / CURRENT;
  printf("resistance = %.2f\r\n", resistance);
  return 0;
}
