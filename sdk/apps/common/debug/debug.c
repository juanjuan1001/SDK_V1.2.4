#include "app_config.h"
#include "typedef.h"

#ifndef CONFIG_DEBUG_ENABLE

int putchar(int a)
{
    return a;
}

int puts(const char *out)
{
    return 0;
}

int printf(const char *format, ...)
{
    return 0;
}

void printf_buf(u8 *buf, u32 len)
{

}

void put_buf(const u8 *buf, int len)
{

}

void put_u8hex(u8 dat)
{

}

void put_u16hex(u16 dat)
{

}

void put_u32hex(u32 dat)
{

}

int assert_printf(const char *format, ...)
{
    cpu_assert_debug();
    return 0;
}

#endif

