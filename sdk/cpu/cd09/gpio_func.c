#define LOG_TAG_CONST   GPIO
#define LOG_TAG         "[gpio_f]"
#include "debug.h"
#include "gpio.h"

const static u16 gptimer_pwm_fix_io_table[] = {
    IO_PORTD_08,
    IO_PORTD_09,
    IO_PORTD_10,
    IO_PORTD_11,
};

/***************************port peripheral code****************************/
// pin:只能带入1个io
__attribute__((always_inline_when_const_args))
int gpio_set_function(enum gpio_port port, u32 pin, enum gpio_function fn)
{
    /*{{{*/
    if (!pin) {
        log_error("gpio_set_function():Invalid pin !\n");
        return -1;//err
    }
    ASSERT(!(pin & (pin - 1)), "%s():Parameter pin error!It must be one io!", __func__);
    if (gpio_hw_port_pin_judge(port, pin) < 0) {
        return -1;
    }
    char cnt = -1;
    u32 pin_temp = pin;
    while (pin_temp) {
        pin_temp >>= 1;
        cnt++;
    }
    switch (fn) {
    case PORT_FUNC_UART0_TX:
        /* gpio_set_mode(port, pin, PORT_OUTPUT_HIGH); */
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_UART0_TX, 1, 1);
        break;
    case PORT_FUNC_UART0_RX:
        /* gpio_set_mode(port, pin, PORT_INPUT_PULLUP_10K); */
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_UART0_RX);
        break;
    case PORT_FUNC_UART1_TX:
        /* gpio_set_mode(port, pin, PORT_OUTPUT_HIGH); */
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_UART1_TX, 1, 1);
        break;
    case PORT_FUNC_UART1_RX:
        /* gpio_set_mode(port, pin, PORT_INPUT_PULLUP_10K); */
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_UART1_RX);
        break;
    case PORT_FUNC_UART2_RTS:
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_UART2_RTS, 1, 1);
        /* gpio_och_sel_output_signal(port * IO_GROUP_NUM + cnt, OUTPUT_CH_SIGNAL_UART1_RTS); */
        break;
    case PORT_FUNC_UART2_CTS:
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_UART2_CTS);
        /* gpio_ich_sel_input_signal(port * IO_GROUP_NUM + cnt, INPUT_CH_SIGNAL_UART1_CTS, INPUT_CH_TYPE_GP_ICH); */
        break;
    case PORT_FUNC_UART2_TX:
        /* gpio_set_mode(port, pin, PORT_OUTPUT_HIGH); */
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_UART2_TX, 1, 1);
        break;
    case PORT_FUNC_UART2_RX:
        /* gpio_set_mode(port, pin, PORT_INPUT_PULLUP_10K); */
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_UART2_RX);
        break;
#if 0
    case PORT_FUNC_SPI0_CLK:
        /* gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI0_CLK, 1, 1); */
        /* gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI0_CLK); */
        break;
    case PORT_FUNC_SPI0_DA0:
        /* gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI0_DA0, 1, 1); */
        /* gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI0_DA0); */
        break;
    case PORT_FUNC_SPI0_DA1:
        /* gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI0_DA1, 1, 1); */
        /* gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI0_DA1); */
        break;
    case PORT_FUNC_SPI0_DA2:
        /* gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI0_DA2, 1, 1); */
        /* gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI0_DA2); */
        break;
    case PORT_FUNC_SPI0_DA3:
        /* gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI0_DA3, 1, 1); */
        /* gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI0_DA3); */
        break;
