#include <cstdio>
#include <type_traits>

struct point { 
    int x; 
    int y; 

    point operator+(const point& other) const { 
        return point{x + other.x, y + other.y}; 
    } 

    void print() { 
        printf("x = %d, y = %d\r\n", x, y); 
    } 
}; 

template<typename T> 
std::enable_if<std::is_arithmetic<T>::value, T>::type 
add(T a, T b) { 
   return a + b;  
} 

int main() { 

    auto a = add(1, 2); // OK 
    auto b = add(1.1, 2.1); // OK 

    point p_a{1, 2};  
    point p_b{2, 1};   
    auto c = add(p_a, p_b); // compile-error 

    return 0; 

} 