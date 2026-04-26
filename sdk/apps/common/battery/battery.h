#ifndef _BATTERY_H_
#define _BATTERY_H_

#include "typedef.h"
#include "app_config.h"

#if (TCFG_BATTERY_TYPE_SEL == BATTERY_TYPE_2_DRY_DELL)
#define BATTERY_LVL_0		(2000)
#define BATTERY_LVL_10		(2120)
#define BATTERY_LVL_20		(2240)
#define BATTERY_LVL_30		(2360)
#define BATTERY_LVL_40		(2480)
#define BATTERY_LVL_50		(2600)
#define BATTERY_LVL_60		(2720)
#define BATTERY_LVL_70		(2840)
#define BATTERY_LVL_80		(2960)
#define BATTERY_LVL_90		(3080)
#define BATTERY_LVL_100		(3200)
#elif (TCFG_BATTERY_TYPE_SEL == BATTERY_TYPE_3_DRY_DELL)
#define BATTERY_LVL_0		(3000)
#define BATTERY_LVL_10		(3180)
#define BATTERY_LVL_20		(3360)
#define BATTERY_LVL_30		(3540)
#define BATTERY_LVL_40		(3720)
#define BATTERY_LVL_50		(3900)
#define BATTERY_LVL_60		(4080)
#define BATTERY_LVL_70		(4260)
#define BATTERY_LVL_80		(4440)
#define BATTERY_LVL_90		(4620)
#define BATTERY_LVL_100		(4800)
#elif (TCFG_BATTERY_TYPE_SEL == BATTERY_TYPE_4_DRY_DELL)
#define BATTERY_LVL_0		(4000)
#define BATTERY_LVL_10		(4240)
#define BATTERY_LVL_20		(4480)
#define BATTERY_LVL_30		(4720)
#define BATTERY_LVL_40		(4960)
#define BATTERY_LVL_50		(5200)
#define BATTERY_LVL_60		(5440)
#define BATTERY_LVL_70		(5680)
#define BATTERY_LVL_80		(5920)
#define BATTERY_LVL_90		(6160)
#define BATTERY_LVL_100		(6400)
#elif (TCFG_BATTERY_TYPE_SEL == BATTERY_TYPE_LITHIUM_DELL)
#define BATTERY_LVL_0		(3300)
#define BATTERY_LVL_10		(3680)
#define BATTERY_LVL_20		(3740)
#define BATTERY_LVL_30		(3770)
#define BATTERY_LVL_40		(3790)
#define BATTERY_LVL_50		(3820)
#define BATTERY_LVL_60		(3870)
#define BATTERY_LVL_70		(3920)
#define BATTERY_LVL_80		(3980)
#define BATTERY_LVL_90		(4060)
#define BATTERY_LVL_100		(4100)
#endif

enum charge_state {
    CHARGE_STATE,
    DISCHARGE_STATE,
};

enum battery_state {
    BATTERY_NORMAL,
    BATTERY_LOWPOWER,
};

/*
 * @brief 供电电源初始化函数，启动电量监测
 * @return BATTERY_NORMAL: 正常状态，BATTERY_LOWPOWER: 低电状态
 */
enum battery_state battery_init(void);

/*
 * @brief 获取供电电源电量
 */
u8 get_battery_lvl(void);

#endif
