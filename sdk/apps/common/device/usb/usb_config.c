#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".usb.data.bss")
#pragma data_seg(".usb.data")
#pragma code_seg(".usb.text")
#pragma const_seg(".usb.text.const")
#pragma str_literal_override(".usb.text.const")
#endif

#include "usb_config.h"
#include "usb/scsi.h"
#include "asm/irq.h"
#include "malloc.h"
#include "gpio.h"
#include "timer.h"
#include "app_config.h"
#include "lbuf.h"
#include "asm/clock.h"
#include "asm/power_interface.h"
#include "system/event.h"

#ifdef CONFIG_ADAPTER_ENABLE
#include "adapter_usb_hid.h"
#endif//CONFIG_ADAPTER_ENABLE

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[USB]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#define     SET_INTERRUPT   ___interrupt


#define     MAX_EP_TX   5
#define     MAX_EP_RX   5

static usb_interrupt usb_interrupt_tx[USB_MAX_HW_NUM][MAX_EP_TX];
static usb_interrupt usb_interrupt_rx[USB_MAX_HW_NUM][MAX_EP_RX];

struct usb_config_var_t {
    u8 usb_setup_buffer[USB_SETUP_SIZE];
    struct usb_ep_addr_t usb_ep_addr;
    struct usb_setup_t usb_setup;
};
static struct usb_config_var_t *usb_config_var = {NULL};

#if USB_MALLOC_ENABLE
#else
static struct usb_config_var_t _usb_config_var SEC(.usb.data.bss);
#endif

__attribute__((always_inline_when_const_args))
void *usb_alloc_ep_dmabuffer(const usb_dev usb_id, u32 ep, u32 dma_size)
{
    u8 *ep_buffer = NULL;
    ep_buffer = (u8 *)malloc(dma_size + 4);
    ASSERT(ep_buffer, "usb_alloc_ep_dmabuffer ep_buffer = NULL!!!, _ep = %x, dma_size = %d\n", ep, dma_size);

    log_info("ep_buffer = %x, ep = %x, dma_size = %d\n", (u32)ep_buffer, ep, dma_size);

    return ep_buffer;
}
void usb_dp_wakeup_disable()
{
    return;
}
void usb_dp_wakeup_enable()
{
    return;
}
static u8 usb_slave_status = 1; //1:可以进休眠  0:不可以进休眠
static u8 usb_otg_resume_flag;
static void usb_slave_status_set(u8 status)
{
    usb_slave_status = status;
}
u8 usb_slave_status_get()
{
    return usb_slave_status;
}

static void usb_resume_sign(void *priv)
{
    usb_dev usb_id = usb_device2id(priv);
    u32 reg = usb_read_power(usb_id);
    usb_write_power(usb_id, reg | BIT(2));//send resume
    /* os_time_dly(2);//10ms~20ms */
    mdelay(20);
    usb_write_power(usb_id, reg);//clean resume

}

void usb_remote_wakeup(const usb_dev usb_id)
{
    if (usb_slave_status_get()) {
        usb_phy_resume(0);
        struct usb_device_t *usb_device = usb_id2device(usb_id);
        sys_timeout_add(usb_device, usb_resume_sign, 1);
    }

}
void usb_phy_resume(const usb_dev usb_id)
{
    log_info("usb_phy_resume\n");
#if USB_SUSPEND_RESUME
    usb_dp_wakeup_disable();
#if TCFG_OTG_USB_DEV_EN
    usb_otg_resume_flag = 1;
#endif
#endif

    usb_slave_status_set(0);

    struct usb_device_t *usb_device = usb_id2device(usb_id);
    usb_write_faddr(usb_id, usb_device->baddr);
    if (usb_device->baddr == 0) {
        usb_device->bDeviceStates = USB_DEFAULT;
    } else {
        usb_device->bDeviceStates = USB_CONFIGURED;
    }
    usb_iomode(0);
}

