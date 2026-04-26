#include "gpio.h"

#define LOG_TAG_CONST   GPIO
#define LOG_TAG         "[gpio]"
#include "debug.h"


/***************************gpio code****************************/
__attribute__((always_inline_when_const_args))
AT(.gpio.text.cache.L2)
int gpio_set_mode(enum gpio_port port, u32 pin, enum gpio_mode mode)
{
    if (gpio_hw_port_pin_judge(port, pin) < 0) {
        return -1;
    }
    switch (mode) {
    case PORT_OUTPUT_LOW:
        gpio_hw_write_port(port, pin, 0);//out 0
        gpio_hw_set_direction(port, pin, 0);//0:out, 1:in
        break;
    case PORT_OUTPUT_HIGH:
        gpio_hw_write_port(port, pin, 1);//out 1
        gpio_hw_set_direction(port, pin, 0);//0:out, 1:in
        break;
    case PORT_INPUT_FLOATING:
        gpio_hw_set_direction(port, pin, 1);//0:out, 1:in
        gpio_hw_set_die(port, pin, 1);
        gpio_hw_set_dieh(port, pin, 1);
        gpio_hw_set_pull_up(port, pin, GPIO_PULLUP_DISABLE);
        gpio_hw_set_pull_down(port, pin, GPIO_PULLDOWN_DISABLE);
        break;
    case PORT_HIGHZ:
        gpio_hw_set_direction(port, pin, 1);//0:out, 1:in
        gpio_hw_set_die(port, pin, 0);
        gpio_hw_set_dieh(port, pin, 0);
        gpio_hw_set_pull_up(port, pin, GPIO_PULLUP_DISABLE);
        gpio_hw_set_pull_down(port, pin, GPIO_PULLDOWN_DISABLE);
        break;
    case PORT_INPUT_PULLUP_10K:
        gpio_hw_set_pull_up(port, pin, GPIO_PULLUP_10K);
        gpio_hw_set_pull_down(port, pin, GPIO_PULLDOWN_DISABLE);
        gpio_hw_set_direction(port, pin, 1);//0:out, 1:in
        gpio_hw_set_die(port, pin, 1);
        gpio_hw_set_dieh(port, pin, 1);
        break;
    case PORT_INPUT_PULLUP_100K:
        gpio_hw_set_pull_up(port, pin, GPIO_PULLUP_100K);
        gpio_hw_set_pull_down(port, pin, GPIO_PULLDOWN_DISABLE);
        gpio_hw_set_direction(port, pin, 1);//0:out, 1:in
        gpio_hw_set_die(port, pin, 1);
        gpio_hw_set_dieh(port, pin, 1);
        break;
    case PORT_INPUT_PULLUP_1M:
        gpio_hw_set_pull_up(port, pin, GPIO_PULLUP_1M);
        gpio_hw_set_pull_down(port, pin, GPIO_PULLDOWN_DISABLE);
        gpio_hw_set_direction(port, pin, 1);//0:out, 1:in
        gpio_hw_set_die(port, pin, 1);
        gpio_hw_set_dieh(port, pin, 1);
        break;
    case PORT_INPUT_PULLDOWN_10K:
        gpio_hw_set_pull_up(port, pin, GPIO_PULLUP_DISABLE);
        gpio_hw_set_pull_down(port, pin, GPIO_PULLDOWN_10K);
        gpio_hw_set_direction(port, pin, 1);//0:out, 1:in
        gpio_hw_set_die(port, pin, 1);
        gpio_hw_set_dieh(port, pin, 1);
        break;
    case PORT_INPUT_PULLDOWN_100K:
        gpio_hw_set_pull_up(port, pin, GPIO_PULLUP_DISABLE);
        gpio_hw_set_pull_down(port, pin, GPIO_PULLDOWN_100K);
        gpio_hw_set_direction(port, pin, 1);//0:out, 1:in
        gpio_hw_set_die(port, pin, 1);
        gpio_hw_set_dieh(port, pin, 1);
        break;
    case PORT_INPUT_PULLDOWN_1M:
        gpio_hw_set_pull_up(port, pin, GPIO_PULLUP_DISABLE);
        gpio_hw_set_pull_down(port, pin, GPIO_PULLDOWN_1M);
        gpio_hw_set_direction(port, pin, 1);//0:out, 1:in
        gpio_hw_set_die(port, pin, 1);
        gpio_hw_set_dieh(port, pin, 1);
        break;
    default:
        log_error("%s(), param:mode error!", __func__);
        return -1;//error
    }
    return 0;
}

