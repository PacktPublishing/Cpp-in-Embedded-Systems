#include <cstdio> 

template<typename T> 
T add(T a, T b) { 
   return a + b;  
} 

int main() { 
    int result_int = add(1, 4); 
    float result_float = add(1.11f, 1.91f); 

    printf("result_int = %d\r\n", result_int); 
    printf("result_float = %.2f\r\n", result_float); 
    
    return 0; 
} 