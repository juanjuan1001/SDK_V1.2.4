/*
* @file  		at_char_app.c
* @brief        AT_CHAR协议应用
* @details		AT_CHAR APP
* @author		JL
* @date     	2023-06-14
* @version  	V1.0
* @copyright    Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
 */

#include "at_char_app.h"

#define LOG_TAG_CONST   AT_CHAR
#define LOG_TAG	        "[AT_CHAR]"
#include "debug.h"

/*
 * @brief AT_CHAR数据包发送和接收函数
 * @param enum at_char_head _head: 头部
 * @param enum at_char_cmd _cmd: 命令
 * @param enum at_spec_char _spec: 特殊字符
 * @param char **_param_buf: 参数指针（无参数填NULL）
 * @param u8 _param_size: 参数个数（无参数填0）
 * @return 接收从机数据的buf指针，返回NULL代表失败，否则成功
 */
static char *_at_char_send_and_recv(enum at_char_head _head, enum at_char_cmd _cmd, enum at_spec_char _spec, char **_param_buf, u8 _param_size)
{
    AT_CHAR_SEND at_char = {
        .head = _head,
        .cmd = _cmd,
        .spec = _spec,
        .param_buf = _param_buf,
        .param_size = _param_size,
    };
    return at_char_send_and_recv(&at_char);
}

/*
 * @brief BLE从机广播配置
 * @return AT_CHAR_ERR: 配置失败 AT_CHAR_SUCC: 配置成功
 */
enum at_char_res at_char_ble_slave_broadcast_config(void)
{
    char *param_table[] = {"0", "JL_BLE_CD09", "020106", "07094A4C5F424C45", "160", "1"};
    enum at_char_cmd cmd_table[] = {ADV, NAME, ADVDATA, SRDATA, ADVPARAM, ADV};

    for (u8 i = 0; i < ARRAY_SIZE(cmd_table); i++) {
        if (_at_char_send_and_recv(AT_CMD, cmd_table[i], EQUAL_SIGN, &param_table[i], 1) == NULL) {
            return AT_CHAR_ERR;
        }
    }
    return AT_CHAR_SUCC;
}

/*
 * @brief 从机低功耗使能配置
 * param en: 0:不使能 1:使能
 * @return AT_CHAR_ERR: 配置失败 AT_CHAR_SUCC: 配置成功
 */
enum at_char_res at_char_slave_lowpower_mode_config(u8 en)
{
    char *param_table[] = {"0", "1"};
    en = !!en;
    if (_at_char_send_and_recv(AT_CMD, LOWPOWER, EQUAL_SIGN, &param_table[en], 1) == NULL) {
        return AT_CHAR_ERR;
    }
    return AT_CHAR_SUCC;
}

/*
 * @brief 获取从机低功耗使能状态
 * @return -1: 获取失败 0: 正常状态 1: 低功耗状态
 */
s8 at_char_get_slave_lowpower_mode(void)
{
    char *rx = NULL;
    char mode;
    rx = _at_char_send_and_recv(AT_CMD, LOWPOWER, QUESTION_SIGN, NULL, 0);
    if (rx) {
        char *mode_str = strstr(rx, "LOWPOWER:");
        if (mode_str != NULL) {
            mode = *(mode_str + strlen("LOWPOWER:"));
            if (mode == '0') {
                return 0;
            } else if (mode == '1') {
                return 1;
            }
        }
    }
    return -1;
}

/*
 * @brief 从机关机配置
 * @return AT_CHAR_ERR: 配置失败 AT_CHAR_SUCC: 配置成功
 * @note at_char_init会唤醒蓝牙模块
 */
enum at_char_res at_char_slave_poweroff_config(void)
{
    char *param_table[] = {"8"};
    if (_at_char_send_and_recv(AT_CMD, DISC, EQUAL_SIGN, &param_table[0], 1) == NULL) {
        return AT_CHAR_ERR;
    }
    if (_at_char_send_and_recv(AT_CMD, POWEROFF, ENTER_SIGN, NULL, 0) == NULL) {
        return AT_CHAR_ERR;
    }
    return AT_CHAR_SUCC;
}
