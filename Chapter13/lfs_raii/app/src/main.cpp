#include <cstdint>
#include <cstdio>

#include <stm32f072xb.h>

#include <hal.hpp>
#include <stm32f0xx_hal_uart.hpp>
#include <uart_stm32.hpp>

#include <retarget.hpp>

#include <lfs.h>
#include <lfs_ramfs.h>

int main()
{
    hal::init();

    hal::uart_stm32<hal::stm32::uart> uart(USART2);
    uart.init();

    retarget::set_stdio_uart(&uart);

    lfs_t lfs;
    const lfs_config * lfs_ramfs_cfg = get_ramfs_lfs_config();
    lfs_format(&lfs, lfs_ramfs_cfg);
    lfs_mount(&lfs, lfs_ramfs_cfg);

    lfs_file_t file;

    if(lfs_file_open(&lfs, &file, "song.txt", LFS_O_WRONLY | LFS_O_CREAT) >= 0) {
        const char * file_content = "These are some lyrics!";
        lfs_file_write(&lfs, 
                        &file, 
                        reinterpret_cast<const void *>(file_content), 
                        strlen(file_content));
        lfs_file_close(&lfs, &file);
    }

    if(lfs_file_open(&lfs, &file, "song.txt", LFS_O_RDONLY) >= 0) {
        std::array<char, 64> buff = {0};
        lfs_file_read(&lfs, 
                        &file, 
                        reinterpret_cast<void *>(buff.data()), 
                        buff.size() - 1);
        printf("This is content from the file\r\n%s\r\n", buff.data());
        lfs_file_close(&lfs, &file);
    }

    while(true)
    {
    }
}
