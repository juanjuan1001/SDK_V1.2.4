#ifndef _AT_CHAR_APP_H_
#define _AT_CHAR_APP_H_

#include "at_char_drv.h"

/*
 * @brief BLE从机广播配置
 * @return AT_CHAR_ERR: 配置失败 AT_CHAR_SUCC: 配置成功
 */
enum at_char_res at_char_ble_slave_broadcast_config(void);

/*
 * @brief 从机低功耗使能配置
 * param en: 0:不使能 1:使能
 * @return AT_CHAR_ERR: 配置失败 AT_CHAR_SUCC: 配置成功
 */
enum at_char_res at_char_slave_lowpower_mode_config(u8 en);

/*
 * @brief 获取从机低功耗使能状态
 * @return -1: 获取失败 0: 正常状态 1: 低功耗状态
 */
s8 at_char_get_slave_lowpower_mode(void);

/*
 * @brief 从机关机配置
 * @return AT_CHAR_ERR: 配置失败 AT_CHAR_SUCC: 配置成功
 * @note at_char_init会唤醒蓝牙模块
 */
enum at_char_res at_char_slave_poweroff_config(void);


#endif
