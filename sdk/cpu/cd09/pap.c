#include "asm/pap.h"
#include "gpio.h"
/* #include "system/generic/log.h" */
#include "asm/clock.h"
#include "asm/cpu.h"
/* #include "update.h" */


#undef LOG_TAG_CONST
#define LOG_TAG     "[pap]"
#include "debug.h"

//PAP_IO:
//      PAP_RD:PD1
//      PAP_WR:PD0
//      PAPD0~PAPD7: PC8~PC15
//PAP_IOMAP:
//      PAP_DEN:IOMAPCON0 bit13(Dx)
//      PAP_REN:IOMAPCON0 bit12(RD)
//      PAP_WEN:IOMAPCON0 bit11(WR)

static JL_PAP_TypeDef *const pap_regs[PAP_HW_NUM] = {
    JL_PAP,
};
/* #define pap_info_hdrive(pap)    (hw_pap_cfg[pap_get_id(pap)].hdrive) */
/* #define pap_info_io_pu(pap)     (hw_pap_cfg[pap_get_id(pap)].io_pu) */
extern void mdelay(u32 ms);
void pap_port_dir_set(u8 dx_dir)//pap d0~d7 输入输出状态
{
    gpio_set_mode(PORTC, 0xff00, dx_dir);//PC8~PC15
}

static int pap_port_init(hw_pap_dev pap)
{
    u32 reg;
    int ret = 0;
    if (pap == 0) {
        JL_IOMC->IOMC0 |= BIT(13);//PAP_Dx
        JL_IOMC->IOMC0 |= BIT(12);//PAP_RD
        JL_IOMC->IOMC0 |= BIT(11);//PAP_WR
        /* gpio_set_die(IO_PORTD_01, 1); */
        /* gpio_set_die(IO_PORTD_00, 1); */


        gpio_set_mode(PORTD, PORT_PIN_0 | PORT_PIN_1, PORT_INPUT_FLOATING);//PD0,PD1
        gpio_set_mode(PORTC, 0xff00, PORT_INPUT_PULLDOWN_10K);//PC8~PC15//pap d0~d7 输入输出状态
        gpio_set_mode(PORTD, PORT_PIN_0 | PORT_PIN_1, PORT_OUTPUT_LOW);//PD0,PD1
        gpio_set_mode(PORTC, 0xff00, PORT_OUTPUT_LOW);//pap d0~d7 输入输出状态
        /* if (pap_info_hdrive(pap)) { */
        /*     gpio_set_hd(scl, 1); */
        /*     gpio_set_hd(sda, 1); */
        /* } else { */
        /*     gpio_set_hd(scl, 0); */
        /*     gpio_set_hd(sda, 0); */
        /* } */
        /* if (pap_info_io_pu(pap)) { */
        /*     gpio_set_pull_up(scl, 1); */
        /*     gpio_set_pull_up(sda, 1); */
        /* } else { */
        /*     gpio_set_pull_up(scl, 0); */
        /*     gpio_set_pull_up(sda, 0); */
        /* } */
    } else {
        ret = -1;
    }
    return ret;
}

int hw_pap_init(hw_pap_dev pap)
{
    int ret;
    ret = pap_port_init(pap);
    if (ret < 0) {
        log_error("invalid hardware pap port\n");
        return ret;
    }
    pap_write_en_polarity_h(pap_regs[pap]);//write 0
    pap_read_en_polarity_h(pap_regs[pap]);//read 1
    pap_data_width_8bit(pap_regs[pap]);//data width 8
    /* pap_data_width_16bit(pap_regs[pap]);//data width 16,only 8bit */
    pap_data_little_endian(pap_regs[pap]);//小端
    /* pap_data_big_endian(pap_regs[pap]);//大端 波形异常 */
    pap_rw_en_time_width(pap_regs[pap], 0); //TW:x(0~15) system clock width(x=0:16 system clock width)
    pap_data_hold_time_width(pap_regs[pap], 3); //TH:>=x(0~3) system clock width
    pap_data_build_time_width(pap_regs[pap], 3); //TS:x(0~3) system clock width
    pap_data_ext_disable(pap_regs[pap]);
    /* pap_set_ie(pap_regs[pap]);//ie */
    pap_pnd_clr(pap_regs[pap]);
    pap_enable(pap_regs[pap]);
#if 0
    printf("PAP_CON 0x%04x\n", pap_regs[pap]->CON);
    printf("IOMC0[bit11~13] 0x%08x\n", JL_IOMC->IOMC0);
#endif
    return 0;
}

void hw_pap_uninit(hw_pap_dev pap)
{
    if (pap == 0) {
        JL_IOMC->IOMC0 &= ~ BIT(13); //PAP_Dx
        JL_IOMC->IOMC0 &= ~ BIT(12); //PAP_RD
        JL_IOMC->IOMC0 &= ~ BIT(11); //PAP_WR
        gpio_set_mode(PORTD, PORT_PIN_0 | PORT_PIN_1, PORT_HIGHZ);//PD0,PD1
        gpio_set_mode(PORTC, 0xff00, PORT_HIGHZ);//PC8~PC15//pap d0~d7 输入输出状态
        pap_disable(pap_regs[pap]);
    }
}

