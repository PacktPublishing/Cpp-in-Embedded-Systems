#include <cstdio>

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
T add(T a, T b) { 
   return a + b;  
} 

int main() {  

    point a{1, 2}; 
    point b{2, 1}; 

    auto c = add(a, b); 
    c.print();
    return 0; 
} 