#endif
    case PORT_FUNC_SPI1_CLK:
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI1_CLK, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI1_CLK);
        break;
    case PORT_FUNC_SPI1_DA0:
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI1_DA0, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI1_DA0);
        break;
    case PORT_FUNC_SPI1_DA1:
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI1_DA1, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI1_DA1);
        break;
    case PORT_FUNC_SPI1_DA2:
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI1_DA2, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI1_DA2);
        break;
    case PORT_FUNC_SPI1_DA3:
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI1_DA3, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI1_DA3);
        break;
    case PORT_FUNC_SPI2_CLK:
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI2_CLK, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI2_CLK);
        break;
    case PORT_FUNC_SPI2_DA0:
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI2_DA0, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI2_DA0);
        break;
    case PORT_FUNC_SPI2_DA1:
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SPI2_DA1, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SPI2_DA1);
        break;

    case PORT_FUNC_IIC0_SCL:
        /* gpio_set_mode(port, pin, PORT_INPUT_PULLUP_10K); //PORT_INPUT_FLOATING */
        gpio_hw_set_spl(port, pin, 1);
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_IIC0_SCL, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_IIC0_SCL);
        break;
    case PORT_FUNC_IIC0_SDA:
        /* gpio_set_mode(port, pin, PORT_INPUT_PULLUP_10K); //PORT_INPUT_FLOATING */
        gpio_hw_set_spl(port, pin, 1);
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_IIC0_SDA, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_IIC0_SDA);
        break;
    case PORT_FUNC_IIC1_SCL:
        /* gpio_set_mode(port, pin, PORT_INPUT_PULLUP_10K); //PORT_INPUT_FLOATING */
        gpio_hw_set_spl(port, pin, 1);
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_IIC1_SCL, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_IIC1_SCL);
        break;
    case PORT_FUNC_IIC1_SDA:
        /* gpio_set_mode(port, pin, PORT_INPUT_PULLUP_10K); //PORT_INPUT_FLOATING */
        gpio_hw_set_spl(port, pin, 1);
        gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_IIC1_SDA, 1, 1);
        gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_IIC1_SDA);
        break;

    /* case PORT_FUNC_SD0_CLK: */
    /*     gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SD0_CLK, 1, 1); */
    /*     break; */
    /* case PORT_FUNC_SD0_CMD: */
    /*     gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SD0_CMD, 1, 1); */
    /*     gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SD0_CMD); */
    /*     break; */
    /* case PORT_FUNC_SD0_DA0: */
    /*     gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_SD0_DA0, 1, 1); */
    /*     gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_SD0_DA0); */
    /*     break; */

    case PORT_FUNC_GPADC:
        /* gpio_set_mode(port, pin, PORT_INPUT_PULLUP_10K); //PORT_HIGHZ */
        gpio_hw_set_die(port, pin, 0);
        break;
    case PORT_FUNC_PWM_LED:
        gpio_hw_set_pull_down(port, pin, GPIO_PULLDOWN_1M);//portabcd:pd0,pd1,usb:pd0
        gpio_hw_set_die(port, pin, 1);
        gpio_hw_set_direction(port, pin, 0);//0:out, 1:in
        gpio_hw_set_pull_up(port, pin, GPIO_PULLUP_1M);
        gpio_hw_write_port(port, pin, 1);
        break;

    /* case PORT_FUNC_PLNK_SCLK: */
    /*     gpio_set_fun_output_port(port * IO_GROUP_NUM + cnt, FO_PLNK_SCLK, 0, 1); */
    /*     gpio_set_mode(port, pin, PORT_OUTPUT_HIGH); */
    /*     break; */
    /* case PORT_FUNC_PLNK_DAT0: */
    /*     gpio_set_mode(port, pin, PORT_INPUT_PULLUP_10K); */
    /*     gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_PLNK_DAT0); */
    /*     break; */
    /* case PORT_FUNC_PLNK_DAT1: */
    /*     gpio_set_mode(port, pin, PORT_INPUT_PULLUP_10K); */
    /*     gpio_set_fun_input_port(port * IO_GROUP_NUM + cnt, PFI_PLNK_DAT1); */
    /*     break; */

    case PORT_FUNC_OCH_CLOCK_OUT0:
        gpio_set_mode(port, pin, PORT_OUTPUT_LOW);
        gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP0_CLK_OUT0);
        break;
    case PORT_FUNC_OCH_CLOCK_OUT1:
        gpio_set_mode(port, pin, PORT_OUTPUT_LOW);
        gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP1_CLK_OUT1);
        break;
    case PORT_FUNC_OCH_CLOCK_OUT2:
        gpio_set_mode(port, pin, PORT_OUTPUT_LOW);
        gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP2_CLK_OUT2);
        break;

    case PORT_FUNC_TIMER0_PWM:
        if (port * IO_GROUP_NUM + cnt != gptimer_pwm_fix_io_table[0]) {
            gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP4_TIMER0_PWM);
        } else {
            JL_IOMC->IOMC0 |= BIT(24);
        }
        break;
    case PORT_FUNC_TIMER1_PWM:
        if (port * IO_GROUP_NUM + cnt != gptimer_pwm_fix_io_table[1]) {
            gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP5_TIMER1_PWM);
        } else {
            JL_IOMC->IOMC0 |= BIT(25);
        }
        break;
    case PORT_FUNC_TIMER2_PWM:
        if (port * IO_GROUP_NUM + cnt != gptimer_pwm_fix_io_table[2]) {
            gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP6_TIMER2_PWM);
        } else {
            JL_IOMC->IOMC0 |= BIT(26);
        }
        break;
    case PORT_FUNC_TIMER3_PWM:
        if (port * IO_GROUP_NUM + cnt != gptimer_pwm_fix_io_table[3]) {
            gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP7_TIMER3_PWM);
        } else {
            JL_IOMC->IOMC0 |= BIT(27);
        }
        break;
    case PORT_FUNC_TIMER0_CAPTURE:
        gpio_mux_in(port * IO_GROUP_NUM + cnt, GPIO_INPUT_ICH1_TIMER0_CAP);
        break;
    case PORT_FUNC_TIMER1_CAPTURE:
        gpio_mux_in(port * IO_GROUP_NUM + cnt, GPIO_INPUT_ICH3_TIMER1_CAP);
        break;
    case PORT_FUNC_TIMER2_CAPTURE:
        gpio_mux_in(port * IO_GROUP_NUM + cnt, GPIO_INPUT_ICH5_TIMER2_CAP);
        break;
    case PORT_FUNC_TIMER3_CAPTURE:
        gpio_mux_in(port * IO_GROUP_NUM + cnt, GPIO_INPUT_ICH7_TIMER3_CAP);
        break;
    case PORT_FUNC_IRFLT_0:
    case PORT_FUNC_IRFLT_1:
    case PORT_FUNC_IRFLT_2:
    case PORT_FUNC_IRFLT_3:
        gpio_mux_in(port * IO_GROUP_NUM + cnt, GPIO_INPUT_ICH2_IRFLT);
        break;
    /* case PORT_FUNC_CLK_PIN: */
    /*     gpio_ich_sel_input_signal(port * IO_GROUP_NUM + cnt, INPUT_CH_SIGNAL_CLK_PIN, INPUT_CH_TYPE_GP_ICH); */
    /*     break; */
    case PORT_FUNC_PORT_WKUP:
        break;
    case PORT_FUNC_MCPWM0_H:
        gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP0_MC_PWM0_H);
        break;
    case PORT_FUNC_MCPWM0_L:
        gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP1_MC_PWM0_L);
        break;
    case PORT_FUNC_MCPWM1_H:
        gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP2_MC_PWM1_H);
        break;
    case PORT_FUNC_MCPWM1_L:
        gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP3_MC_PWM1_L);
        break;
    case PORT_FUNC_MCPWM2_H:
        gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP4_MC_PWM2_H);
        break;
    case PORT_FUNC_MCPWM2_L:
        gpio_mux_out(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP5_MC_PWM2_L);
        break;
    /* case PORT_FUNC_MCPWM0_FP: */
    /*     gpio_ich_sel_input_signal(port * IO_GROUP_NUM + cnt, INPUT_CH_SIGNAL_MCPWM0_FP, INPUT_CH_TYPE_GP_ICH); */
    /*     break; */
    /* case PORT_FUNC_MCPWM1_FP: */
    /*     gpio_ich_sel_input_signal(port * IO_GROUP_NUM + cnt, INPUT_CH_SIGNAL_MCPWM1_FP, INPUT_CH_TYPE_GP_ICH); */
    /*     break; */
    /* case PORT_FUNC_LEDC0_OUT: */
    /*     gpio_och_sel_output_signal(port * IO_GROUP_NUM + cnt, OUTPUT_CH_SIGNAL_LEDC0_OUT); */
    /*     break; */
    /* case PORT_FUNC_LEDC1_OUT: */
    /*     gpio_och_sel_output_signal(port * IO_GROUP_NUM + cnt, OUTPUT_CH_SIGNAL_LEDC1_OUT); */
    case PORT_FUNC_RDEC0_PORT0:
        gpio_mux_in(port * IO_GROUP_NUM + cnt, GPIO_INPUT_ICH0_QDEC_PHASE_A);
        break;
    case PORT_FUNC_RDEC0_PORT1:
        gpio_mux_in(port * IO_GROUP_NUM + cnt, GPIO_INPUT_ICH4_QDEC_PHASE_B);
        break;
    default:
        break;

    }
    return 0;
}
// pin:只能带入1个io
__attribute__((always_inline_when_const_args))
int gpio_disable_function(enum gpio_port port, u32 pin, enum gpio_function fn)
{
    if (!pin) {
        log_error("gpio_set_function():Invalid pin !\n");
        return -1;//err
    }
    ASSERT(!(pin & (pin - 1)), "%s():Parameter pin error!It must be one io!", __func__);
    if (gpio_hw_port_pin_judge(port, pin) < 0) {
        return -1;
    }
    char cnt = -1;
    u32 pin_temp = pin;
    while (pin_temp) {
        pin_temp >>= 1;
        cnt++;
    }
    gpio_deinit(port, pin);
    switch (fn) {
    case PORT_FUNC_UART0_TX:
    case PORT_FUNC_UART1_TX:
    case PORT_FUNC_UART2_TX:
        /* case PORT_FUNC_SD0_CLK: */
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        break;
    case PORT_FUNC_UART0_RX:
        gpio_disable_fun_input_port(PFI_UART0_RX);
        break;
    case PORT_FUNC_UART1_RX:
        gpio_disable_fun_input_port(PFI_UART1_RX);
        break;
    case PORT_FUNC_UART2_CTS:
        gpio_disable_fun_input_port(PFI_UART2_CTS);
        /* gpio_ich_disable_input_signal(port * IO_GROUP_NUM + cnt, INPUT_CH_SIGNAL_UART1_CTS, INPUT_CH_TYPE_GP_ICH); */
        break;
    case PORT_FUNC_UART2_RTS:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        /* gpio_och_disable_output_signal(port * IO_GROUP_NUM + cnt, OUTPUT_CH_SIGNAL_UART1_RTS); */
        break;
    case PORT_FUNC_UART2_RX:
        gpio_disable_fun_input_port(PFI_UART2_RX);
        break;
#if 0
    case PORT_FUNC_SPI0_CLK:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI0_CLK);
        break;
    case PORT_FUNC_SPI0_DA0:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI0_DA0);
        break;
    case PORT_FUNC_SPI0_DA1:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI0_DA1);
        break;
    case PORT_FUNC_SPI0_DA2:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI0_DA2);
        break;
    case PORT_FUNC_SPI0_DA3:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI0_DA3);
        break;
