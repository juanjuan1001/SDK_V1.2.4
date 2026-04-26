#ifndef _NORFLASH_H
#define _NORFLASH_H

#include "system/includes.h"
#include "ioctl_cmds.h"
#include "spi.h"
#include "printf.h"
#include "gpio.h"
#include "malloc.h"

#include "generic/jiffies.h"

static inline u32 spi_get_jiffies(void)
{
#if 1
    return jiffies;
#endif
#if 0
    return Jtime_updata_jiffies();
#endif
}
static inline u32 spi_msecs_to_jiffies(u32 msecs)
{
    if (msecs >= 10) {
        msecs /= 10;
    } else if (msecs) {
        msecs = 1;
    }
    return msecs;
}

typedef volatile u32 SPI_mutex;
static inline int SPI_OSMutexCreate(SPI_mutex *mutex)
{
    *mutex = 1;
    return 0;
}
static inline int SPI_OSMutexPost(SPI_mutex *mutex)
{
    (*mutex) = 1;
    return 0;
}
static inline int SPI_OSMutexPend(SPI_mutex *mutex, u32 timeout)
{
    if (!timeout) {
        timeout = spi_msecs_to_jiffies(10);
    } else {
        timeout = spi_msecs_to_jiffies(timeout);
    }
    u32 _timeout = timeout + spi_get_jiffies();
    while (1) {
        if (*mutex) {
            (*mutex) = 0;
            break;
        }
        if ((timeout != 0) && time_before(_timeout, spi_get_jiffies())) {
            return -1;
        }
        wdt_clear();
    }
    return 0;
}
static inline int UT_OSMutexClose(SPI_mutex *mutex, u32 block)
{
    return 0;
}

/*************************************************/
/*
   COMMAND LIST - WinBond FLASH WX25X
 */
/***************************************************************/
#define WINBOND_WRITE_ENABLE		                        0x06
#define WINBOND_READ_SR1			                        0x05
#define WINBOND_READ_SR2			                        0x35
#define WINBOND_WRITE_SR1			                        0x01
#define WINBOND_WRITE_SR2			                        0x31
#define WINBOND_READ_DATA		                            0x03
#define WINBOND_FAST_READ_DATA		                        0x0b
#define WINBOND_FAST_READ_DUAL_OUTPUT                       0x3b
#define WINBOND_PAGE_PROGRAM	                            0x02
#define WINBOND_PAGE_ERASE                                  0x81
#define WINBOND_SECTOR_ERASE		                        0x20
#define WINBOND_BLOCK_ERASE		                            0xD8
#define WINBOND_CHIP_ERASE		                            0xC7
#define WINBOND_JEDEC_ID                                    0x9F

enum {
    FLASH_PAGE_ERASER,
    FLASH_SECTOR_ERASER,
    FLASH_BLOCK_ERASER,
    FLASH_CHIP_ERASER,
};

struct norflash_dev_platform_data {
    int spi_hw_num;         //只支持SPI1或SPI2
    u32 spi_cs_port;        //cs的引脚
    u32 spi_read_width;     //flash读数据的线宽
    struct spi_platform_data *spi_pdata;
    u32 start_addr;         //分区起始地址
    u32 size;               //分区大小，若只有1个分区，则这个参数可以忽略
};

#if 0 //使用device驱动访问flash,不用下面接口
int ext_norflash_init(const char *name, struct norflash_dev_platform_data *pdata);

int ext_norflash_open(const char *name);

int ext_norflash_close(const char *name);

int ext_norflash_read(const char *name, void *buf, u32 len, u32 offset);

int ext_norflash_write(const char *name, void *buf, u32 len, u32 offset);

int ext_norflash_ioctl(const char *name, u32 cmd, u32 arg);
#endif

#endif

