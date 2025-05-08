#include <cstdio> 

  

int main() { 

    int a = 42;  

    int& a_ref = a; 

    const int& a_const_ref = a; 


    printf("a = %d\r\n", a); 

  
    a_ref = 16; 

    printf("a = %d\r\n", a); 


    // a_const_ref = 16; compiler error 

    return 0;  

} 