#endif
    case PORT_FUNC_SPI1_CLK:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI1_CLK);
        break;
    case PORT_FUNC_SPI1_DA0:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI1_DA0);
        break;
    case PORT_FUNC_SPI1_DA1:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI1_DA1);
        break;
    case PORT_FUNC_SPI1_DA2:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI1_DA2);
        break;
    case PORT_FUNC_SPI1_DA3:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI1_DA3);
        break;
    case PORT_FUNC_SPI2_CLK:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI2_CLK);
        break;
    case PORT_FUNC_SPI2_DA0:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI2_DA0);
        break;
    case PORT_FUNC_SPI2_DA1:
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_SPI2_DA1);
        break;
#if 1
    case PORT_FUNC_IIC0_SCL:
        gpio_hw_set_spl(port, pin, 0);
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_IIC0_SCL);
        break;
    case PORT_FUNC_IIC0_SDA:
        gpio_hw_set_spl(port, pin, 0);
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_IIC0_SDA);
        break;
    case PORT_FUNC_IIC1_SCL:
        gpio_hw_set_spl(port, pin, 0);
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_IIC1_SCL);
        break;
    case PORT_FUNC_IIC1_SDA:
        gpio_hw_set_spl(port, pin, 0);
        gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt);
        gpio_disable_fun_input_port(PFI_IIC1_SDA);
        break;
