#ifndef DEVICE_ADKEY_H
#define DEVICE_ADKEY_H

#include "typedef.h"
#include "key_driver.h"

#define ADKEY_MAX_NUM 5

struct adkey_platform_data {
    u8 adkey_pin;
    u8 extern_up_en;//是否用外部上拉，1：用外部上拉， 0：用内部上拉10K
    u32 ad_channel;
    u16 ad_value[ADKEY_MAX_NUM];
    u8  key_value[ADKEY_MAX_NUM];
};

extern int adkey_init(void);
extern u8 ad_get_key_value(void);
extern struct key_driver_para adkey_scan_para;

#endif


