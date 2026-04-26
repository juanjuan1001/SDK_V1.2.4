#include "system/includes.h"
#include "app_config.h"
#include "gpadc_api.h"
#include "gpio.h"
#include "app_main.h"
#include "key_driver.h"
#include "decode/decode.h"
#include "decode/voice.h"
#include "asm/power/power_app.h"
#include "usb/device/usb_stack.h"
#include "usb/host/usb_host.h"
#include "usb/device/msd.h"

#define LOG_TAG_CONST   APP
#define LOG_TAG         "[APP]"
#include "debug.h"

static struct usb_device_t *usb_device;

void app_usb_event_handler(struct sys_event *event)
{
    const char *usb_msg = (const char *)event->u.dev.value;
    const u8 u_dev_event = event->u.dev.event;
    const int u_dev_value = event->u.dev.value;
    const usb_dev usb_id = usb_msg[2] - '0';
    switch ((int)event->arg) {
    case DEVICE_EVENT_FROM_OTG:
#if TCFG_USB_SLAVE_ENABLE
        if (usb_msg[0] == 's') {
            if (u_dev_event == DEVICE_EVENT_IN) {
                log_info("usb_slave online\n");
                usb_start(usb_id);
                usb_device = usb_id2device(0);
            } else if (u_dev_event == DEVICE_EVENT_OUT) {
                log_info("usb_slave offline\n");
                usb_device = NULL;
                usb_stop(usb_id);
            }
        }
#endif
#if USB_HOST_ENABLE
        if (usb_msg[0] == 'h') {
            if (u_dev_event == DEVICE_EVENT_IN) {
                log_info("usb_host online\n");
                usb_host_mount(usb_id, MOUNT_RETRY, MOUNT_RESET, MOUNT_TIMEOUT);
            } else if (u_dev_event == DEVICE_EVENT_OUT) {
                log_info("usb_host offline\n");
                usb_host_unmount(usb_id);
            }
        }
#endif
        break;
    case DEVICE_EVENT_FROM_USB_HOST:
        if (u_dev_event == DEVICE_EVENT_IN) {
            /* dev_open("name", &arg); */
        } else if (u_dev_event == DEVICE_EVENT_OUT) {
            /* dev_close("name", &arg); */
        } else if (u_dev_event == DEVICE_EVENT_CHANGE) {
        }
        break;
    default:
        break;
    }
}

void app_key_event_handler(struct sys_event *event)
{
    struct key_event *key = &event->u.key;
    log_info("key_type: %d, key_event: %d, key_value: %d\n", key->type, key->event, key->value);
}

void app_msg_event_handler(struct sys_event *event)
{
    struct msg_event *msg = &event->u.msg;
    log_info("msg_event: %d, msg_value: %d\n", msg->event, msg->value);
    switch (msg->event) {
    case MSG_EVENT_DECODE_END:
        if (!app_play_file_loop()) {
            a_player_stop(1);
        }
        break;
    }
}

void app_device_event_handler(struct sys_event *event)
{
    struct device_event *dev = &event->u.dev;
    log_info("dev_arg: 0x%x, dev_event: %d, dev_value: %d\n", (u32)event->arg, dev->event, dev->value);
    if ((int)event->arg == DEVICE_EVENT_FROM_OTG) {
        app_usb_event_handler(event);
    } else if ((int)event->arg == DEVICE_EVENT_FROM_USB_HOST) {
        app_usb_event_handler(event);
    } else {

    }
}

void onchip_device_init()
{
    sys_power_init();

    //若使用RTC建议在此进行初始化

    adc_init();

    devices_init();
}

void app_main(void)
{
    bool ret;
    struct sys_event event;
    log_info("app_main!\n");

    onchip_device_init();

    key_driver_init();

    user_usb_otg_init();

    while (1) {
        wdt_clear();
#if TCFG_USB_SLAVE_ENABLE && TCFG_USB_SLAVE_MSD_ENABLE
        if (usb_device != NULL) {
            USB_MassStorage(usb_device);
        }
#endif
        ret = sys_event_get(&event);
        if (ret == FALSE) {
            ___idle;//无事件响应,系统进入idle
            continue;
        }
        switch (event.type) {
        case SYS_KEY_EVENT:
            app_key_event_handler(&event);
            break;
        case SYS_MSG_EVENT:
            app_msg_event_handler(&event);
            break;
        case SYS_DEVICE_EVENT:
            app_device_event_handler(&event);
            break;
        }
    }
}

