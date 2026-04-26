#include "key_driver.h"
#include "system/includes.h"
#include "app_config.h"
#include "gpadc_api.h"
#include "adkey.h"

#if ((defined TCFG_ADKEY_ENABLE) && (TCFG_ADKEY_ENABLE))

#define __this (&adkey_data)

//必须从小到大填电阻，没有则同VDDIO,填AD_MAX_VALUE
#define TCFG_ADKEY_AD0      (0)                                 	//0R
#define TCFG_ADKEY_AD1      (AD_MAX_VALUE * 51   / (51   + R_UP))   //5.1k
#define TCFG_ADKEY_AD2      (AD_MAX_VALUE * 150  / (150  + R_UP))   //15k
#define TCFG_ADKEY_AD3      (AD_MAX_VALUE * 330  / (330  + R_UP))   //33k
#define TCFG_ADKEY_AD4      (AD_MAX_VALUE * 1000 / (1000 + R_UP))   //100k
#define TCFG_ADKEY_VDDIO    (AD_MAX_VALUE)

#define TCFG_ADKEY_VOLTAGE0 ((TCFG_ADKEY_AD0 + TCFG_ADKEY_AD1) / 2)
#define TCFG_ADKEY_VOLTAGE1 ((TCFG_ADKEY_AD1 + TCFG_ADKEY_AD2) / 2)
#define TCFG_ADKEY_VOLTAGE2 ((TCFG_ADKEY_AD2 + TCFG_ADKEY_AD3) / 2)
#define TCFG_ADKEY_VOLTAGE3 ((TCFG_ADKEY_AD3 + TCFG_ADKEY_AD4) / 2)
#define TCFG_ADKEY_VOLTAGE4 ((TCFG_ADKEY_AD4 + TCFG_ADKEY_VDDIO) / 2)

#define TCFG_ADKEY_VALUE0   0
#define TCFG_ADKEY_VALUE1   1
#define TCFG_ADKEY_VALUE2   2
#define TCFG_ADKEY_VALUE3   3
#define TCFG_ADKEY_VALUE4   4

const struct adkey_platform_data adkey_data = {
    .adkey_pin = TCFG_ADKEY_PORT,                             //AD按键对应引脚
    .ad_channel = TCFG_ADKEY_AD_CHANNEL,                      //AD通道值
    .extern_up_en = TCFG_ADKEY_EXTERN_UP_ENABLE,              //是否使用外接上拉电阻
    .ad_value = {                                             //根据电阻算出来的电压值
        TCFG_ADKEY_VOLTAGE0,
        TCFG_ADKEY_VOLTAGE1,
        TCFG_ADKEY_VOLTAGE2,
        TCFG_ADKEY_VOLTAGE3,
        TCFG_ADKEY_VOLTAGE4,
    },
    .key_value = {
        TCFG_ADKEY_VALUE0,
        TCFG_ADKEY_VALUE1,
        TCFG_ADKEY_VALUE2,
        TCFG_ADKEY_VALUE3,
        TCFG_ADKEY_VALUE4,
    },
};

//按键驱动扫描参数列表
struct key_driver_para adkey_scan_para = {
    .scan_time 	  	  = 10,				//按键扫描频率, 单位: ms
    .last_key 		  = NO_KEY,  		//上一次get_value按键值, 初始化为NO_KEY;
    .filter_time  	  = 2,				//按键消抖延时;
    .long_time 		  = 75,  			//按键判定长按数量
    .hold_time 		  = (75 + 15),  	//按键判定HOLD数量
    .click_delay_time = 20,				//按键被抬起后等待连击延时数量
    .key_type		  = KEY_DRIVER_TYPE_AD,
    .get_value 		  = ad_get_key_value,
};

u8 ad_get_key_value(void)
{
    u8 i;
    u16 ad_data = AD_MAX_VALUE;
    ad_data = adc_get_value(__this->ad_channel);

    for (i = 0; i < ADKEY_MAX_NUM; i++) {
        if ((ad_data <= __this->ad_value[i]) && (__this->ad_value[i] < AD_MAX_VALUE)) {
            return __this->key_value[i];
        }
    }
    return NO_KEY;
}

int adkey_init(void)
{
    adc_add_sample_ch(__this->ad_channel);//注意：初始化AD_KEY之前，先初始化ADC
    if (__this->extern_up_en) {
        gpio_set_mode(IO_PORT_SPILT(__this->adkey_pin), PORT_INPUT_FLOATING);
    } else {
        gpio_set_mode(IO_PORT_SPILT(__this->adkey_pin), PORT_INPUT_PULLUP_10K);
    }
    gpio_set_function(IO_PORT_SPILT(__this->adkey_pin), PORT_FUNC_GPADC);
    return 0;
}

#endif  /* #if TCFG_ADKEY_ENABLE */







