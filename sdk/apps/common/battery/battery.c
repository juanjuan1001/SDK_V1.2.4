/*
* @file  		battery.c
* @brief        供电电源电量管理
* @details		BATTERY API
* @author		JL
* @date     	2023-06-12
* @version  	V1.0
* @copyright    Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
 */

#include "battery.h"
#include "gpadc_api.h"
#include "timer.h"
#include "event.h"
#include "gpio.h"

#define LOG_TAG_CONST   BAT
#define LOG_TAG		    "[BATTERY]"
#include "debug.h"

struct _battery_info {
    u8 battery_lvl; 	//供电电源电量
    u16 battery_timer;  //供电电源检测定时器
};
static struct _battery_info battery_info;
#define __this (&battery_info)

const u16 voltage_table[] = {
    BATTERY_LVL_0, 	BATTERY_LVL_10, BATTERY_LVL_20, BATTERY_LVL_30, BATTERY_LVL_40,
    BATTERY_LVL_50, BATTERY_LVL_60, BATTERY_LVL_70, BATTERY_LVL_80, BATTERY_LVL_90,
    BATTERY_LVL_100,
};

/*
 * @brief 获取电池电压
 */
static u16 battery_get_voltage(void)
{
    //锂电池和4节电池时,采用外部IO采集电压,默认分压1/3
    if (TCFG_BAT_AD_CHANNEL == AD_ANA_PMU_CH_VPWR_DIV_4) {
        return (adc_get_voltage(TCFG_BAT_AD_CHANNEL) * 4);
    } else {
        return (adc_get_voltage(TCFG_BAT_AD_CHANNEL) * 3);
    }
}

/*
 * @brief 获取供电电源电量
 */
u8 get_battery_lvl(void)
{
    return __this->battery_lvl;
}

/*
 * @brief 获取供电电源充电状态
 * @return CHARGE_STATE: 充电状态 DISCHARGE_STATE: 放电状态
 * @note 干电池只有放电状态，锂电池需根据实际充电情况返回对应状态
 */
enum charge_state get_battery_charge_state(void)
{
#if (TCFG_BATTERY_TYPE_SEL != BATTERY_TYPE_LITHIUM_DELL)
    return DISCHARGE_STATE;
#else
    if (gpio_read(TCFG_CHARGE_DET_IO)) {
        return CHARGE_STATE;
    } else {
        return DISCHARGE_STATE;
    }
#endif
}

/*
 * @brief 电源电压转换电源电量百分比函数
 * @param voltage: 电源电压
 * @return 返回电源电量百分比
 */
static u8 cal_battery_lvl(u16 voltage)
{
    u16 tmp0, tmp1;
    if (voltage >= BATTERY_LVL_100)	{
        return 100;
    } else if (voltage <= BATTERY_LVL_0) {
        return 0;
    }

    for (u8 i = 1; i < ARRAY_SIZE(voltage_table); i++) {
        if (voltage <= voltage_table[i]) {
            tmp0 = voltage_table[i]	- voltage_table[i - 1];
            tmp1 = voltage - voltage_table[i - 1];
            return (tmp1 * 10 / tmp0 + (i - 1) * 10);
        }
    }
    return 0;
}

/*
 * @brief 退出供电电源检测
 */
void battery_uninit(void)
{
    adc_remove_sample_ch(AD_ANA_PMU_CH_VPWR_DIV_4);
    if (__this->battery_timer) {
        sys_timer_del(__this->battery_timer);
        __this->battery_timer = 0;
    }
}

/*
 * @brief 供电电源电量检测函数，一般半分钟扫描一次
 */
void battery_scan(void *priv)
{
    u16 battery_volt;
    u8 curr_battery_lvl;
    enum charge_state state = get_battery_charge_state();

    battery_volt = battery_get_voltage();
    curr_battery_lvl = cal_battery_lvl(battery_volt);

    if (curr_battery_lvl < __this->battery_lvl) {
        __this->battery_lvl--;
    } else if (curr_battery_lvl > __this->battery_lvl) {
        if (state == CHARGE_STATE) {
            __this->battery_lvl++;
        }
    }
    log_info("scan volt: %dmV, lvl: %d, cur_lvl: %d", battery_volt, __this->battery_lvl, curr_battery_lvl);

    if (!__this->battery_lvl && (state == DISCHARGE_STATE)) {
        battery_uninit();
        sys_msg_event_notify(MSG_EVENT_BATTERY_LOWPOWER, 0);
    }
}

/*
 * @brief 供电电源初始化函数，启动电量监测
 * @return BATTERY_NORMAL: 正常状态，BATTERY_LOWPOWER: 低电状态
 */
enum battery_state battery_init(void)
{
    u16 battery_volt;
    enum charge_state state = get_battery_charge_state();

    if (TCFG_BAT_AD_CHANNEL == AD_ANA_PMU_CH_VPWR_DIV_4) {
        battery_volt = adc_get_voltage_by_blocking(TCFG_BAT_AD_CHANNEL, 8) * 4;
    } else {
        battery_volt = adc_get_voltage_by_blocking(TCFG_BAT_AD_CHANNEL, 8) * 3;
    }
    __this->battery_lvl = cal_battery_lvl(battery_volt);
    /* log_info("init volt: %dmV, lvl: %d", battery_volt, __this->battery_lvl); */

    if (!__this->battery_lvl && (state == DISCHARGE_STATE)) {
        return BATTERY_LOWPOWER;
    } else {
        if (!__this->battery_timer) {
            __this->battery_timer = sys_timer_add(NULL, battery_scan, 30 * 1000);
        }
        return BATTERY_NORMAL;
    }
}
