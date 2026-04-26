#include "system/includes.h"
#include "asm/norflash.h"
#include "asm/sfc_norflash_api.h"
#include "app_main.h"
#include "app_config.h"

#if TCFG_NORFLASH_DEV_ENABLE
static struct spi_platform_data spi2_p_data = {
    .port = {
        TCFG_HW_SPI2_PORT_CLK,
        TCFG_HW_SPI2_PORT_DO,
        TCFG_HW_SPI2_PORT_DI,
        0xff,//d2
        0xff,//d3
        0xff,//cs
    },
    .role = TCFG_HW_SPI2_ROLE,
    .mode = TCFG_HW_SPI2_MODE,
    .clk  = TCFG_HW_SPI2_BAUD,
    .bit_mode = SPI_FIRST_BIT_MSB,
    .cpol = 0,//clk level in idle state:0:low,  1:high
    .cpha = 0,//sampling edge:0:first,  1:second
};

static struct norflash_dev_platform_data norflash_dev_data = {
    .spi_hw_num     = HW_SPI2,
    .spi_cs_port    = TCFG_HW_SPI2_PORT_CS,
    .spi_pdata      = &spi2_p_data,
    .start_addr     = 0,
    .size           = 1 * 1024 * 1024,
};

static struct norflash_dev_platform_data norfs_dev_data = {
    .spi_hw_num     = HW_SPI2,
    .spi_cs_port    = TCFG_HW_SPI2_PORT_CS,
    .spi_pdata      = &spi2_p_data,
    .start_addr     = 1 * 1024 * 1024,
    .size           = 1 * 1024 * 1024,
};
#endif


#if TCFG_MSFC_DEV_ENABLE
static struct msfc_dev_platform_data msfc_dev_data = {
    .start_addr     = TCFG_MSFC_DEV_ADDR * 1024,
    .size           = TCFG_MSFC_DEV_SIZE * 1024,
};

#endif

REGISTER_DEVICES(device_table) = {
#if TCFG_NORFLASH_DEV_ENABLE
    //如需按512byte读写的,选用下面的ops
    {.name = __EXT_FLASH_BULK, .ops = &norflash_dev_ops, .priv_data = (void *) &norflash_dev_data},
    //如需要按1byte读写的,选用下面的ops
    {.name = __EXT_FLASH_BYTE, .ops = &norfs_dev_ops, .priv_data = (void *) &norfs_dev_data},
#endif
#if TCFG_MSFC_DEV_ENABLE
    {.name = __MSFC_NAME, .ops = &msfc_dev_ops, .priv_data = (void *) &msfc_dev_data},
#endif
};



