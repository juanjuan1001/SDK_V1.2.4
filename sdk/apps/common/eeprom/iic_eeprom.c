#include "includes.h"
#include "iic_eeprom.h"

/* #define LOG_TAG_CONST       EEPROM */
#define LOG_TAG             "[eeprom]"
#include "debug.h"


#if 1 //0:软件iic,  1:硬件iic
#define _IIC_USE_HW
#endif

#include "iic_api.h"

#define EEPROM_RADDR        0xa1
#define EEPROM_WADDR        0xa0

#define DELAY_CNT           0


extern void udelay(u32 us);
void eeprom_write(int iic, u8 *buf, u32 addr, u32 len)
{
    int i;
    u32 retry;
    int ret;
    u32 tx_len;
    u32 offset;
#if 1
    offset = 0;
    while (offset < len) {
        tx_len = len - offset > 8 ? 8 : len - offset;
        retry = 100;
        do {
            iic_start(iic);
            ret = iic_tx_byte(iic, EEPROM_WADDR);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            udelay(DELAY_CNT);
            ret = iic_tx_byte(iic, addr + offset);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            udelay(DELAY_CNT);
            /* putchar('h'); */
            for (i = 0; i < tx_len - 1; i++) {
                ret = iic_tx_byte(iic, buf[offset + i]);
                if (!ret) {
                    if (--retry) {
                        continue;
                    } else {
                        goto __exit;
                    }
                }
                udelay(DELAY_CNT);
            }
            /* putchar('i'); */
            ret = iic_tx_byte(iic, buf[offset + tx_len - 1]);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            /* putchar('j'); */
            iic_stop(iic);
            udelay(DELAY_CNT);
        } while (0);
        offset += tx_len;
        do {
            iic_start(iic);
            ret = iic_tx_byte(iic, EEPROM_WADDR);
            if (ret) {
                break;
            }
            udelay(DELAY_CNT);
        } while (1);
    }
__exit:
    iic_stop(iic);
#else
    offset = 0;
    while (offset < len) {
        retry = 100;
        tx_len = len - offset > 8 ? 8 : len - offset;
        do {
            iic_start(iic);
            ret = iic_tx_byte(iic, EEPROM_WADDR);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            ret = iic_tx_byte(iic, addr + offset);
            if (!ret) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            /* putchar('h'); */
            ret = iic_write_buf(iic, buf + offset, tx_len);
            if (ret < tx_len) {
                if (--retry) {
                    continue;
                } else {
                    goto __exit;
                }
            }
            /* putchar('i'); */
            iic_stop(iic);
            udelay(DELAY_CNT);
        } while (0);
        offset += tx_len;
        do {
            iic_start(iic);
            ret = iic_tx_byte(iic, EEPROM_WADDR);
            if (ret) {
                break;
            }
            udelay(DELAY_CNT);
        } while (1);
        /* putchar('j'); */
    }
__exit:
    iic_stop(iic);
#endif
}

void eeprom_read(int iic, u8 *buf, u32 addr, u32 len)
{
    int i;
    u32 retry = 100;
    int ret;
#if 1
    do {
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_WADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        udelay(DELAY_CNT);
        ret = iic_tx_byte(iic, addr);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        udelay(DELAY_CNT);
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_RADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        udelay(DELAY_CNT);
        /* putchar('k'); */
        for (i = 0; i < len - 1; i++) {
            buf[i] = iic_rx_byte(iic, 1);
            udelay(DELAY_CNT);
        }
        /* putchar('l'); */
        buf[len - 1] = iic_rx_byte(iic, 0);
        iic_stop(iic);
        udelay(DELAY_CNT);
        /* putchar('m'); */
    } while (0);
#else
    do {
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_WADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        ret = iic_tx_byte(iic, addr);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        iic_start(iic);
        ret = iic_tx_byte(iic, EEPROM_RADDR);
        if (!ret) {
            if (--retry) {
                continue;
            } else {
                break;
            }
        }
        iic_read_buf(iic, buf, len);
        iic_stop(iic);
    } while (0);
#endif
}

#if 0
#define iic_dev 0
#define IIC_SCL_IO IO_PORTC_04
#define IIC_SDA_IO IO_PORTC_03
static u8 eeprom_rbuf[256], eeprom_wbuf[256];
void eeprom_test_main()
{
    int i = 0;
    u8 flag = 0;
    struct iic_master_config iic_config_test = {
        .role = IIC_MASTER,
        .scl_io = IIC_SCL_IO,
        .sda_io = IIC_SDA_IO,
        .io_mode = PORT_INPUT_PULLUP_10K,//上拉或浮空
        .hdrive = PORT_DRIVE_STRENGT_2p4mA,   //enum GPIO_HDRIVE 0:2.4MA, 1:8MA, 2:26.4MA, 3:40MA
        .master_frequency = 100000, //软件iic频率不准(hz)
        .io_filter = 1,  //软件无效
    };
#ifdef _IIC_USE_HW
    log_info("-------hw_iic%d-------\n", iic_dev);
#else
    log_info("-------soft_iic%d-------\n", iic_dev);
#endif
    iic_init(iic_dev, &iic_config_test);
    for (i = 0; i < 256; i++) {
        eeprom_wbuf[i] = i % 26 + 'A';
        eeprom_rbuf[i] = 0;
    }
    puts(">>>> write in\n");
    eeprom_write(iic_dev, eeprom_wbuf, 0, 128);
    puts("<<<< write out\n");
    puts(">>>> read in\n");
    eeprom_read(iic_dev, eeprom_rbuf, 0, 128);
    puts("<<<< read out\n");

    for (i = 0; i < 128; i++) {
        if (eeprom_wbuf[i] != eeprom_rbuf[i]) {
            flag = 1;
            break;
        }
    }
    for (i = 0; i < 128; i++) {
        putchar(eeprom_rbuf[i]);
        putchar(' ');
        if (i % 16 == 15) {
            putchar('\n');
        }
    }
    putchar('\n');
    if (flag == 0) {
        puts("eeprom read/write test pass\n");
    } else {
        puts("eeprom read/write test fail\n");
    }
    iic_deinit(iic_dev);
}
#endif