int gpio_init(enum gpio_port port, const struct gpio_config *config)
{
    /* log_info("%s():%d", __func__, __LINE__); */
    gpio_set_mode(port, config->pin, config->mode);
    gpio_hw_set_drive_strength(port, config->pin, config->hd);
    return 0;
}

__attribute__((always_inline_when_const_args))
int gpio_deinit(enum gpio_port port, u32 pin)
{
    if (gpio_hw_port_pin_judge(port, pin) < 0) {
        return -1;
    }
    gpio_hw_set_direction(port, pin, 1);//0:out, 1:in
    gpio_hw_set_die(port, pin, 0);
    gpio_hw_set_dieh(port, pin, 0);
    /* gpio_hw_write_port(port, pin, 0);//out 0 */
    gpio_hw_set_pull_up(port, pin, GPIO_PULLUP_DISABLE);
    gpio_hw_set_pull_down(port, pin, GPIO_PULLDOWN_DISABLE);
    gpio_hw_set_drive_strength(port, pin, PORT_DRIVE_STRENGT_2p4mA);
    return 0;
}

__attribute__((always_inline_when_const_args))
int gpio_keep_mode_at_sleep(enum gpio_port port, u32 pin)
{
    gpio_hw_set_dieh(port, pin, 1);
    return 0;
}

enum gpio_mode gpio_get_mode(enum gpio_port port, u32 pin)
{
    ASSERT(!(pin & (pin - 1)), "%s():Parameter pin error!It must be one io!", __func__);

    return 0;
}

// gpio输入值获取 gpio:IO_PORTA_00
__attribute__((always_inline_when_const_args))
int gpio_read(u32 gpio)
{
    return gpio_hw_read(gpio);
}
__attribute__((always_inline_when_const_args))
int gpio_read_port(enum gpio_port port, u32 pin)
{
    return gpio_hw_read_port(port, pin);
}

// gpio输出电平设置 gpio:IO_PORTA_00
__attribute__((always_inline_when_const_args))
AT(.gpio.text.cache.L2)
int gpio_write(u32 gpio, u32 value)
{
    gpio_hw_write(gpio, value);
    return 0;
}
__attribute__((always_inline_when_const_args))
int gpio_write_port(enum gpio_port port, u32 pin, int out_state)
{
    gpio_hw_write_port(port, pin, out_state);
    return 0;
}

//调用此函数前需先设置对应io为输出
__attribute__((always_inline_when_const_args))
int gpio_toggle_port(enum gpio_port port, u32 pin)
{
    pin &= PORT_PIN_MASK;
    switch (port) {
#ifdef GPIOA
    case PORTA:
        pin &= IO_PORT_PA_MASK;
        _toggle_port(PA, pin);
        break;
#endif
#ifdef GPIOB
    case PORTB:
        pin &= IO_PORT_PB_MASK;
        _toggle_port(PB, pin);
        break;
#endif
#ifdef GPIOC
    case PORTC:
        pin &= IO_PORT_PC_MASK;
        _toggle_port(PC, pin);
        break;
#endif
#ifdef GPIOD
    case PORTD:
        pin &= IO_PORT_PD_MASK;
        _toggle_port(PD, pin);
        break;
#endif
#ifdef GPIOE
    case PORTE:
        pin &= IO_PORT_PE_MASK;
        _toggle_port(PE, pin);
        break;
#endif
#ifdef GPIOF
    case PORTF:
        pin &= IO_PORT_PF_MASK;
        _toggle_port(PF, pin);
        break;
#endif
#ifdef GPIOG
    case PORTG:
        pin &= IO_PORT_PG_MASK;
        _toggle_port(PG, pin);
        break;
#endif
#ifdef GPIOH
    case PORTH:
        pin &= IO_PORT_PH_MASK;
        _toggle_port(PH, pin);
        break;
#endif
#ifdef GPIOP
    case PORTP:
        pin &= IO_PORT_PP_MASK;
        _toggle_port(PP, pin);
        break;
#endif
#ifdef GPIOUSB
    case PORTUSB:
        pin &= IO_PORT_USB_MASK;
        _toggle_port(PU, pin);
        break;
#endif
#ifdef GPIOR
    case PORTR:
        pin &= IO_PORT_PR_MASK;
        _toggle_port(PR, pin);
        break;
#endif
    default:
        log_error("%s(), param:port error!", __func__);
        return -1;//error
    }
    return 0;
}

