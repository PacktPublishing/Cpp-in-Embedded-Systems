#include <string> 
#include <vector>  
#include <cstdio> 

int main() 
{ 

    std::string str = "Hello world, this is move semantics demo!!!"; 
    printf("str.data address is %p\r\n", (void*)str.data()); 

    std::vector<std::string> v; 

    v.push_back(str); 
    printf("str after copy is <%s>\r\n", str.data()); 

    v.push_back(std::move(str)); 
    //v.push_back(static_cast<std::string&&>(str)); 

    printf("str after move is <%s>\r\n", str.data()); 

    for(const auto & s:v) { 
        printf("s is <%s>\r\n", s.data()); 
        printf("s.data address is %p\r\n", (void*)s.data()); 
    } 

    return 0; 
} 
