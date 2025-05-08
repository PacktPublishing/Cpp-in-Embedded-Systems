#include <cstdio> 
#include <vector> 
#include <array> 
#include <algorithm> 
#include <numeric> 

void print_container(const auto& container) { 

    for(auto& elem: container) { 
       printf("%d ", elem);  
    } 
    printf("\r\n");  
} 

int main() { 

    std::array<int, 10> src{0}; 
    std::array<int, 10> dst{0}; 

    std::iota(src.begin(), src.end(), 0); 
    std::copy_if(src.begin(), src.end(), dst.begin(),[] (int x) {return x > 3;}); 

    print_container(src); 
    print_container(dst); 

    return 0; 
} 