#endif
    /* case PORT_FUNC_SD0_CMD: */
    /*     gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt); */
    /*     gpio_disable_fun_input_port(PFI_SD0_CMD); */
    /*     break; */
    /* case PORT_FUNC_SD0_DA0: */
    /*     gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt); */
    /*     gpio_disable_fun_input_port(PFI_SD0_DA0); */
    /*     break; */

    case PORT_FUNC_GPADC:
        break;
    case PORT_FUNC_PWM_LED:
        gpio_hw_write_port(port, pin, 0);//out 0
        break;

    /* case PORT_FUNC_PLNK_SCLK: */
    /*     gpio_disable_fun_output_port(port * IO_GROUP_NUM + cnt); */
    /*     break; */
    /* case PORT_FUNC_PLNK_DAT0: */
    /*     gpio_disable_fun_input_port(PFI_PLNK_DAT0); */
    /*     break; */
    /* case PORT_FUNC_PLNK_DAT1: */
    /*     gpio_disable_fun_input_port(PFI_PLNK_DAT1); */
    /*     break; */

    case PORT_FUNC_OCH_CLOCK_OUT0:
        gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP0_CLK_OUT0);
        break;
    case PORT_FUNC_OCH_CLOCK_OUT1:
        gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP1_CLK_OUT1);
        break;
    case PORT_FUNC_OCH_CLOCK_OUT2:
        gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP2_CLK_OUT2);
        break;

    case PORT_FUNC_TIMER0_PWM:
        JL_IOMC->IOMC0 &= ~BIT(24);
        if (port * IO_GROUP_NUM + cnt != gptimer_pwm_fix_io_table[0]) {
            gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP4_TIMER0_PWM);
        }
        break;
    case PORT_FUNC_TIMER1_PWM:
        JL_IOMC->IOMC0 &= ~BIT(25);
        if (port * IO_GROUP_NUM + cnt != gptimer_pwm_fix_io_table[1]) {
            gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP5_TIMER1_PWM);
        }
        break;
    case PORT_FUNC_TIMER2_PWM:
        JL_IOMC->IOMC0 &= ~BIT(26);
        if (port * IO_GROUP_NUM + cnt != gptimer_pwm_fix_io_table[2]) {
            gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP6_TIMER2_PWM);
        }
        break;
    case PORT_FUNC_TIMER3_PWM:
        JL_IOMC->IOMC0 &= ~BIT(27);
        if (port * IO_GROUP_NUM + cnt != gptimer_pwm_fix_io_table[3]) {
            gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP7_TIMER3_PWM);
        }
        break;
    case PORT_FUNC_TIMER0_CAPTURE:
        gpio_mux_in_close(GPIO_INPUT_ICH1_TIMER0_CAP);
        break;
    case PORT_FUNC_TIMER1_CAPTURE:
        gpio_mux_in_close(GPIO_INPUT_ICH3_TIMER1_CAP);
        break;
    case PORT_FUNC_TIMER2_CAPTURE:
        gpio_mux_in_close(GPIO_INPUT_ICH5_TIMER2_CAP);
        break;
    case PORT_FUNC_TIMER3_CAPTURE:
        gpio_mux_in_close(GPIO_INPUT_ICH7_TIMER3_CAP);
        break;
    case PORT_FUNC_IRFLT_0:
    case PORT_FUNC_IRFLT_1:
    case PORT_FUNC_IRFLT_2:
    case PORT_FUNC_IRFLT_3:
        gpio_mux_in_close(GPIO_INPUT_ICH2_IRFLT);
        break;
    /* case PORT_FUNC_CLK_PIN: */
    /*     gpio_ich_disable_input_signal(port * IO_GROUP_NUM + cnt, INPUT_CH_SIGNAL_CLK_PIN, INPUT_CH_TYPE_GP_ICH); */
    /*     break; */
    case PORT_FUNC_PORT_WKUP:
        break;
    case PORT_FUNC_MCPWM0_H:
        gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP0_MC_PWM0_H);
        break;
    case PORT_FUNC_MCPWM0_L:
        gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP1_MC_PWM0_L);
        break;
    case PORT_FUNC_MCPWM1_H:
        gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP2_MC_PWM1_H);
        break;
    case PORT_FUNC_MCPWM1_L:
        gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP3_MC_PWM1_L);
        break;
    case PORT_FUNC_MCPWM2_H:
        gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP4_MC_PWM2_H);
        break;
    case PORT_FUNC_MCPWM2_L:
        gpio_mux_out_close(port * IO_GROUP_NUM + cnt, GPIO_OUT_COMP5_MC_PWM2_L);
        break;
    /* case PORT_FUNC_MCPWM0_FP: */
    /*     gpio_ich_disable_input_signal(port * IO_GROUP_NUM + cnt, INPUT_CH_SIGNAL_MCPWM0_FP, INPUT_CH_TYPE_GP_ICH); */
    /*     break; */
    /* case PORT_FUNC_MCPWM1_FP: */
    /*     gpio_ich_disable_input_signal(port * IO_GROUP_NUM + cnt, INPUT_CH_SIGNAL_MCPWM1_FP, INPUT_CH_TYPE_GP_ICH); */
    /*     break; */
    case PORT_FUNC_RDEC0_PORT0:
        gpio_mux_in_close(GPIO_INPUT_ICH0_QDEC_PHASE_A);
        break;
    case PORT_FUNC_RDEC0_PORT1:
        gpio_mux_in_close(GPIO_INPUT_ICH4_QDEC_PHASE_B);
        break;
    default:
        break;
    }
    return 0;
}
/*}}}*/


/***************************test code****************************/
#if 0

void wdt_clear();
void gpio_func_test()
{
    log_info("*****************port peripheral test*******************\n");

    gpio_set_function(PORTA, PORT_PIN_0, PORT_FUNC_UART1_TX);
    gpio_set_function(PORTA, PORT_PIN_2, PORT_FUNC_UART1_RX);
    gpio_disable_function(PORTA, PORT_PIN_0, PORT_FUNC_UART1_TX);
    gpio_disable_function(PORTA, PORT_PIN_2, PORT_FUNC_UART1_RX);
}
#endif