u8 hw_pap_tx_byte(hw_pap_dev pap, u16 byte)//支持扩展模式
{
    gpio_set_mode(PORTC, 0xff00, PORT_OUTPUT_LOW);//PC8~PC15//pap d0~d7 输入输出状态
    pap_dir_out(pap_regs[pap]);
    pap_buf_reg(pap_regs[pap]) = byte;
    /* putchar('a'); */
    while (!pap_pnd(pap_regs[pap]));
    pap_pnd_clr(pap_regs[pap]);
    /* putchar('b'); */
    return 1;//ok
}

u16 hw_pap_rx_byte(hw_pap_dev pap)//不支持扩展模式
{
    pap_data_ext_disable(pap_regs[pap]);
    gpio_set_mode(PORTC, 0xff00, PORT_INPUT_PULLDOWN_10K);//PC8~PC15//pap d0~d7 输入输出状态
    pap_dir_in(pap_regs[pap]);
    pap_buf_reg(pap_regs[pap]) = 0xff;
    /* putchar('c'); */
    while (!pap_pnd(pap_regs[pap]));
    pap_pnd_clr(pap_regs[pap]);
    /* putchar('d'); */
    return pap_buf_reg(pap_regs[pap]);
}

int hw_pap_read_buf(hw_pap_dev pap, void *buf, int len)
{
    if (!buf || !len) {
        return -1;
    }
    pap_data_ext_disable(pap_regs[pap]);
    gpio_set_mode(PORTC, 0xff00, PORT_INPUT_PULLDOWN_10K);//PC8~PC15//pap d0~d7 输入输出状态
    pap_dir_in(pap_regs[pap]);
    pap_dma_addr_reg(pap_regs[pap]) = (u32)buf;
    pap_dma_cnt_reg(pap_regs[pap]) = len;
    while (!pap_pnd(pap_regs[pap]));
    pap_pnd_clr(pap_regs[pap]);
    return len;
}

int hw_pap_write_buf(hw_pap_dev pap, const void *buf, int len)
{
    if (!buf || !len) {
        return -1;
    }
    gpio_set_mode(PORTC, 0xff00, PORT_OUTPUT_LOW);//PC8~PC15//pap d0~d7 输入输出状态
    pap_dir_out(pap_regs[pap]);
    pap_dma_addr_reg(pap_regs[pap]) = (u32)buf;
    pap_dma_cnt_reg(pap_regs[pap]) = len;
    while (!pap_pnd(pap_regs[pap]));
    pap_pnd_clr(pap_regs[pap]);
    return len;
}

void hw_pap_set_data_reg(hw_pap_dev pap, u16 data0, u16 data1)
{
    pap_ext_data0_reg(pap_regs[pap]) = data0;
    pap_ext_data1_reg(pap_regs[pap]) = data1;
}

void hw_pap_set_ie(hw_pap_dev pap, u8 en)
{
    if (en) {
        pap_set_ie(pap_regs[pap]);
    } else {
        pap_clr_ie(pap_regs[pap]);
    }
}

u8 hw_pap_get_pnd(hw_pap_dev pap)
{
    return !!pap_pnd(pap_regs[pap]);
}

void hw_pap_clr_pnd(hw_pap_dev pap)
{
    pap_pnd_clr(pap_regs[pap]);
}

/* void pap_disable_for_ota() */
/* { */
/*     JL_PAP->CON = 0; */
/* } */
/*  */
/* REGISTER_UPDATE_TARGET(pap_update_target) = { */
/*     .name = "pap", */
/*     .driver_close = pap_disable_for_ota, */
/* }; */



/**************************test***************************/
#if 0
void pap_test()
{
    u16 pap_tx_byte = 0, pap_rx_byte = 0;
    hw_pap_set_data_reg(0, 0x5555, 0xaaaa);
    hw_pap_init(0);
    gpio_set_direction(IO_PORTA_08, GPIO_MODE_OUTPUT);
    gpio_write(IO_PORTA_08, 0);
#if 0//写(tx)
#if 1//byte写(tx)
    pap_tx_byte = 0x25;
    hw_pap_tx_byte(0, pap_tx_byte);//支持扩展模式
    hw_pap_tx_byte(0, 0x52);//支持扩展模式
    hw_pap_tx_byte(0, 0x88);//支持扩展模式
    hw_pap_tx_byte(0, 0x44);//支持扩展模式
    hw_pap_tx_byte(0, 0x22);//支持扩展模式
    hw_pap_tx_byte(0, 0x11);//支持扩展模式
#else//dma写(tx)
    u8 pap_dma_tx_buf[100], ii = 0;
    for (ii = 0; ii < sizeof(pap_dma_tx_buf); ii++) {
        pap_dma_tx_buf[ii] = 1 << (ii % 8);
    }
    hw_pap_write_buf(0, pap_dma_tx_buf, sizeof(pap_dma_tx_buf));
#endif//写(tx)
#else//读(rx)
#if 1//byte读(rx)

    pap_rx_byte = hw_pap_rx_byte(0); //不支持扩展模式
    log_info("pap_rx_byte:0x%x", pap_rx_byte);
    mdelay(700);
    pap_rx_byte = 0;
    pap_rx_byte = hw_pap_rx_byte(0); //不支持扩展模式
    log_info("pap_rx_byte:0x%x", pap_rx_byte);
#else//dma读(rx)
    u8 pap_dma_rx_buf[100] = {0};
    hw_pap_read_buf(0, pap_dma_rx_buf, sizeof(pap_dma_rx_buf));
    log_info_hexdump(pap_dma_rx_buf, sizeof(pap_dma_rx_buf));
#endif//读(rx)

#endif//读(rx)
}
#endif//test