void usb_phy_suspend(const usb_dev usb_id)
{
    log_info("usb_phy_suspend\n");

    usb_iomode(1);

    usb_slave_status_set(1);

    gpio_set_mode(IO_PORT_SPILT(IO_PORT_DP + (usb_id * 2)), PORT_INPUT_PULLUP_10K);
    gpio_set_mode(IO_PORT_SPILT(IO_PORT_DM + (usb_id * 2)), PORT_INPUT_FLOATING);

#if USB_SUSPEND_RESUME
    usb_dp_wakeup_enable();
    putchar('{');
#if TCFG_OTG_USB_DEV_EN
    usb_otg_resume_flag = 0;
#endif
#endif
}
void usb_phy_resume_force(const usb_dev usb_id)
{
    if (usb_slave_status_get()) {
        log_info("usb_phy_resume\n");
#if TCFG_OTG_USB_DEV_EN
        extern void usb_detect_timer_add();
        usb_detect_timer_add();
#endif
        usb_slave_status_set(0);
    }
}
void usb_phy_suspend_force(const usb_dev usb_id)
{
    log_info("usb_phy_suspend\n");
#if TCFG_OTG_USB_DEV_EN
    extern void usb_detect_timer_del();
    usb_detect_timer_del();
    if (JL_USB->CON0 & BIT(4)) { //device_mode
#if TCFG_USB_SLAVE_ENABLE
        usb_stop(usb_id);
#endif
    } else {
#if USB_HOST_ENABLE
        usb_host_unmount(usb_id);
#endif
    }
#endif
    JL_PORTUSB->CON &= BIT(0); //usb_phy close
    JL_USB->CON0 &= ~BIT(11); //usb_sie close
    if ((JL_PORTUSB->CON & BIT(8)) == 0) {
        JL_PORTUSB->CON |= BIT(8);
        gpio_set_mode(IO_PORT_SPILT(IO_PORT_DP + (usb_id * 2)), PORT_HIGHZ);
        gpio_set_mode(IO_PORT_SPILT(IO_PORT_DM + (usb_id * 2)), PORT_HIGHZ);
    }
    usb_slave_status_set(1);
}
void usb_slave_suspend_notify(usb_dev usb_id)
{
    struct sys_event event;
    event.type = SYS_DEVICE_EVENT;
    event.arg = (void *)DEVICE_EVENT_FROM_OTG;
    event.u.dev.event = DEVICE_EVENT_CHANGE;
    event.u.dev.value = (int)"is:0";
    sys_event_notify(&event);
}
void usb_slave_resume_notify(usb_dev usb_id)
{
    struct sys_event event;
    event.type = SYS_DEVICE_EVENT;
    event.arg = (void *)DEVICE_EVENT_FROM_OTG;
    event.u.dev.event = DEVICE_EVENT_CHANGE;
    event.u.dev.value = (int)"ir:0";
    sys_event_notify(&event);
}

void usb_isr(const usb_dev usb_id)
{
    u32 intr_usb, intr_usbe;
    u32 intr_tx, intr_txe;
    u32 intr_rx, intr_rxe;

    __asm__ volatile("ssync");
    usb_read_intr(usb_id, &intr_usb, &intr_tx, &intr_rx);
    usb_read_intre(usb_id, &intr_usbe, &intr_txe, &intr_rxe);
    struct usb_device_t *usb_device = usb_id2device(usb_id);

    intr_usb &= intr_usbe;
    intr_tx &= intr_txe;
    intr_rx &= intr_rxe;

    if (intr_usb & INTRUSB_SUSPEND) {
        log_error("usb_isr suspend");
#if USB_SUSPEND_RESUME
#if TCFG_OTG_USB_DEV_EN
        usb_slave_suspend_notify(usb_id);
#else
        usb_phy_suspend(usb_id);
#endif
#endif
    }
    if (intr_usb & INTRUSB_RESET_BABBLE) {
        log_error("usb_isr reset");
        usb_reset_interface(usb_device);
        usb_slave_status_set(0);
#if TCFG_FUSB_PLL_TRIM
        log_info("FUSB_PLL_AUTO_TRIM RUN\n");
        /* fusb_pll_trim(USB_TRIM_HAND, 10); */
        fusb_pll_trim(USB_TRIM_AUTO, 10);
#endif

#if USB_SUSPEND_RESUME
        u32 reg = usb_read_power(usb_id);
        usb_write_power(usb_id, (reg | INTRUSB_SUSPEND));//enable suspend resume
#endif
    }

    if (intr_usb & INTRUSB_RESUME) {
        log_error("usb_isr resume");
#if USB_SUSPEND_RESUME
        usb_phy_resume(usb_id);
#endif
    }

    if (intr_tx & BIT(0)) {
        if (usb_interrupt_rx[usb_id][0]) {
            usb_interrupt_rx[usb_id][0](usb_device, 0);
        } else {
            usb_control_transfer(usb_device);
#if TCFG_OTG_USB_DEV_EN
            if (usb_otg_resume_flag) {
                usb_otg_resume_flag = 0;
                usb_slave_resume_notify(usb_id);
            }
#endif
        }
    }

    for (int i = 1; i < MAX_EP_TX; i++) {
        if (intr_tx & BIT(i)) {
            if (usb_interrupt_tx[usb_id][i]) {
                usb_interrupt_tx[usb_id][i](usb_device, i);
            }
        }
    }

    for (int i = 1; i < MAX_EP_RX; i++) {
        if (intr_rx & BIT(i)) {
            if (usb_interrupt_rx[usb_id][i]) {
                usb_interrupt_rx[usb_id][i](usb_device, i);
            }
        }
    }
    __asm__ volatile("csync");
}

