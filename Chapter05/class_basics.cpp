#include <cstdint> 
#include <cstdio>

class uart { 

public: 

    uart(std::uint32_t baud = 9600): baudrate_(baud) {
        printf("%s\n", __func__);
    } 

    void init() { 
        printf("%s\n", __func__);
        write_brr(calculate_uartdiv()); 
    } 

private: 

    std::uint32_t baudrate_; 
    std::uint8_t calculate_uartdiv() { 
        printf("%s\n", __func__);
        return baudrate_ / 32000; 
    } 

    void write_brr(std::uint8_t) {} 
}; 

 

int main () { 

    uart uart1(115200); 
    uart1.init(); 

    return 0; 

} 
