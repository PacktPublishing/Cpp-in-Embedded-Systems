#include <span> 
#include <cstdio> 
#include <cstdint> 

template<typename U> 
class uart_interface { 
public: 
    void init(std::uint32_t baudrate = 9600) { 
       static_cast<U*>(this)->initImpl(baudrate); 
    } 

    void write(std::span<const char> data) { 
       static_cast<U*>(this)->writeImpl(data); 
    } 
}; 

class uart_stm32 : public uart_interface<uart_stm32> { 
public: 
    void initImpl(std::uint32_t baudrate = 9600) { 
        printf("uart_stm32::init: setting baudrate to %d\r\n", baudrate); 
    } 

    void writeImpl(std::span<const char> data) { 
        printf("uart_stm32::write: "); 
        for(auto ch: data) { 
            putc(ch, stdout); 
        } 
    } 
}; 

template<typename T> 
concept TheUart = std::derived_from<T, uart_interface<T>>; 

template<TheUart T> 
class gsm_lib{ 
public: 
    gsm_lib(T &u) : uart_(u) {} 

    void init() {
        printf("gsm_lib::init: sending AT command\r\n"); 
        uart_.write("AT"); 
    } 

private: 
    T &uart_; 
}; 

int main() { 
    uart_stm32 uart_stm32_obj; 
    uart_stm32_obj.init(115200); 

    gsm_lib gsm(uart_stm32_obj); 
    gsm.init(); 

    return 0; 
} 