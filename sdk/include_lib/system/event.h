#ifndef SYS_EVENT_H
#define SYS_EVENT_H

#include "generic/typedef.h"
#include "generic/list.h"

#define SYS_KEY_EVENT 			0x0001
#define SYS_DEVICE_EVENT 		0x0002
#define SYS_MSG_EVENT           0x0004//system库使用,不要改动
#define SYS_CALL_EVENT          0x0008//system库使用,不要改动

#define DEVICE_EVENT_FROM_USB_HOST     (('U' << 24) | ('H' << 16) | '\0' | '\0')
#define DEVICE_EVENT_FROM_OTG          (('O' << 24) | ('T' << 16) | ('G' << 8) | '\0')

enum {
    KEY_EVENT_CLICK,
    KEY_EVENT_LONG,
    KEY_EVENT_HOLD,
    KEY_EVENT_UP,
    KEY_EVENT_DOUBLE_CLICK,
    KEY_EVENT_TRIPLE_CLICK,
    KEY_EVENT_FOURTH_CLICK,
    KEY_EVENT_FIRTH_CLICK,
    KEY_EVENT_USER,
    KEY_EVENT_MAX,
};

enum {
    DEVICE_EVENT_IN,
    DEVICE_EVENT_OUT,
    DEVICE_EVENT_ONLINE,
    DEVICE_EVENT_OFFLINE,
    DEVICE_EVENT_CHANGE,
};

enum msg_table {
    MSG_EVENT_DECODE_END,
    MSG_EVENT_BATTERY_LOWPOWER,
    MSG_EVENT_AT_CHAR_CALIBRATE_WEIGHT,
    MSG_EVENT_AT_CHAR_CALIBRATE_END,
    MSG_EVENT_AT_CHAR_CALIBRATION_RESET,
    MSG_EVENT_AT_CHAR_BODY_FAT_SCALE_GO,
    MSG_EVENT_AT_CHAR_CONN,
    MSG_EVENT_AT_CHAR_DISCONN,
    MSG_EVENT_APP_TASK_SWITCH,
    MSG_EVENT_OXIMETER_RUN,
    MSG_EVENT_AILINK,
    MSG_EVENT_MOTOR_RESUME_DETECT,
};

struct key_event {
    u8 type;//按键类型,IOkey,ADkey...
    u8 event;//按下,短按,长按,抬起...
    u32 value;//按下的键值
};

struct msg_event {
    u8 event;
    u32 value;
};

struct device_event {
    u8 event;
    int value;
};

struct call_event {
    void (*call_back)(void *arg);
};

struct sys_event {
    u16 type;//区分事件类型,按键,设备...
    void *arg;
    union {
        struct key_event key;
        struct device_event dev;
        struct msg_event msg;
        struct call_event call;
    } u;
};

/* 事件通知函数,系统有事件发生时调用此函数 */
bool sys_event_notify(struct sys_event *e);

/* 消息事件发布函数 */
void sys_msg_event_notify(enum msg_table msg, u32 value);

/* 清空事件列表 */
void sys_event_clear(void);

/* 从事件列表获取事件 */
bool sys_event_get(struct sys_event *e);

/* 事件初始化 */
void sys_event_init(void);

#endif

