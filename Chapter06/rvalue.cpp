#include <cstdio>  

struct my_struct { 

    int a_; 

    my_struct() : a_(0) {} 

    my_struct(int a) : a_(a) {} 

}; 

int main() {  

    printf("a_ = %d\r\n", my_struct().a_); 
    printf("a_ = %d\r\n", (my_struct()=my_struct(16)).a_);  

    return 0;  
} 
