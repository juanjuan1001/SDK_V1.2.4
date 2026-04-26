#define LOG_TAG_CONST       GPIO
#define LOG_TAG     "[gpio_irq]"
#define LOG_ERROR_ENABLE
//#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#include "debug.h"
#include "gpio.h"
#include "asm/clock.h"
#include "exti.h"


/***************************irq code****************************/
//该功能需要ich通道,注意功能复用
//PORT_IRQ_ANYEDGE:最多4个io, 单边沿最多8个io
//单边沿与双边沿切换: 请先注销再注册
int gpio_irq_config(enum gpio_port port, const struct gpio_irq_config_st *config)//pa,pc,pd,usb
{
    u32 portx = port * IO_GROUP_NUM;
    u32 old_ich = gpio_get_ich_use_flag();
    for (u8 i = 0; i < IO_GROUP_NUM; i++) {
        if (config->pin & BIT(i)) {
            //注销io
            if (config->irq_edge == PORT_IRQ_DISABLE) {
                int res1 = exti_ich_close(portx + i);
                /* int res2 = exti_io_close(portx + i); */
                /* if ((res1 < 0) && (res2 < 0)) { */
                if (res1 < 0) {
                    log_error("io:%d exti_close error!\n", portx + i);
                } else {
                    log_info("io:%d exti_close ok", portx + i);
                }
            } else if (config->irq_edge == PORT_IRQ_EDGE_RISE) {
                exti_ich_init(portx + i, 0, config->callback, config->irq_priority);
            } else if (config->irq_edge == PORT_IRQ_EDGE_FALL) {
                exti_ich_init(portx + i, 1, config->callback, config->irq_priority);
            } else if (config->irq_edge == PORT_IRQ_ANYEDGE) {
                exti_ich_init(portx + i, 0, config->callback, config->irq_priority);
                exti_ich_init(portx + i, 1, config->callback, config->irq_priority);
            }
        }
    }
    u32 new_ich = gpio_get_ich_use_flag();
    if (old_ich != new_ich) {
        log_debug("old ich_use_flag:0x%08x,new:0x%08x", old_ich, new_ich);
    }
    return 0;
}
int gpio_irq_set_callback(enum gpio_port port, u32 pin, gpio_irq_callback_p callback)
{
    u32 portx = port * IO_GROUP_NUM;
    for (u8 i = 0; i < IO_GROUP_NUM; i++) {
        if (pin & BIT(i)) {
            exti_change_callback(portx + i, callback);
        }
    }
    return 0;
}
int gpio_irq_enable(enum gpio_port port, u32 pin)
{
    u32 portx = port * IO_GROUP_NUM;
    for (u8 i = 0; i < IO_GROUP_NUM; i++) {
        if (pin & BIT(i)) {
            exti_change_en_state(portx + i, 1);//wkup_en:1:en,0:disable
        }
    }
    return 0;
}
int gpio_irq_disable(enum gpio_port port, u32 pin)
{
    u32 portx = port * IO_GROUP_NUM;
    for (u8 i = 0; i < IO_GROUP_NUM; i++) {
        if (pin & BIT(i)) {
            exti_change_en_state(portx + i, 0);//wkup_en:1:en,0:disable
        }
    }
    return 0;
}

//只有注册单边沿触发才能调用该函数
//单边沿与双边沿切换: 请先注销再注册
int gpio_irq_set_edge(enum gpio_port port, u32 pin, enum gpio_irq_edge irq_edge)//只有注册单边沿触发才能调用该函数
{
    u32 portx = port * IO_GROUP_NUM;
    for (u8 i = 0; i < IO_GROUP_NUM; i++) {
        if (pin & BIT(i)) {
            if (irq_edge == PORT_IRQ_EDGE_RISE) {
                exti_change_edge_state(portx + i, 0);
            } else if (irq_edge == PORT_IRQ_EDGE_FALL) {
                exti_change_edge_state(portx + i, 1);
            }
        }
    }
    return 0;
}
// pin:只能带入1个io
enum gpio_irq_edge gpio_irq_get_edge(enum gpio_port port, u32 pin)
{
    ASSERT(!(pin & (pin - 1)), "%s():Parameter pin error!It must be one io!", __func__);
    u8 edge_ret = 0;
    u32 portx = port * IO_GROUP_NUM;
    for (u8 i = 0; i < IO_GROUP_NUM; i++) {
        if (pin & BIT(i)) {
            edge_ret = exti_get_edge_state(portx + i);
            /* log_info("port%c:%d,edge:%d", port + 'a', i, edge_ret); */
            return edge_ret;//return 第一个io
        }
    }
    return edge_ret;//return 最后一个io
}
/***************************port irq function test code****************************/
#if 0

void wdt_clear();
/*{{{*/
void gpio_irq_callback_p_test1(enum gpio_port port, u32 pin, enum gpio_irq_edge edge)
{
    JL_PORTA->OUT ^= BIT(5); //test io
    log_info("port%d.%d:%d-cb1", port, pin, edge);
    JL_PORTA->OUT ^= BIT(5); //test io
}
void gpio_irq_callback_p_test2(enum gpio_port port, u32 pin, enum gpio_irq_edge edge)
{
    JL_PORTA->OUT ^= BIT(5); //test io
    log_info("port%d.%d:%d-cb2", port, pin, edge);
    JL_PORTA->OUT ^= BIT(5); //test io
}

