#include <array> 

int sum(const std::array<int, 4> &arr) { 

    int ret; 

    for(int elem: arr) { 
        ret += elem; 
    } 

    return ret; 

} 
