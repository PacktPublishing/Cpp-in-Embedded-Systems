#include <cstdio>

struct Base {
  virtual void print() { 
      printf("Base\r\n"); 
  }
};

struct Derived : public Base {
  void print() override { 
      printf("Derived\r\n"); 
  }
};

void printer(Base &base) { 
    base.print(); 

    if(Derived *derived = dynamic_cast<Derived*>(&base); derived!=nullptr) {
        printf("We found Base using RTTI!\r\n");
    }
}

int main() {
  Base base;
  Derived derived;
  printer(base);
  printer(derived);
  return 0;
}
