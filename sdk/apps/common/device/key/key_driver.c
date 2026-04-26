#include "system/includes.h"
#include "key_driver.h"
#include "iokey.h"
#include "adkey.h"
#include "irkey.h"
#include "touch_key.h"
#include "app_config.h"

#define LOG_TAG_CONST       KEY
#define LOG_TAG             "[KEY]"
#include "debug.h"

//=======================================================//
// 按键值重新映射函数:
// 用户可以实现该函数把一些按键值重新映射, 可用于组合键的键值重新映射
//=======================================================//
int __attribute__((weak)) key_event_remap(struct sys_event *e)
{
    return true;
}

//=======================================================//
// 按键扫描函数: 扫描所有注册的按键驱动
//=======================================================//
static void key_driver_scan(void *_scan_para)
{
    struct key_driver_para *scan_para = (struct key_driver_para *)_scan_para;
    u8 key_event = 0;
    u8 cur_key_value = NO_KEY;
    u8 key_value = 0;
    struct sys_event e;

    cur_key_value = scan_para->get_value();

    //===== 按键消抖处理
    //当前按键值与上一次按键值如果不相等, 重新消抖处理, 注意filter_time != 0;
    if (cur_key_value != scan_para->filter_value && scan_para->filter_time) {
        scan_para->filter_cnt = 0;//消抖次数清0, 重新开始消抖
        scan_para->filter_value = cur_key_value;//记录上一次的按键值
        return;//第一次检测, 返回不做处理
    }
    //当前按键值与上一次按键值相等, filter_cnt开始累加;
    if (scan_para->filter_cnt < scan_para->filter_time) {
        scan_para->filter_cnt++;
        return;
    }

    //===== 按键消抖结束, 开始判断按键类型(单击, 双击, 长按, 多击, HOLD, (长按/HOLD)抬起)
    if (cur_key_value != scan_para->last_key) {
        if (cur_key_value == NO_KEY) {
            //cur_key = NO_KEY; last_key = valid_key -> 按键被抬起
            if (scan_para->press_cnt >= scan_para->long_time) {
                //长按/HOLD状态之后被按键抬起;
                key_event = KEY_EVENT_UP;
                key_value = scan_para->last_key;
                goto _notify;//发送抬起消息
            }
            scan_para->click_delay_cnt = 1;//按键等待下次连击延时开始
        } else {
            //cur_key = valid_key, last_key = NO_KEY -> 按键被按下
            scan_para->press_cnt = 1;//用于判断long和hold事件的计数器重新开始计时;
            if (cur_key_value != scan_para->notify_value) {
                //第一次单击/连击时按下的是不同按键, 单击次数重新开始计数
                scan_para->click_cnt = 1;
                scan_para->notify_value = cur_key_value;
            } else {
                //单击次数累加
                scan_para->click_cnt++;
            }
        }
        goto _scan_end;//返回, 等待延时时间到
    } else {
        //cur_key = last_key -> 没有按键按下/按键长按(HOLD)
        if (cur_key_value == NO_KEY) {
            //last_key = NO_KEY; cur_key = NO_KEY -> 没有按键按下
            if (scan_para->click_cnt > 0) {
                //有按键需要消息需要处理
                if (scan_para->click_delay_cnt > scan_para->click_delay_time) { //按键被抬起后延时到
                    //TODO: 在此可以添加任意多击事件
                    if (scan_para->click_cnt >= 5) {
                        key_event = KEY_EVENT_FIRTH_CLICK;  //五击
                    } else if (scan_para->click_cnt >= 4) {
                        key_event = KEY_EVENT_FOURTH_CLICK;  //4击
                    } else if (scan_para->click_cnt >= 3) {
                        key_event = KEY_EVENT_TRIPLE_CLICK;  //三击
                    } else if (scan_para->click_cnt >= 2) {
                        key_event = KEY_EVENT_DOUBLE_CLICK;  //双击
                    } else {
                        key_event = KEY_EVENT_CLICK;  //单击
                    }
                    key_value = scan_para->notify_value;
                    goto _notify;
                } else {
                    //按键抬起后等待下次延时时间未到
                    scan_para->click_delay_cnt++;
                    goto _scan_end;//按键抬起后延时时间未到, 返回
                }
            } else {
                goto _scan_end;  //没有按键需要处理
            }
        } else {
            //last_key = valid_key; cur_key = valid_key, press_cnt累加用于判断long和hold
            scan_para->press_cnt++;
            if (scan_para->press_cnt == scan_para->long_time) {
                key_event = KEY_EVENT_LONG;
            } else if (scan_para->press_cnt == scan_para->hold_time) {
                key_event = KEY_EVENT_HOLD;
                scan_para->press_cnt = scan_para->long_time;
            } else {
                goto _scan_end;//press_cnt没到长按和HOLD次数, 返回
            }
            //press_cnt没到长按和HOLD次数,发消息
            key_value = cur_key_value;
            goto _notify;
        }
    }

_notify:
    e.type = SYS_KEY_EVENT;
    e.u.key.type = scan_para->key_type;//区分按键类型
    e.u.key.event = key_event;
    e.u.key.value = key_value;

    scan_para->click_cnt = 0;//单击次数清0
    scan_para->notify_value = NO_KEY;

    //检查是否有按键重映射
    if (key_event_remap(&e)) {
        sys_event_notify(&e);
    }
_scan_end:
    scan_para->last_key = cur_key_value;
    return;
}

//=======================================================//
// 按键初始化函数: 初始化所有注册的按键驱动
//=======================================================//
void key_driver_init(void)
{
#if ((defined TCFG_IOKEY_ENABLE) && (TCFG_IOKEY_ENABLE))
    if (iokey_init() == 0) {
        usr_timer_add((void *)&iokey_scan_para, key_driver_scan, iokey_scan_para.scan_time, 0);
    }
#endif

#if ((defined TCFG_ADKEY_ENABLE) && (TCFG_ADKEY_ENABLE))
    if (adkey_init() == 0) {
        usr_timer_add((void *)&adkey_scan_para, key_driver_scan, adkey_scan_para.scan_time, 0);
    }
#endif

#if ((defined TCFG_IRKEY_ENABLE) && (TCFG_IRKEY_ENABLE))
    if (irkey_init() == 0) {
        usr_timer_add((void *)&irkey_scan_para, key_driver_scan, irkey_scan_para.scan_time, 0);
    }
#endif

#if ((defined TCFG_TOUCH_KEY_ENABLE) && (TCFG_TOUCH_KEY_ENABLE))
    if (touch_key_init() == 0) {
        usr_timer_add((void *)&touch_key_scan_para, key_driver_scan, touch_key_scan_para.scan_time, 0);
    }
#endif
}