// 获取gpio输出电平
__attribute__((always_inline_when_const_args))
int gpio_get_out_level(enum gpio_port port, u32 pin)
{
    return gpio_hw_read_out_level(port, pin);
}

// 设置gpio引脚输出强度
__attribute__((always_inline_when_const_args))
AT(.gpio.text.cache.L2)
int gpio_set_drive_strength(enum gpio_port port, u32 pin, enum gpio_drive_strength drive)
{
    gpio_hw_set_drive_strength(port, pin, drive);
    return 0;
}

// 获取gpio输出强度 pin:只能带入1个io
enum gpio_drive_strength  gpio_get_drive_strength(enum gpio_port port, u32 pin)
{
    ASSERT(!(pin & (pin - 1)), "%s():Parameter pin error!It must be one io!", __func__);
    if (gpio_hw_port_pin_judge(port, pin) < 0) {
        return -1;
    }
    u32 hd_value = gpio_hw_read_drive_strength(port, pin);//return hd1:高16位,  hd0:低16位
    u8 gpio_d_strength = 0;
    for (u8 i = 0; i < 16; i++) { //group num:16
        if (pin & BIT(i)) {
#if defined(GPIO_NEW_PUPDHD_EN) && GPIO_NEW_PUPDHD_EN
            gpio_d_strength  = (hd_value >> (i * 2)) & 0x03;
#else
            gpio_d_strength  = ((hd_value & BIT(i + 16)) >> i >> 15) | ((hd_value & BIT(i)) >> i);
#endif
            /* printf("PORT%c:%d,drive_strength:%d(0x%x)\n", port + 'A', i, gpio_d_strength, hd_value); */
            return gpio_d_strength;//return 第一个io
        }
    }
    return gpio_d_strength;//return 最后一个io
}

