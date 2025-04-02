#include <cstdio>

// Stub DWT_CYCCNT register
#define DWT_CYCCNT 100

extern "C"
{
    __attribute__((no_instrument_function)) void __cyg_profile_func_enter(void *this_fn, void *call_site)
    {

        printf("entry, %p, %d\n", this_fn, DWT_CYCCNT);
    }

    __attribute__((no_instrument_function)) void __cyg_profile_func_exit(void *this_fn, void *call_site)
    {

        printf("entry, %p, %d\n", this_fn, DWT_CYCCNT);
    }
}

void fun()
{
    printf("Hi from fun!\n");
}

int main()
{
    fun();
    printf("Hi from main!\n");

    return 0;
}