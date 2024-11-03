#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <stm32f0xx_hal_uart.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

#include <lfs.h>
#include <lfs_ramfs.h>

namespace fs{

struct lfs {

    lfs() = delete;

    static inline lfs_t fs_lfs;

    static void init() {
        const lfs_config * lfs_ramfs_cfg = get_ramfs_lfs_config();
        lfs_format(&fs_lfs, lfs_ramfs_cfg);
        lfs_mount(&fs_lfs, lfs_ramfs_cfg);
    }    
};

class file {
public:
    file(const char * filename, int flags = LFS_O_RDONLY) {
        if(lfs_file_open(&lfs::fs_lfs, &file_, filename, flags) >= 0) {
            is_open_ = true;
        }
    }
    ~file() {
        if(is_open()) {
            printf("Closing file in destructor.\r\n");
            lfs_file_close(&lfs::fs_lfs, &file_);
        }
    }

    [[nodiscard]] bool is_open() const {
        return is_open_;
    }

    int read(std::span<char> buff) {
        return lfs_file_read(&lfs::fs_lfs, 
                            &file_, 
                            reinterpret_cast<void *>(buff.data()), 
                            buff.size() - 1);
    }

    void write(std::span<const char> buff) {
        lfs_file_write(&lfs::fs_lfs, 
                        &file_, 
                        reinterpret_cast<const void *>(buff.data()), 
                        buff.size());
    }
private:
    bool is_open_ = false;
    lfs_file_t file_;
};
};

int main()
{
    hal::init();

    hal::uart_stm32<hal::stm32::uart> uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);
    
    fs::lfs::init();

    {
        fs::file song_file("song.txt", LFS_O_WRONLY | LFS_O_CREAT);
        if(song_file.is_open()) {
            song_file.write("These are some lyrics!");
        }
    }

    fs::file song_file("song.txt");
    std::array<char, 64> buff = {0};
    if(song_file.is_open()) {
        song_file.read(buff);
        printf("This is content from the file\r\n%s\r\n", buff.data());
    }
    while(true)
    {
    }
}