/***************************br27 br50 test code****************************/
#if 0//port function test
/*{{{*/
void wdt_clear();
#define LOG_FORMAT "0x%4x  0x%4x  0x%4x  0x%4x,0x%4x  0x%4x,0x%4x  0x%4x,0x%4x"
#ifdef GPIOA
#define LOG_PORTA JL_PORTA->DIR,JL_PORTA->DIE,JL_PORTA->OUT,JL_PORTA->PU0,JL_PORTA->PU1,JL_PORTA->PD0,JL_PORTA->PD1,JL_PORTA->HD0,JL_PORTA->HD1
#endif
#ifdef GPIOB
#define LOG_PORTB JL_PORTB->DIR,JL_PORTB->DIE,JL_PORTB->OUT,JL_PORTB->PU0,JL_PORTB->PU1,JL_PORTB->PD0,JL_PORTB->PD1,JL_PORTB->HD0,JL_PORTB->HD1
#endif
#ifdef GPIOC
#define LOG_PORTC JL_PORTC->DIR,JL_PORTC->DIE,JL_PORTC->OUT,JL_PORTC->PU0,JL_PORTC->PU1,JL_PORTC->PD0,JL_PORTC->PD1,JL_PORTC->HD0,JL_PORTC->HD1
#endif
#ifdef GPIOD
#define LOG_PORTD JL_PORTD->DIR,JL_PORTD->DIE,JL_PORTD->OUT,JL_PORTD->PU0,JL_PORTD->PU1,JL_PORTD->PD0,JL_PORTD->PD1,JL_PORTD->HD0,JL_PORTD->HD1
#endif
#ifdef GPIOP
#define LOG_PORTP JL_PORTP->DIR,JL_PORTP->DIE,JL_PORTP->OUT,JL_PORTP->PU0,JL_PORTP->PU1,JL_PORTP->PD0,JL_PORTP->PD1,JL_PORTP->HD0,JL_PORTP->HD1
#endif
#ifdef GPIOR
#define LOG_PORTR R3_PR_DIR,R3_PR_DIE,R3_PR_OUT,R3_PR_PU0,R3_PR_PU1,R3_PR_PD0,R3_PR_PD1,R3_PR_HD0,R3_PR_HD1
#endif
#ifdef GPIOUSB
#define LOG_PORTU _portx(PU,dir),_portx(PU,die),_portx(PU,out),_portx(PU,pu0),_portx(PU,pu1),_portx(PU,pd0),_portx(PU,pd0),_portx(PU,pd0),_portx(PU,pd0)
#endif
void gpio_log()
{
    printf("port   :dir     die     out     pu0   ,pu1     pd0   ,pd1     hd0   ,hd1\n");
    printf("porta  :"LOG_FORMAT"\n", LOG_PORTA);
    /* printf("portb  :"LOG_FORMAT"\n", LOG_PORTB); */
    printf("portc  :"LOG_FORMAT"\n", LOG_PORTC);
    printf("portd  :"LOG_FORMAT"\n", LOG_PORTD);
    /* printf("portp  :"LOG_FORMAT"\n", LOG_PORTP); */
#ifdef GPIOR
    printf("portr  :"LOG_FORMAT"\n", LOG_PORTR);
#endif
    /* printf("portusb:0x%4x 0x%4x 0x%4x 0x%4x,0x%4x 0x%4x,0x%4x 0x%4x,0x%4x\n",JL_HUSB_IO,,,,,,,,); */
    printf("portusb:0x%4x  0x%4x  0x%4x  0x%4x,0x%4x  0x%4x,no0x%4x0x%4x,0x%4x\n\n\n", LOG_PORTU);

}


