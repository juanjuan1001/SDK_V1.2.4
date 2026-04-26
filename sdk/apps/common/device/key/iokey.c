#include "key_driver.h"
#include "system/includes.h"
#include "app_config.h"
#include "iokey.h"

#if ((defined TCFG_IOKEY_ENABLE) && (TCFG_IOKEY_ENABLE))

#define __this  (&iokey_data)

const struct iokey_port iokey_list[] = {
#if defined(TCFG_IOKEY_0_ONE_PORT) && (TCFG_IOKEY_0_ONE_PORT != NO_CONFIG_PORT)
    {
        .connect_way = TCFG_IOKEY_0_CONNECT_WAY,    //IO按键的连接方式
        .key_port = TCFG_IOKEY_0_ONE_PORT,          //IO按键对应的引脚
        .key_value = 0,                             //按键值
    },
#endif
#if defined(TCFG_IOKEY_1_ONE_PORT) && (TCFG_IOKEY_1_ONE_PORT != NO_CONFIG_PORT)
    {
        .connect_way = TCFG_IOKEY_1_CONNECT_WAY,    //IO按键的连接方式
        .key_port = TCFG_IOKEY_1_ONE_PORT,          //IO按键对应的引脚
        .key_value = 1,                             //按键值
    },
#endif
#if defined(TCFG_IOKEY_2_ONE_PORT) && (TCFG_IOKEY_2_ONE_PORT != NO_CONFIG_PORT)
    {
        .connect_way = TCFG_IOKEY_2_CONNECT_WAY,    //IO按键的连接方式
        .key_port = TCFG_IOKEY_2_ONE_PORT,          //IO按键对应的引脚
        .key_value = 2,                             //按键值
    },
#endif
};

const struct iokey_platform_data iokey_data = {
    .num = ARRAY_SIZE(iokey_list),                            //IO按键的个数
    .port = iokey_list,                                       //IO按键参数表
};

//按键驱动扫描参数列表
struct key_driver_para iokey_scan_para = {
    .scan_time 	  	  = 10,				//按键扫描频率, 单位: ms
    .last_key 		  = NO_KEY,  		//上一次get_value按键值, 初始化为NO_KEY;
    .filter_time  	  = 4,				//按键消抖延时;
    .long_time 		  = 75,  			//按键判定长按数量
    .hold_time 		  = (75 + 40),  	//按键判定HOLD数量
    .click_delay_time = 20,				//按键被抬起后等待连击延时数量
    .key_type		  = KEY_DRIVER_TYPE_IO,
    .get_value 		  = io_get_key_value,
};

static void key_io_pull_down_input(u8 key_io)
{
    gpio_set_mode(IO_PORT_SPILT(key_io), PORT_INPUT_PULLDOWN_10K);
}

static void key_io_pull_up_input(u8 key_io)
{
    gpio_set_mode(IO_PORT_SPILT(key_io), PORT_INPUT_PULLUP_10K);
}

static int get_io_key_value(u8 key_io)
{
    return gpio_read(key_io);
}

static void key_io_reset(void)
{
    int i;
    for (i = 0; i < __this->num; i++) {
        switch (__this->port[i].connect_way) {
        case ONE_PORT_TO_HIGH:
            key_io_pull_down_input(__this->port[i].key_port);
            break;
        case ONE_PORT_TO_LOW:
            key_io_pull_up_input(__this->port[i].key_port);
            break;
        default:
            ASSERT(0, "IO KEY CONNECT ERR!!!");
            break;
        }
    }
}

u8 io_get_key_value(void)
{
    int i;

    u8 press_value = 0;
    u8 read_value = 0;
    u8 read_io;
    u8 connect_way;
    u8 ret_value = NO_KEY;

    //先扫描单IO接按键方式
    for (i = 0; i < __this->num; i++) {
        connect_way = __this->port[i].connect_way;
        if (connect_way == ONE_PORT_TO_HIGH) {
            press_value = 1;
        } else if (connect_way == ONE_PORT_TO_LOW) {
            press_value = 0;
        } else {
            continue;
        }

        read_io = __this->port[i].key_port;
        read_value = get_io_key_value(read_io);

        if (read_value == press_value) {
            ret_value = __this->port[i].key_value;
            break;
        }
    }

    return ret_value;
}

int iokey_init(void)
{
    key_io_reset();
    return 0;
}

#endif  /* #if TCFG_IOKEY_ENABLE */


