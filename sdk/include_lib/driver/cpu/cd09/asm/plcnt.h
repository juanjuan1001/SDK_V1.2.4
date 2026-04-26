#ifndef _PL_CNT_H_
#define _PL_CNT_H_

#include "gpio.h"

struct touch_key_port {
    u16 press_delta;    //按下判决的阈值
    u8 port; 			//触摸按键IO
    u8 key_value; 		//按键返回值
};
struct plcnt_platform_data {
    u8 port_num;    //引脚个数
    const struct touch_key_port *port_list;
    u8 sum_num;     //重复计数的次数，用于求平均值
    u8 charge_time; //充电时间，要保证寄生电容能充满电
};

void plcnt_init(const struct plcnt_platform_data *pdata);
void plcnt_uninit();
u32 get_plcnt_value(u8 ch);

#endif

