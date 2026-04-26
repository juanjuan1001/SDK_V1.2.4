#ifndef DEVICE_IRKEY_H
#define DEVICE_IRKEY_H

#include "typedef.h"
#include "key_driver.h"

/*ir key define*/
#define IR_00                   0
#define IR_01                   1
#define IR_02                   2
#define IR_03                   3
#define IR_04                   4
#define IR_05                   5
#define IR_06                   6
#define IR_07                   7
#define IR_08                   8
#define IR_09                   9
#define IR_10                   10
#define IR_11                   11
#define IR_12                   12
#define IR_13                   13
#define IR_14                   14
#define IR_15                   15
#define IR_16                   16
#define IR_17                   17
#define IR_18                   18
#define IR_19                   19
#define IR_20                   20
#define IR_21                   21
#define IR_22                   22

struct ff00_2_keynum {
    u8 source_value;
    u8 key_value;
};

extern u8 ir_get_key_value(void);
extern int irkey_init(void);
extern struct key_driver_para irkey_scan_para;

#endif


