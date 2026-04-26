#include "asm/includes.h"
#include "asm/tick_timer.h"
#include "asm/efuse.h"
#include "app_config.h"
#include "asm/clock_define.h"
#include "asm/clock.h"
#include "asm/hadc.h"
#include "boot.h"
#include "timer.h"
#include "event.h"
#include "asm/sys_debug.h"
#include "sdfile.h"
#include "vm.h"
#include "asm/sfc_norflash_api.h"
#include "power/power_api.h"
#include "app_main.h"

#define LOG_TAG_CONST       SETUP
#define LOG_TAG             "[SETUP]"
#include "debug.h"

u32 stack_magic[4] AT(.stack_magic);
u32 stack_magic0[4] AT(.stack_magic0);
BOOT_INFO boot_info AT(.boot_info);

//注册中断时,在该表上查到对应中断优先级,则使用表里面配置的,否则使用注册接口传入的优先级参数
//中断优先级设置0 ~7
static const struct irq_info irq_index_2_priority_table[] = {
    {IRQ_TICK_TMR_IDX,          1},
    {IRQ_TIME0_IDX,             0},
    {IRQ_TIME1_IDX,             0},
    {IRQ_TIME2_IDX,             0},
    {IRQ_TIME3_IDX,             0},
    {IRQ_UART0_IDX,             0},
    {IRQ_UART1_IDX,             0},
    {IRQ_UART2_IDX,             0},
    {IRQ_SPI0_IDX,              3},
    {IRQ_SPI1_IDX,              3},
    {IRQ_SPI2_IDX,              3},
    {IRQ_IIC0_IDX,              0},
    {IRQ_IIC1_IDX,              0},
    {IRQ_PORT_IDX,              3},
    {IRQ_SARADC_IDX,            0},
    {IRQ_GPCNT_IDX,             0},
    {IRQ_USB_SOF_IDX,           3},
    {IRQ_USB_CTRL_IDX,          3},
    {IRQ_MCPWM_TMR_IDX,         0},
    {IRQ_PMU_SOFT_IDX,          0},
    {IRQ_PMU_TIMER0_IDX,        0},
    {IRQ_PMU_TIMER1_IDX,        0},
    {IRQ_QDEC_IDX,              0},
    {IRQ_PAP_IDX,               0},
    {IRQ_LCD_IDX,               0},
    {IRQ_MCPWM_CHX_IDX,         0},
    {IRQ_DMA_GEN0_IDX,          0},
    {IRQ_DMA_GEN1_IDX,          0},
    {IRQ_DMA0_IDX,              0},
    {IRQ_DMA1_IDX,              0},
    {IRQ_AUDIO_IDX,             6},
    {IRQ_APA_IDX,               6},
    {IRQ_HADC_IDX,              0},
    {IRQ_SOFT0_IDX,             0},
    {IRQ_SOFT1_IDX,             0},
    {IRQ_SOFT2_IDX,             0},
    {IRQ_SOFT3_IDX,             0},
};

_WEAK_
void fast_boot_for_app(void)
{
}

_WEAK_
void app_early_init(void)
{
}

void boot_info_init(void *_info)
{
    BOOT_DEVICE_INFO *info = (BOOT_DEVICE_INFO *)_info;
    memcpy(&(boot_info.sfc), &(info->sfc), sizeof(struct sfc_info));
    boot_info.flash_size = info->fs_info->FlashSize;
    boot_info.vm.align = info->fs_info->align;
    boot_info.chip_id = info->chip_id;
    //必须在时钟初始化前调用,避免时钟初始化时改变DCVDD电压,导致AVBG电压不对
    hadc_avbg_init_for_fast_boot();
    fast_boot_mode_clk();
    fast_boot_for_app();
}

u32 boot_info_get_sfc_base_addr(void)
{
    return boot_info.sfc.sfc_base_addr;
}

void maskrom_init(void)
{
    struct maskrom_argv argv;
    memset((void *)&argv, 0, sizeof(struct maskrom_argv));
    argv.pchar = putbyte;
    argv.exp_hook = NULL;
#ifndef CONFIG_PRINT_FLOAT_ENABLE
    argv.flt = NULL;
#else
    argv.flt = flt;
#endif
    argv.local_irq_enable = local_irq_enable;
    argv.local_irq_disable = local_irq_disable;
    mask_init(&argv);
}

int main(void)
{
    memset(stack_magic, 0x5a, sizeof(stack_magic));
    memset(stack_magic0, 0x5a, sizeof(stack_magic0));

    maskrom_init();

    interrupt_init();
    irq_info_table_set(irq_index_2_priority_table, ARRAY_SIZE(irq_index_2_priority_table));

    wdt_init(WDT_4S);

    //配置长按复位功能,可通过efuse关闭,默认为PA0,可支持配置到任意IO
    if (efuse_get_pinr_reset_en()) {
        gpio_longpress_pin0_reset_config(PINR_RESET_IO, PINR_RESET_LVL, PINR_PRESS_TIME, PINR_RESET_RELASE, PINR_PROTECT_EN);
    }
    //关MCLR -- 可通过efuse关闭
    /* mclr_reset_sw(0); */

    port_init();
    clk_voltage_init(CLOCK_MODE_ADAPTIVE, DVDD_VOL_SEL_123V);
    //clock init
    clk_early_init(CLOCK_PLL_REF_LRC200K, 200000, TCFG_CLOCK_PLL_HZ);

    efuse_init();
    clk_set("lsb", TCFG_CLOCK_LSB_HZ);
    clk_set("sys", TCFG_CLOCK_SYS_HZ);

#if TCFG_DB_UART_ENABLE
    //uart init
    uart_log_init();
#endif

    app_early_init();

    sys_timer_init();
    tick_timer_init();
    sys_event_init();

    log_info("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    log_info("       cd09 setup %s %s", __DATE__, __TIME__);
    log_info("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    clock_dump();

    power_early_init();
    /* get_vir_rtc_status(); */
    /* reset_source_dump(); */
    /* get_wakeup_source(); */

    /* power_reset_source_dump(); */

    //Register debugger interrupt
    request_irq(IRQ_SDTAP_IDX, 2, exception_irq_handler, 0);
    request_irq(IRQ_EXCEPTION_IDX, 2, exception_irq_handler, 0);

    //debug_init
    debug_init();

    //sdfile init
    sdfile_init();

    //vm_init
    syscfg_vm_init(TCFG_VM_INIT_FAST);

    if (is_reset_source(P33_POWER_RETURN) == 0) {
        //只需要上电时调用
        norflash_set_write_protect(1, boot_info.vm.vm_saddr);
    }

    app_main();
    return 0;
}

void cpu_assert_debug(void)
{
    while (1) {
        asm("nop");
    }
}

