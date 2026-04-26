#include "key_driver.h"
#include "system/includes.h"
#include "ir_decoder.h"
#include "app_config.h"
#include "irkey.h"

#if ((defined TCFG_IRKEY_ENABLE) && (TCFG_IRKEY_ENABLE))

//按键驱动扫描参数列表
struct key_driver_para irkey_scan_para = {
    .scan_time 	  	  = 110,            //按键扫描频率, 单位: ms
    .last_key 		  = NO_KEY,         //上一次get_value按键值, 初始化为NO_KEY;
    .filter_time  	  = 0,              //按键消抖延时;
    .long_time 		  = 6,              //按键判定长按数量
    .hold_time 		  = (6 + 3),        //按键判定HOLD数量
    .click_delay_time = 2,              //按键被抬起后等待连击延时数量
    .key_type		  = KEY_DRIVER_TYPE_IR,
    .get_value 		  = ir_get_key_value,
};

const struct ff00_2_keynum IRTabFF00[] = {
    {0x07,  IR_06},
    {0x08,  IR_15},
    {0x09,  IR_08},
    {0x0C,  IR_12},
    {0x0D,  IR_11},
    {0x15,  IR_07},
    {0x15,  IR_09},
    {0x18,  IR_13},
    {0x19,  IR_10},
    {0x1C,  IR_16},
    {0x40,  IR_04},
    {0x42,  IR_18},
    {0x43,  IR_05},
    {0x44,  IR_03},
    {0x45,  IR_00},
    {0x46,  IR_01},
    {0x47,  IR_02},
    {0x4A,  IR_20},
    {0x52,  IR_19},
    {0x5A,  IR_17},
    {0x5E,  IR_14},
};

/*----------------------------------------------------------------------------*/
/**@brief   获取ir按键值
   @param   void
   @param   void
   @return  void
   @note    void get_irkey_value(void)
*/
/*----------------------------------------------------------------------------*/
u8 ir_get_key_value(void)
{
    u8 tkey;
    u8 key_value = 0xff;
    tkey = ir_decoder_get_command_value();
    if (tkey == 0xff) {
        return NO_KEY;
    }
    for (u8 i = 0; i < ARRAY_SIZE(IRTabFF00); i++) {
        if (tkey == IRTabFF00[i].source_value) {
            key_value = IRTabFF00[i].key_value;
            break;
        }
    }
    printf("recive tkey: 0x%x\n", tkey);
    return key_value;
}

/*----------------------------------------------------------------------------*/
/**@brief   ir按键初始化
   @param   void
   @param   void
   @return  void
   @note    void ir_key_init(void)
*/
/*----------------------------------------------------------------------------*/
int irkey_init(void)
{
    const struct gptimer_config ir_decode_config = {
        .capture.filter = 0,//38000,
        .capture.max_period = 110 * 1000, //110ms
        .capture.port = TCFG_IRKEY_PORT / 16,
        .capture.pin = BIT(TCFG_IRKEY_PORT % 16),
        .irq_cb = NULL,
        .irq_priority = 3,
        //根据红外模块的 idle 电平状态，选择边沿触发方式
        .mode = GPTIMER_MODE_CAPTURE_EDGE_FALL,
        /* .mode = GPTIMER_MODE_CAPTURE_EDGE_RISE, */
    };
    ir_decoder_init(&ir_decode_config);
    return 0;
}

#endif


