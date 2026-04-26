#include "asm/norflash.h"
#include "app_config.h"

#if defined(TCFG_NORFLASH_DEV_ENABLE) && TCFG_NORFLASH_DEV_ENABLE

//外置flash读写demo
static u8 flash_buffer[512];
void ext_norflash_test_demo(void)
{
    u32 addr, len;
    void *dev_hdl;
#if 1//使用norflash_dev_ops的按块读写例子
    addr = 0 / 512;
    len = sizeof(flash_buffer) / 512;
    dev_hdl = dev_open(__EXT_FLASH_BULK, NULL);
    if (dev_hdl == NULL) {
        return;
    }
    memset(flash_buffer, addr, len);
    dev_bulk_read(dev_hdl, flash_buffer, addr, len);
    printf("\nread data-1:");
    printf_buf(flash_buffer, sizeof(flash_buffer));

    memset(flash_buffer, 0xa7, sizeof(flash_buffer));
    dev_bulk_write(dev_hdl, flash_buffer, addr, len);

    dev_ioctl(dev_hdl, IOCTL_FLUSH, 0);//立即更新数据到flash

    dev_bulk_read(dev_hdl, flash_buffer, addr, len);
    printf("\nread data-2:");
    printf_buf(flash_buffer, sizeof(flash_buffer));

    dev_close(dev_hdl);
#endif

#if 1//使用norfs_dev_ops的按byte读写的例子
    addr = 0;
    len = sizeof(flash_buffer);
    dev_hdl = dev_open(__EXT_FLASH_BYTE, NULL);
    if (dev_hdl == NULL) {
        return;
    }
    memset(flash_buffer, 0, sizeof(flash_buffer));
    dev_byte_read(dev_hdl, flash_buffer, addr, len);
    printf("\nread data-3:");
    printf_buf(flash_buffer, sizeof(flash_buffer));

    memset(flash_buffer, 0xa5, sizeof(flash_buffer));
    dev_byte_write(dev_hdl, flash_buffer, addr, len);

    dev_ioctl(dev_hdl, IOCTL_FLUSH, 0);//立即更新数据到flash

    dev_byte_read(dev_hdl, flash_buffer, addr, len);
    printf("\nread data-4:");
    printf_buf(flash_buffer, sizeof(flash_buffer));

    dev_close(dev_hdl);
#endif
}

#endif
