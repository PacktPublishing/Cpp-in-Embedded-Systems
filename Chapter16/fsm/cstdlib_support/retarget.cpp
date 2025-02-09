#include <retarget.hpp>

#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#include <span>

namespace 
{
hal::uart *uart_stdio;
};

void retarget::set_stdio_uart(hal::uart *uart)
{
    uart_stdio = uart;

    /* Disable I/O buffering for STDOUT stream, so that
    * chars are sent out as soon as they are printed. */
    setvbuf(stdout, NULL, _IONBF, 0);
}

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

extern "C" int _write(int fd, char * ptr, int len)
{
    if(fd == STDOUT_FILENO || fd == STDERR_FILENO)
    {
        uart_stdio->write(std::span<const char>(ptr, len));
    }

    return len;
}

extern "C" int _isatty(int fd)
{
    if(fd >= STDIN_FILENO && fd <= STDERR_FILENO)
    {
        return 1;
    }
    errno = EBADF;
    return 0;
}

extern "C" int _close(int fd)
{
        if(fd >= STDIN_FILENO && fd <= STDERR_FILENO)
                return 0;

        errno = EBADF;
        return -1;
}

extern "C" int _lseek(int fd, int ptr, int dir)
{
        (void)fd;
        (void)ptr;
        (void)dir;

        errno = EBADF;
        return -1;
}

extern "C" int _read(int fd, char *ptr, int len)
{
    return -1;
}

int _fstat(int fd, struct stat *st)
{
        if(fd >= STDIN_FILENO && fd <= STDERR_FILENO)
        {
                st->st_mode = S_IFCHR;
                return 0;
        }

        errno = EBADF;
        return 0;
}