#define TEST_PORT     PORTUSB
#define TEST_LOG_PORT LOG_PORTU
#define TEST_PIN   0xffff//pa:3fdf,pb:0xfffd,pc:0x0fff(0x1fff),pd:0x7f,pp:0x1,pr:0x3,pusb:0x03
struct gpio_config gpio_config_test = {
    .pin = TEST_PIN,//PORT_PIN_0 | PORT_PIN_1 | PORT_PIN_2 | PORT_PIN_3,//多个IO一起初始化
    .mode = PORT_OUTPUT_LOW,//PORT_INPUT_PULLUP_10K,
    .hd = PORT_DRIVE_STRENGT_2p4mA,
};
void gpio__test()
{
    printf("*****************port test*******************\n");
    if (gpio_hw_set_direction(PORTA, 0x1, 0) < 0) { //0:out, 1:in
        printf("\nset err\n");
    }
    gpio_log();
    gpio_init(TEST_PORT, &gpio_config_test);
    gpio_log();

    gpio_toggle_port(TEST_PORT, gpio_config_test.pin);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_toggle_port(TEST_PORT, gpio_config_test.pin);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_toggle_port(TEST_PORT, gpio_config_test.pin);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_toggle_port(TEST_PORT, gpio_config_test.pin);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_get_drive_strength(TEST_PORT, gpio_config_test.pin);
    gpio_hw_set_pull_up(TEST_PORT, gpio_config_test.pin, GPIO_PULLUP_DISABLE);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_hw_set_pull_up(TEST_PORT, gpio_config_test.pin, GPIO_PULLUP_10K);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_hw_set_pull_up(TEST_PORT, gpio_config_test.pin, GPIO_PULLUP_100K);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_hw_set_pull_up(TEST_PORT, gpio_config_test.pin, GPIO_PULLUP_1M);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_hw_set_pull_up(TEST_PORT, gpio_config_test.pin, GPIO_PULLUP_DISABLE);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);

    int out_value = 0;
    gpio_write_port(TEST_PORT, gpio_config_test.pin, 1);
    out_value = gpio_get_out_level(TEST_PORT, gpio_config_test.pin);
    printf("out_value:0x%x\n", out_value);
    gpio_write_port(TEST_PORT, gpio_config_test.pin, 0);
    out_value = gpio_get_out_level(TEST_PORT, gpio_config_test.pin);
    printf("out_value:0x%x\n", out_value);
    gpio_set_drive_strength(TEST_PORT, gpio_config_test.pin, PORT_DRIVE_STRENGT_8p0mA);
    gpio_get_drive_strength(TEST_PORT, gpio_config_test.pin);
    gpio_set_drive_strength(TEST_PORT, gpio_config_test.pin, PORT_DRIVE_STRENGT_24p0mA);
    gpio_get_drive_strength(TEST_PORT, gpio_config_test.pin);
    gpio_set_drive_strength(TEST_PORT, gpio_config_test.pin, PORT_DRIVE_STRENGT_64p0mA);
    gpio_get_drive_strength(TEST_PORT, gpio_config_test.pin);


    gpio_config_test.mode = PORT_INPUT_PULLUP_10K;//PORT_INPUT_PULLDOWN_10K
    gpio_init(TEST_PORT, &gpio_config_test);
    gpio_log();
    printf("gpio_read dm:0x%x\n", gpio_read(IO_PORT_DM));
    printf("gpio_read_port:0x%x\n", gpio_read_port(TEST_PORT, gpio_config_test.pin));
}/*}}}*/
#endif
/***************************br28 br36 test code****************************/
#if 0//port function test
/*{{{*/
void wdt_clear();
#define LOG_FORMAT "0x%4x  0x%4x  0x%4x  0x%4x,  0x%4x,  0x%4x,0x%4x"
#ifdef GPIOA
#define LOG_PORTA JL_PORTA->DIR,JL_PORTA->DIE,JL_PORTA->OUT,JL_PORTA->PU,JL_PORTA->PD,JL_PORTA->HD0,JL_PORTA->HD
#endif
#ifdef GPIOB
#define LOG_PORTB JL_PORTB->DIR,JL_PORTB->DIE,JL_PORTB->OUT,JL_PORTB->PU,JL_PORTB->PD,JL_PORTB->HD0,JL_PORTB->HD
#endif
#ifdef GPIOC
#define LOG_PORTC JL_PORTC->DIR,JL_PORTC->DIE,JL_PORTC->OUT,JL_PORTC->PU,JL_PORTC->PD,JL_PORTC->HD0,JL_PORTC->HD
#endif
#ifdef GPIOD
#define LOG_PORTD JL_PORTD->DIR,JL_PORTD->DIE,JL_PORTD->OUT,JL_PORTD->PU,JL_PORTD->PD,JL_PORTD->HD0,JL_PORTD->HD
#endif
#ifdef GPIOE
#define LOG_PORTE JL_PORTE->DIR,JL_PORTE->DIE,JL_PORTE->OUT,JL_PORTE->PU,JL_PORTE->PD,JL_PORTE->HD0,JL_PORTE->HD
#endif
#ifdef GPIOG
#define LOG_PORTG JL_PORTG->DIR,JL_PORTG->DIE,JL_PORTG->OUT,JL_PORTG->PU,JL_PORTG->PD,JL_PORTG->HD0,JL_PORTG->HD
#endif
#ifdef GPIOP
#define LOG_PORTP JL_PORTP->DIR,JL_PORTP->DIE,JL_PORTP->OUT,JL_PORTP->PU,JL_PORTP->PD,JL_PORTP->HD0,JL_PORTP->HD
#endif
#ifdef GPIOUSB
#define LOG_PORTU _portx(PU,dir),_portx(PU,die),_portx(PU,out),_portx(PU,pu0),_portx(PU,pd0),_portx(PU,pd0),_portx(PU,pd0)
#endif
void gpio_log()
{
    printf("port   :dir     die     out     pu0   ,  pd0   ,  hd0   ,hd1\n");
    printf("porta  :"LOG_FORMAT"\n", LOG_PORTA);
    printf("portb  :"LOG_FORMAT"\n", LOG_PORTB);
    printf("portc  :"LOG_FORMAT"\n", LOG_PORTC);
    printf("portd  :"LOG_FORMAT"\n", LOG_PORTD);
#ifdef GPIOE
    printf("porte  :"LOG_FORMAT"\n", LOG_PORTE);
#endif
#ifdef GPIOG
    printf("portg  :"LOG_FORMAT"\n", LOG_PORTG);
#endif
    printf("portp  :"LOG_FORMAT"\n", LOG_PORTP);
    /* printf("portusb:0x%4x 0x%4x 0x%4x 0x%4x,0x%4x 0x%4x,0x%4x 0x%4x,0x%4x\n",JL_HUSB_IO,,,,,,,,); */
    printf("portusb:0x%4x  0x%4x  0x%4x  0x%4x,  0x%4x,no 0x%4x,0x%4x\n\n\n", LOG_PORTU);

}