void usb_sof_isr(const usb_dev usb_id)
{
    usb_sof_clr_pnd(usb_id);
    static u32 sof_count = 0;
    if ((sof_count++ % 1000) == 0) {
        log_debug("sof 1s isr frame:%d", usb_read_sofframe(usb_id));
    }
}
void usb_suspend_check(void *p)
{
    usb_dev usb_id = (usb_dev)p;

    static u16 sof_frame = 0;
    u16 frame = usb_read_sofframe(usb_id);// sof frame 不更新，则usb进入断开或者suspend状态
    if (frame == sof_frame) {
        usb_phy_suspend(usb_id);
    }
    sof_frame = frame;
}

SET_INTERRUPT
void usb0_g_isr()
{
    usb_isr(0);
}
SET_INTERRUPT
void usb0_sof_isr()
{
    usb_sof_isr(0);
}

#if USB_MAX_HW_NUM == 2
SET_INTERRUPT
void usb1_g_isr()
{
    usb_isr(1);
}
SET_INTERRUPT
void usb1_sof_isr()
{
    usb_sof_isr(1);
}
#endif
__attribute__((always_inline_when_const_args))
u32 usb_g_set_intr_hander(const usb_dev usb_id, u32 ep, usb_interrupt hander)
{
    if (ep & USB_DIR_IN) {
        usb_interrupt_tx[usb_id][ep & 0xf] = hander;
    } else {
        usb_interrupt_rx[usb_id][ep] = hander;
    }
    return 0;
}
void usb_g_isr_reg(const usb_dev usb_id, u8 priority, u8 cpu_id)
{
    if (usb_id == 0) {
        request_irq(IRQ_USB_CTRL_IDX, priority, usb0_g_isr, cpu_id);
    } else {
#if USB_MAX_HW_NUM == 2
        request_irq(IRQ_USB1_CTRL_IDX, priority, usb1_g_isr, cpu_id);
#endif
    }
}
void usb_sof_isr_reg(const usb_dev usb_id, u8 priority, u8 cpu_id)
{
    if (usb_id == 0) {
        request_irq(IRQ_USB_SOF_IDX, priority, usb0_sof_isr, cpu_id);
    } else {
#if USB_MAX_HW_NUM == 2
        request_irq(IRQ_USB1_SOF_IDX, priority, usb1_sof_isr, cpu_id);
#endif
    }
    usb_sofie_enable(usb_id);
}
u32 usb_config(const usb_dev usb_id)
{
    memset(usb_interrupt_rx[usb_id], 0, sizeof(usb_interrupt_rx[usb_id]));
    memset(usb_interrupt_tx[usb_id], 0, sizeof(usb_interrupt_tx[usb_id]));

    if (!usb_config_var) {
#if USB_MALLOC_ENABLE
        usb_config_var = (struct usb_config_var_t *)malloc(sizeof(struct usb_config_var_t));
        memset(usb_config_var, 0, sizeof(struct usb_config_var_t));
        if (!usb_config_var) {
            return -1;
        }
#else
        memset(&_usb_config_var, 0, sizeof(_usb_config_var));
        usb_config_var = &_usb_config_var;
#endif
    }
    log_debug("zalloc: usb_config_var = %x\n", (u32)usb_config_var);

    usb_var_init(usb_id, &(usb_config_var->usb_ep_addr));
    usb_setup_init(usb_id, &(usb_config_var->usb_setup), usb_config_var->usb_setup_buffer);
    return 0;
}

u32 usb_release(const usb_dev usb_id)
{
    log_debug("free zalloc: usb_id = %d, usb_config_var = %x\n", usb_id, (u32)usb_config_var);
    usb_var_init(usb_id, NULL);
    usb_setup_init(usb_id, NULL, NULL);
#if USB_MALLOC_ENABLE
    if (usb_config_var) {
        log_debug("free: usb_config_var = %x\n", (u32)usb_config_var);
        free(usb_config_var);
    }
#endif

    usb_config_var = NULL;

    return 0;
}

REGISTER_LP_TARGET(usb_slave_lp_target) = {
    .name = "usb_slave",
    .is_idle = usb_slave_status_get,
};
