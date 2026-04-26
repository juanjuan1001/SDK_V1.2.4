#include "asm/power_interface.h"
#include "app_config.h"
#include "power_api.h"
/* #include "audio.h" */
#include "gpio.h"
#include "asm/hadc.h"
#include "usb/device/usb_stack.h"

#include "rtc/rtc_dev.h"

#define LOG_TAG_CONST       PMU
#define LOG_TAG             "[PMU]"
#include "debug.h"

#define P33_IO_WKUP_IOMAP			POWER_WAKEUP_IO
#define P33_IO_WKUP_EDGE			POWER_WAKEUP_EDGE
#define P33_IO_LONG_PRESS_RESET	    PINR_DEFAULT_IO
#define VDDIOM_VOL_CONFIG			TCFG_VDDIOM_LEVEL
#define FLASH_POWER_GATE_VDDIO      0                   //flash power接vddio时配置为1，内封flash配置为0

/**************************************************** power_param *********************************************************/
const struct low_power_param power_param = {
    //sniff时芯片是否进入低功耗
    .config         = TCFG_LOWPOWER_LOWPOWER_SEL,
    //外接晶振频率
    .btosc_hz       = 24000000,
    .vddiom_lev     = VDDIOM_VOL_CONFIG,
    .vddiow_lev     = 0,
    .osc_type       = OSC_TYPE_LRC,
};

/**************************************************** p33_io_wkup_param *********************************************************/
const struct p33_io_wkup_config port0 = {
    .pullup_down_enable = 1,                          //配置I/O 内部上下拉是否使能
    .filter_enable = 1,
    .edge       = P33_IO_WKUP_EDGE,                  //唤醒方式选择,可选：上升沿\下降沿
    .iomap      = P33_IO_WKUP_IOMAP,                    //唤醒口选择
};

const struct p33_io_wkup_param wkup_param = {
    .filter = PORT_FLT_4ms,
#if P33_IO_WKUP_IOMAP != NO_CONFIG_PORT
    .port[1] = &port0,
#endif
};

void sleep_exit_callback()
{
    usb_iomode(0);
    putchar('>');
}

void sleep_enter_callback()
{
    usb_iomode(1);
    putchar('<');
}

__attribute__((weak))
u8 get_soft_save_data(void)
{
    return 0;
}


static void __mask_io_cfg()
{

    u32 flash_power_keep_vddio = FLASH_POWER_GATE_VDDIO;
    struct boot_soft_flag_t boot_soft_flag = {0};
    /* boot_soft_flag.flag0.boot_ctrl.flash_stable_delay_sel = ~(flash_power_keep_vddio);   //0: 0mS;   1: 4mS */
    /* boot_soft_flag.flag0.boot_ctrl.sfc_flash_stable_delay_sel = ~(flash_power_keep_vddio); //0: 0.5mS; 1: 1mS */
    boot_soft_flag.flag0.boot_ctrl.flash_stable_delay_sel = 0;   //0: 0mS;   1: 4mS
    boot_soft_flag.flag0.boot_ctrl.sfc_flash_stable_delay_sel = 0; //0: 0.5mS; 1: 1mS

    boot_soft_flag.flag1.boot_ctrl.usbdm = SOFTFLAG_HIGH_RESISTANCE;
    boot_soft_flag.flag1.boot_ctrl.usbdp = SOFTFLAG_HIGH_RESISTANCE;

    boot_soft_flag.flag2.boot_ctrl.avddcp_short_en = hadc_soft_info.avddcp_short_en;
    boot_soft_flag.flag2.boot_ctrl.avddcp_vol_sel  = hadc_soft_info.avddcp_vol_sel;
    boot_soft_flag.flag2.boot_ctrl.avddcp_delay_sel = hadc_soft_info.avddcp_delay_sel;
    boot_soft_flag.flag2.boot_ctrl.fast_adc        = hadc_soft_info.fast_adc;
    boot_soft_flag.flag2.boot_ctrl.avddcp_hadc_sel = hadc_soft_info.avddcp_hadc_sel;

    boot_soft_flag.flag3.boot_ctrl.avddcp_clktune_sel = hadc_soft_info.avddcp_clktune_sel;
    boot_soft_flag.flag3.boot_ctrl.avddcp_clkdiv_sel = hadc_soft_info.avddcp_clkdiv_sel;
    boot_soft_flag.flag3.boot_ctrl.avddr_vol_sel     = hadc_soft_info.avddr_vol_sel;

    boot_soft_flag.flag7_4.boot_ctrl.avddr_ref_sel  = hadc_soft_info.avddr_ref_sel;
    boot_soft_flag.flag7_4.boot_ctrl.lrc_pll_ds_cfg = JL_PLL0->NR;
    boot_soft_flag.flag7_4.boot_ctrl.res = get_soft_save_data();

    u32 sfc_fast_boot = TCFG_SFC_FAST_BOOT_ENABLE;
    mask_softflag_config(&boot_soft_flag, sfc_fast_boot);
}

