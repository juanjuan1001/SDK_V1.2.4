#pragma bss_seg(".usb_app.task_pc.data.bss")
#pragma data_seg(".usb_app.task_pc.data")
#pragma const_seg(".task_pc.text.const")
#pragma code_seg(".task_pc.text")
#pragma str_literal_override(".task_pc.text.const")

#include "typedef.h"
#include "app_config.h"
/* #include "msg.h" */
/* #include "otg.h" */
#include "includes.h"
#include "usb/usb_config.h"
#include "usb/device/usb_stack.h"
#include "device/device.h"

#if TCFG_USB_SLAVE_MSD_ENABLE
#include "usb/device/msd.h"
#endif
#if TCFG_USB_SLAVE_HID_ENABLE
#include "usb/device/hid.h"
#endif
#if TCFG_USB_SLAVE_AUDIO_ENABLE
#include "usb/device/uac_audio.h"
#endif
#if TCFG_USB_SLAVE_CDC_ENABLE
#include "usb/device/cdc.h"
#endif

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[TASK_PC]"
#include "debug.h"

#if TCFG_USB_SLAVE_MSD_ENABLE
void msd_register_disk_api(void)
{
#if TCFG_NORFLASH_DEV_ENABLE
    msd_register_disk(__EXT_FLASH_BULK, NULL);
#endif

#if TCFG_MSFC_DEV_ENABLE
    msd_register_disk(__MSFC_NAME, NULL);
#endif
}
#endif


#if TCFG_USB_SLAVE_CDC_ENABLE
static void usb_cdc_wakeup(struct usb_device_t *usb_device)
{
    const usb_dev usb_id = usb_device2id(usb_device);
    u8 buf[64] = {0};
    u8 rlen;
    log_debug("cdc rx hook");
    rlen = cdc_read_data(usb_id, buf, 64);
    /* printf_buf(buf, rlen);//测试时打开 */
    /* cdc_write_data(usb_id, buf, rlen);//测试时打开 */
}
#endif

void usb_start(const usb_dev usb_id)
{
    log_info("USB Start");
    log_info("USB_DEVICE_CLASS_CONFIG:%x", USB_DEVICE_CLASS_CONFIG);
    usb_device_mode(usb_id, USB_DEVICE_CLASS_CONFIG);

#if TCFG_USB_SLAVE_MSD_ENABLE
    msd_register_disk_api();
#endif

#if TCFG_USB_SLAVE_CDC_ENABLE
    cdc_set_wakeup_handler(usb_cdc_wakeup);
#endif
}

void usb_pause(const usb_dev usb_id)
{
    log_info("USB Pause");
    usb_sie_disable(usb_id);
    usb_device_mode(usb_id, 0);
}

void usb_stop(const usb_dev usb_id)
{
    log_info("USB Stop");
    usb_pause(usb_id);
    usb_sie_close(usb_id);
}

static struct otg_dev_data otg_data = {
    .usb_dev_en = TCFG_OTG_USB_DEV_EN,
    .slave_online_cnt = TCFG_OTG_SLAVE_ONLINE_CNT,
    .slave_offline_cnt = TCFG_OTG_SLAVE_OFFLINE_CNT,
    .host_online_cnt = TCFG_OTG_HOST_ONLINE_CNT,
    .host_offline_cnt = TCFG_OTG_HOST_OFFLINE_CNT,
    .detect_mode = TCFG_OTG_MODE,
    .detect_time_interval = TCFG_OTG_DET_INTERVAL,
};
void user_usb_otg_init(void)
{
#if TCFG_OTG_USB_DEV_EN
    usb_otg_init(NULL, &otg_data);
#endif
}
