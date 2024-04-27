#include <cstdio>

struct A {
  A() { printf("A is created!\r\n"); }
  ~A() { printf("A is destroyed!\r\n"); }
};

struct B {
  B() { printf("B is created!\r\n"); }
  ~B() { printf("B is destroyed!\r\n"); }
};

void bar() {
  B b;
  throw 0;
}

void foo() {
  A a;
  bar();
  A a1;
}

int main() {
  try {
    foo();
  } catch (int &p) {
    printf("Catching an exception!\r\n");
  }
  return 0;
}