void board_set_soft_poweroff()
{
    /* audio_off(); */
    rtc_save_context_to_vm();

    u32 gpio_config[6] = {0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff};
    u32 usbio_config = 0x3;
    /* #define PORT_PROTECT(gpio)	gpio_config[gpio/IO_GROUP_NUM] &= ~BIT(gpio%IO_GROUP_NUM) */
#define USB_PROTECT(gpio)   usbio_config &= ~BIT(gpio-IO_PORT_DP)

    PORT_PROTECT(P33_IO_WKUP_IOMAP);

    if (is_mclr_en()) {
        PORT_PROTECT(MCLR_PORT);
    }

    //长按复位
    if (is_pinr_en()) {
        u8 port_sel = get_pinr_port();
        if ((port_sel >= PA0_IN) && (port_sel <= PA12_IN)) {
            PORT_PROTECT(IO_PORTA_00 + (port_sel - PA0_IN));
        } else if ((port_sel >= PC0_IN) && (port_sel <= PC15_IN)) {
            PORT_PROTECT(IO_PORTC_00 + (port_sel - PC0_IN));
        } else if ((port_sel >= PD0_IN) && (port_sel <= PD15_IN)) {
            PORT_PROTECT(IO_PORTD_00 + (port_sel - PD0_IN));
        } else if (port_sel == USBDP_IN) {
            USB_PROTECT(IO_PORT_DP);
        } else if (port_sel == USBDM_IN) {
            USB_PROTECT(IO_PORT_DM);
        }
    }

    //FLASH
    u32 read_mode = get_sfc_bit_mode();
    PORT_PROTECT(SPI0_CS_A);
    PORT_PROTECT(SPI0_CLK_A);
    PORT_PROTECT(SPI0_DO_D0_A);
    if (read_mode != 0) {
        PORT_PROTECT(SPI0_DI_D1_A);
        if (read_mode == 4) {
            /* PORT_PROTECT(SPI0_WP_D2_A); */
            /* PORT_PROTECT(SPI0_HOLD_D3_A); */
        }
    }

    __mask_io_cfg();

    gpio_close(JL_PORTA, gpio_config[0]);
    gpio_close(JL_PORTC, gpio_config[2]);
    gpio_close(JL_PORTD, gpio_config[3]);
    gpio_close(JL_PORTF, gpio_config[5]);
    if (usbio_config & BIT(0)) {
        gpio_set_mode(PORTUSB, BIT(0), PORT_HIGHZ);
    }

    if (usbio_config & BIT(1)) {
        gpio_set_mode(PORTUSB, BIT(1), PORT_HIGHZ);
    }


}



void sys_power_init()
{
    /* power_config_vdc13_cap(0); */
    /* power_config_wvdd_lev(WLDO_LEVEL_050V); */
    power_config_flash_pg_vddio(FLASH_POWER_GATE_VDDIO);
    power_config_sf_vddio_keep(VDDIO_KEEP_TYPE_NORMAL);
    power_config_pd_vddio_keep(VDDIO_KEEP_TYPE_NORMAL);

    power_init(&power_param);

    volatage_trim_init();

    power_wakeup_init(&wkup_param);

    soff_latch_release();

    /* while (1) { */
    /* sys_power_down(3000000); */
    /* } */

    /* printf("\nSOFTOFF\n"); */
    /* power_set_soft_poweroff(); */
}
