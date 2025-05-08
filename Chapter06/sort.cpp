#include <cstdio> 
#include <array>
#include <algorithm> 
#include <random> 

void print_container(const auto& container) { 

    for(auto& elem: container) { 
       printf("%d ", elem);  
    } 
    printf("\r\n");  
}

int main() { 

    std::array<int, 10> src{0}; 

    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution<> distrib(1, 6); 

    auto rand = [&](int x) -> int { 
        return distrib(gen); 
    }; 

    std::transform(src.begin(), src.end(), src.begin(), rand); 
    print_container(src); 


    std::sort(src.begin(), src.end()); 
    print_container(src); 

    return 0; 
} 
