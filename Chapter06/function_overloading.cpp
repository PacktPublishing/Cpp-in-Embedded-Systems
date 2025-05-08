#include <cstdio> 

void print(int a) { 
    printf("Int %d\r\n", a); 
} 

void print(float a) { 
    printf("Float %2.f\r\n", a); 
} 

int main() { 

    print(2); 
    print(2.f); 

    return 0; 
} 
