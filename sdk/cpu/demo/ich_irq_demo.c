#include "gpio.h"
#include "asm/hadc.h"


/***************************port irq function test code****************************/
#if 1

void gpio_irq_callback_test(enum gpio_port port, u32 pin, enum gpio_irq_edge edge)
{
    printf("port%d.%d:%d-cb1\n", port, pin, edge);
}

void gpio_irq_callback_change_test(enum gpio_port port, u32 pin, enum gpio_irq_edge edge)
{
    printf("port%d.%d:%d-cb_change\n", port, pin, edge);
}


void inside_signal_irq_callback_test(enum inside_signal_sel sel, u32 null, enum gpio_irq_edge edge)
{
    printf("inside siganl%d.%d:%d-cb\n", sel, null, edge);
}

void inside_signal_irq_callback_change_test(enum inside_signal_sel sel, u32 null, enum gpio_irq_edge edge)
{
    printf("inside siganl%d.%d:%d-cb_change\n", sel, null, edge);
}

void ich_irq_test()
{
#if 1

    printf("*****************port irq test*******************\n");

    struct gpio_irq_config_st gpio_irq_config_test = {
        .pin = PORT_PIN_0,
        .irq_edge = PORT_IRQ_EDGE_FALL,
        .callback = gpio_irq_callback_test,
    };
    gpio_irq_config(PORTA, &gpio_irq_config_test);

    // 测试回调函数修改接口
    //gpio_irq_set_callback(IO_PORT_SPILT(IO_PORTA_01), gpio_irq_callback_change_test);

    // 测试失能接口
    //gpio_irq_disable(IO_PORT_SPILT(IO_PORTA_01));

    // 测试使能接口
    //gpio_irq_enable(IO_PORT_SPILT(IO_PORTA_01));

    // 测试边沿修改接口
    //printf("ich edge is %d\n", gpio_irq_get_edge(IO_PORT_SPILT(IO_PORTA_01)));
    //gpio_irq_set_edge(IO_PORT_SPILT(IO_PORTA_01), PORT_IRQ_EDGE_FALL);
    //printf("ich edge is %d\n", gpio_irq_get_edge(IO_PORT_SPILT(IO_PORTA_01)));

#endif

#if 1

    printf("*****************inside signal irq test*******************\n");

    hadc_avddcp_config(1, AVDDCP_MODE_SEL_NORMAL, AVDDCP_VOL_SEL_3P60);
    hadc_avddr_config(1, AVDDR_VOL_SEL_3P0);
    hadc_avbg_config(1, 1, 1);
    hadc_avcm_config(1, 1);
    hadc_opa_config(1, 1, HADC_OPA_MUXP_SEL_AIN3, HADC_OPA_MUXN_SEL_DISABLE, HADC_OPA_MUXI_SEL_DISABLE, HADC_OPA_MUXO_SEL_AIN6);

    struct inside_signal_irq_config_st inside_signal_irq_config_test = {
        .irq_edge = PORT_IRQ_EDGE_RISE,
        .callback = inside_signal_irq_callback_test,
    };
    inside_signal_irq_config(HADC_OPA_CMP, &inside_signal_irq_config_test);

    // 测试回调函数修改接口
    //inside_signal_irq_set_callback(HADC_OPA_CMP, inside_signal_irq_callback_change_test);

    // 测试失能接口
    //inside_signal_irq_disable(HADC_OPA_CMP);

    // 测试使能接口
    //inside_signal_irq_enable(HADC_OPA_CMP);

    // 测试边沿修改接口
    //printf("ich edge is %d\n", inside_signal_irq_get_edge(HADC_OPA_CMP));
    //inside_signal_irq_set_edge(HADC_OPA_CMP, PORT_IRQ_EDGE_FALL);
    //printf("ich edge is %d\n", inside_signal_irq_get_edge(HADC_OPA_CMP));

#endif

}
#endif