#define TEST_PORT     PORTB
#define TEST_LOG_PORT LOG_PORTB
#define TEST_PIN   0xffff//pa:3fdf,pb:0xfffd,pc:0x0fff(0x1fff),pd:0x7f,pp:0x1,pr:0x3,pusb:0x03
struct gpio_config gpio_config_test = {
    .pin = TEST_PIN,//PORT_PIN_0 | PORT_PIN_1 | PORT_PIN_2 | PORT_PIN_3,//多个IO一起初始化
    .mode = PORT_OUTPUT_LOW,//PORT_INPUT_PULLUP_10K,
    .hd = PORT_DRIVE_STRENGT_2p4mA,
};
void gpio__test()
{
    printf("*****************port test*******************\n");
    if (gpio_hw_set_direction(PORTA, 0x1, 0) < 0) { //0:out, 1:in
        printf("\nset err\n");
    }
    gpio_log();
    gpio_init(TEST_PORT, &gpio_config_test);
    gpio_log();

    gpio_toggle_port(TEST_PORT, gpio_config_test.pin);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_toggle_port(TEST_PORT, gpio_config_test.pin);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_toggle_port(TEST_PORT, gpio_config_test.pin);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_toggle_port(TEST_PORT, gpio_config_test.pin);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_get_drive_strength(TEST_PORT, gpio_config_test.pin);
    gpio_hw_set_pull_up(TEST_PORT, gpio_config_test.pin, GPIO_PULLUP_DISABLE);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_hw_set_pull_up(TEST_PORT, gpio_config_test.pin, GPIO_PULLUP_10K);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_hw_set_pull_up(TEST_PORT, gpio_config_test.pin, GPIO_PULLUP_DISABLE);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);
    gpio_hw_set_pull_up(TEST_PORT, gpio_config_test.pin, GPIO_PULLUP_10K);
    printf("port%c  :"LOG_FORMAT"\n", TEST_PORT + 'A', TEST_LOG_PORT);

    int out_value = 0;
    gpio_write_port(TEST_PORT, gpio_config_test.pin, 1);
    out_value = gpio_get_out_level(TEST_PORT, gpio_config_test.pin);
    printf("out_value:0x%x\n", out_value);
    gpio_write_port(TEST_PORT, gpio_config_test.pin, 0);
    out_value = gpio_get_out_level(TEST_PORT, gpio_config_test.pin);
    printf("out_value:0x%x\n", out_value);
    gpio_set_drive_strength(TEST_PORT, gpio_config_test.pin, PORT_DRIVE_STRENGT_8p0mA);
    gpio_get_drive_strength(TEST_PORT, gpio_config_test.pin);
    gpio_set_drive_strength(TEST_PORT, gpio_config_test.pin, PORT_DRIVE_STRENGT_24p0mA);
    gpio_get_drive_strength(TEST_PORT, gpio_config_test.pin);
    gpio_set_drive_strength(TEST_PORT, gpio_config_test.pin, PORT_DRIVE_STRENGT_64p0mA);
    gpio_get_drive_strength(TEST_PORT, gpio_config_test.pin);


    gpio_config_test.mode = PORT_INPUT_PULLUP_10K;//PORT_INPUT_PULLDOWN_10K
    gpio_init(TEST_PORT, &gpio_config_test);
    gpio_log();
    printf("gpio_read dm:0x%x\n", gpio_read(IO_PORT_DM));
    printf("gpio_read_port:0x%x\n", gpio_read_port(TEST_PORT, gpio_config_test.pin));
}/*}}}*/
#endif