//PORTA,PA0,PA2,PA3设置为打开10k上拉，关闭下拉，下降沿触发中断，
struct gpio_irq_config_st gpio_irq_config_test = {
    .pin = PORT_PIN_0 | PORT_PIN_2 | PORT_PIN_1,
    .irq_edge = PORT_IRQ_EDGE_RISE,//PORT_IRQ_ANYEDGE
    .callback = gpio_irq_callback_p_test1,
    .irq_priority = 3,
};
struct gpio_irq_config_st gpio_irq_config_test2 = {
    .pin = PORT_PIN_0,
    .irq_edge = PORT_IRQ_EDGE_FALL,//PORT_IRQ_ANYEDGE
    .callback = gpio_irq_callback_p_test2,
    .irq_priority = 3,
};

void gpio_irq_test()
{
    log_info("*****************port irq test*******************");
    JL_PORTA->OUT &= ~BIT(5); //test io
    JL_PORTA->DIR &= ~BIT(5); //
    gpio_irq_config(PORTA, &gpio_irq_config_test);
    gpio_irq_get_edge(PORTA, 0x0005);
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    gpio_irq_config(PORTA, &gpio_irq_config_test2);
    gpio_irq_get_edge(PORTA, 0x0005);
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    gpio_irq_config_test2.irq_edge = PORT_IRQ_DISABLE;
    gpio_irq_config(PORTA, &gpio_irq_config_test2);
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    gpio_irq_config_test2.irq_edge = PORT_IRQ_EDGE_FALL;
    gpio_irq_config(PORTA, &gpio_irq_config_test2);

    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    gpio_irq_set_callback(PORTA, 0x0005, gpio_irq_callback_p_test2);
    gpio_irq_disable(PORTA, 0x0005);
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    gpio_irq_enable(PORTA, 0x0005);

    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    gpio_irq_set_edge(PORTA, 0x0005, PORT_IRQ_EDGE_RISE); //只有注册单边沿触发才能调用该函数
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();

    gpio_irq_set_edge(PORTA, 0x0005, PORT_IRQ_EDGE_FALL); //只有注册单边沿触发才能调用该函数
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    gpio_irq_config_test2.irq_edge = PORT_IRQ_DISABLE;
    gpio_irq_config(PORTA, &gpio_irq_config_test2);
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();

    gpio_irq_config_test.irq_edge = PORT_IRQ_DISABLE;
    gpio_irq_config(PORTA, &gpio_irq_config_test);
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();

    gpio_irq_config_test.irq_edge = PORT_IRQ_ANYEDGE;
    gpio_irq_config(PORTA, &gpio_irq_config_test);
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();
    mdelay(1000);
    wdt_clear();

    gpio_irq_config_test.irq_edge = PORT_IRQ_DISABLE;
    gpio_irq_config(PORTA, &gpio_irq_config_test);
    while (1) {
        wdt_clear();
    }
}/*}}}*/
#endif

// 以下是内部信号中断相关接口

int inside_signal_irq_config(enum inside_signal_sel sel, const struct inside_signal_irq_config_st *config)
{
    if (config->irq_edge == PORT_IRQ_EDGE_RISE) {
        inside_signal_ich_wkup_interrupt_init(sel, 0, config->callback, config->irq_priority);
    } else if (config->irq_edge == PORT_IRQ_EDGE_FALL) {
        inside_signal_ich_wkup_interrupt_init(sel, 1, config->callback, config->irq_priority);
    } else if (config->irq_edge == PORT_IRQ_ANYEDGE) {
        inside_signal_ich_wkup_interrupt_init(sel, 0, config->callback, config->irq_priority);
        inside_signal_ich_wkup_interrupt_init(sel, 1, config->callback, config->irq_priority);
    }
    return 0;
}

int inside_signal_irq_set_callback(enum inside_signal_sel sel, inside_signal_irq_callback_p callback)
{
    inside_signal_irq_change_callback(sel, callback);
    return 0;
}

int inside_signal_irq_enable(enum inside_signal_sel sel)
{
    inside_signal_irq_change_en_state(sel, 1);
    return 0;
}

int inside_signal_irq_disable(enum inside_signal_sel sel)
{
    inside_signal_irq_change_en_state(sel, 0);
    return 0;
}

int inside_signal_irq_set_edge(enum inside_signal_sel sel, enum gpio_irq_edge irq_edge)
{
    if (irq_edge == PORT_IRQ_EDGE_RISE) {
        inside_signal_irq_change_edge_state(sel, 0);
    } else if (irq_edge == PORT_IRQ_EDGE_FALL) {
        inside_signal_irq_change_edge_state(sel, 1);
    }
    return 0;
}

enum gpio_irq_edge inside_signal_irq_get_edge(enum inside_signal_sel sel)
{
    return inside_signal_irq_get_edge_state(sel);
}

