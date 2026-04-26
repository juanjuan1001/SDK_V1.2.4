#include "key_driver.h"
#include "app_config.h"
#include "touch_key.h"

#undef LOG_TAG_CONST
#define LOG_TAG     "[plcnt]"
#include "debug.h"

#if ((defined TCFG_TOUCH_KEY_ENABLE) && (TCFG_TOUCH_KEY_ENABLE))

/* =========== 触摸键使用说明 ============= */
//1. 使用plcnt模块作计数;
//2. 配置参数时, 在配置好时钟后, 需要调试no_touch_cnt和touch_cnt的值;

#define __this  (&touch_key_data)

const struct touch_key_port touch_key_list[] = {
#if defined(TCFG_TOUCH_KEY0_PORT) && (TCFG_TOUCH_KEY0_PORT != NO_CONFIG_PORT)
    {
        .press_delta    = TCFG_TOUCH_KEY0_PRESS_DELTA,
        .port           = TCFG_TOUCH_KEY0_PORT,
        .key_value      = TCFG_TOUCH_KEY0_VALUE,
    },
#endif
#if defined(TCFG_TOUCH_KEY1_PORT) && (TCFG_TOUCH_KEY1_PORT != NO_CONFIG_PORT)
    {
        .press_delta    = TCFG_TOUCH_KEY1_PRESS_DELTA,
        .port           = TCFG_TOUCH_KEY1_PORT,
        .key_value      = TCFG_TOUCH_KEY1_VALUE,
    },
#endif
#if defined(TCFG_TOUCH_KEY2_PORT) && (TCFG_TOUCH_KEY2_PORT != NO_CONFIG_PORT)
    {
        .press_delta    = TCFG_TOUCH_KEY2_PRESS_DELTA,
        .port           = TCFG_TOUCH_KEY2_PORT,
        .key_value      = TCFG_TOUCH_KEY2_VALUE,
    },
#endif
};

//滤波算法用到
#define TOUCH_VAL_CALIBRATE_CYCLE       100     //标定常态值的时间，单位为key_scand的时间
#define TOUCH_KEY_NUM 		ARRAY_SIZE(touch_key_list)
#define KEY_BASE_CNT  3

const struct plcnt_platform_data touch_key_data = {
    .port_num       = TOUCH_KEY_NUM,
    .port_list      = touch_key_list,
    .sum_num	    = 5,
    .charge_time    = 100,
};

u8 touch_key_get_value(void);
//按键驱动扫描参数列表
struct key_driver_para touch_key_scan_para = {
    .scan_time 	  	  = 10,				//按键扫描频率, 单位: ms
    .last_key 		  = NO_KEY,  		//上一次get_value按键值, 初始化为NO_KEY;
    .filter_time  	  = 1,				//按键消抖延时;
    .long_time 		  = 75,  			//按键判定长按数量
    .hold_time 		  = (75 + 15),  	//按键判定HOLD数量
    .click_delay_time = 20,				//按键被抬起后等待连击延时数量
    .key_type		  = KEY_DRIVER_TYPE_TOUCH,
    .get_value 		  = touch_key_get_value,
};

static u8 get_touch_key_value(void);
u8 touch_key_get_value(void)
{
    u8 key = get_touch_key_value();
    if (key != NO_KEY) {
        return __this->port_list[key].key_value;
    }
    return NO_KEY;
}






static u32 touch_pre_value[TOUCH_KEY_NUM] = {0};
static u32 touch_normal_value[TOUCH_KEY_NUM] = {0};
static u32 touch_calibrate_cnt[TOUCH_KEY_NUM] = {0};
static u32 touch_calibrate_tmp_value[TOUCH_KEY_NUM] = {0};

/*----------------------------------------------------------------------------*/
/**@brief   触摸按键初始化
   @param   void
   @note    void touch_key_init(void)
*/
/*----------------------------------------------------------------------------*/
int touch_key_init(void)
{
    plcnt_init(&touch_key_data);
    for (u8 i = 0; i < __this->port_num; i ++) {
        touch_normal_value[i] = 0 - (__this->port_list[i].press_delta * 2);
    }

    return 0;
}

/*----------------------------------------------------------------------------*/
/**@brief   按键去抖函数，输出稳定键值
   @param   key：键值
   @return  稳定按键
   @note    u8 key_filter(u8 key)
*/
/*----------------------------------------------------------------------------*/
static u8 key_filter(u8 key)
{
    static u8 used_key = NO_KEY;
    static u8 old_key;
    static u8 key_counter;

    if (old_key != key) {
        key_counter = 0;
        old_key = key;
    } else {
        key_counter++;
        if (key_counter == KEY_BASE_CNT) {
            used_key = key;
        }
    }
    return used_key;
}

/*----------------------------------------------------------------------------*/
/**@brief   获取触摸按键值
   @param   void
   @param   void
   @return  key_num:io按键号
   @note    u8 get_touch_value(void)
*/
/*----------------------------------------------------------------------------*/
static u8 get_touch_key_value(void)
{
    u16 key_or = 0;
    u32 cur_val = 0;
    for (u8 i = 0; i < __this->port_num; i ++) {
        cur_val = get_plcnt_value(i);
        /* log_print("%d ", cur_val); */
        //简单滤波
        if (touch_pre_value[i] == 0) {
            touch_pre_value[i] = cur_val;
        } else if (cur_val >= touch_pre_value[i]) {
            touch_pre_value[i] = touch_pre_value[i] + (u32)((cur_val - touch_pre_value[i]) * 0.2f);
        } else {
            touch_pre_value[i] = touch_pre_value[i] - (u32)((touch_pre_value[i] - cur_val) * 0.2f);
        }
        //处理滤波之后的值
        if (touch_pre_value[i] > (touch_normal_value[i] + __this->port_list[i].press_delta)) {
            key_or |= BIT(i);
            touch_calibrate_cnt[i] = 0;
        } else {
            touch_calibrate_cnt[i] ++;
        }
        //定期标定常态下的基准值
        if (touch_calibrate_cnt[i] > TOUCH_VAL_CALIBRATE_CYCLE) {
            touch_normal_value[i] = touch_calibrate_tmp_value[i] / 10;
            touch_calibrate_tmp_value[i] = 0;
            touch_calibrate_cnt[i] = 0;
        } else if (touch_calibrate_cnt[i] >= (TOUCH_VAL_CALIBRATE_CYCLE / 2)) {
            if (touch_calibrate_cnt[i] < ((TOUCH_VAL_CALIBRATE_CYCLE / 2) + 10)) {
                touch_calibrate_tmp_value[i] += touch_pre_value[i];
            }
        } else {
            touch_calibrate_tmp_value[i] = 0;
        }
    }
    /* log_print("\n"); */
    static u8 pre_i = 0;
    u8 key_num = NO_KEY;
    if (key_or) {
        if (key_or & BIT(pre_i)) {
            key_num = pre_i;
        } else {
            for (u8 i = 0; i < __this->port_num; i ++) {
                if (key_or & BIT(i)) {
                    key_num = i;
                    pre_i = i;
                    break;
                }
            }
        }
    }
    return key_filter(key_num);
}




#endif  	/* #if TCFG_TOUCH_KEY_ENABLE */





