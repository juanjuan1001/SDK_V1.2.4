#include "asm/mcpwm.h"
#include "gpio.h"
#include "asm/clock.h"

int usr_id0;
int usr_id1;
int usr_id2;

void usr_mcpwm_detect_callback(u32 ch)
{
    printf("usr detect ch %d\n", ch);
    mcpwm_fpnd_clr(ch); //检测到故障，手动清PND恢复
}

___interrupt
void mcpwm_timeout_callback(void)
{
    MCPWM_TIMERx_REG *timer_reg = NULL;
    timer_reg = (MCPWM_TIMERx_REG *)(MCPWM_TMR_BASE_ADDR);
    JL_PORTC->DIR &= ~BIT(7);
    if (timer_reg->tmr_con & BIT(MCPWM_TMR_OFPN)) {
        //溢出中断发生在H端低电平中点
        timer_reg->tmr_con |= BIT(MCPWM_TMR_OFCLR);
        JL_PORTC->OUT ^= BIT(7);
    }
    if (timer_reg->tmr_con & BIT(MCPWM_TMR_UFPND)) {
        //借位中断发生在H端高电平中点
        timer_reg->tmr_con |= BIT(MCPWM_TMR_UFCLR);
        JL_PORTC->OUT ^= BIT(7);
    }
}

void mcpwm_test_demo_start(void)
{
    struct mcpwm_config usr_mcpwm_cfg0 = {
        .ch = MCPWM_CH0,                        // 通道号
        .aligned_mode = MCPWM_CENTER_ALIGNED,   // 中心对齐
        .frequency = 1000,                      // 1KHz
        .duty = 5000,                           // 占空比50%
        .h_pin = IO_PORTC_00,                   // 任意引脚
        .l_pin = IO_PORTC_01,                   // 任意引脚,不需要就填0xff
        .complementary_en = 1,                  // 0: 同步, 1: 互补
        .detect_port = MCPWM_FPIN_IO_PORTC_11,  // 故障保护检测IO
        .edge = MCPWM_EDGE_FAILL,				// 下降沿
        .irq_cb = usr_mcpwm_detect_callback,	// 故障保护回调
        .irq_priority = 1,                 	 	// 优先级默认为1
        .dten = 1,								// 死区使能
        .dtpr = 0,								// 死区时间=Tsys/(2^dtckps)*(dtpr+1)
        .dtckps = 4,							// 时钟分频
        .ofie = 1,								// 溢出中断
        .ufie = 1,								// 借位中断
    };

    struct mcpwm_config usr_mcpwm_cfg1 = {
        .ch = MCPWM_CH1,                        // 通道号
        .aligned_mode = MCPWM_EDGE_ALIGNED,     // 边沿对齐
        .frequency = 1000,                      // 1KHz
        .duty = 5000,                           // 占空比50%
        .h_pin = IO_PORTC_02,                   // 任意引脚
        .l_pin = IO_PORTC_03,                   // 任意引脚,不需要就填0xff
        .complementary_en = 0,                  // 0: 同步, 1: 互补
        .detect_port = MCPWM_FPIN_IO_PORTC_11,  // 故障保护检测IO
        .edge = MCPWM_EDGE_FAILL,				// 下降沿
        .irq_cb = usr_mcpwm_detect_callback,	// 故障保护回调
        .irq_priority = 1,                 	 	// 优先级默认为1
        .dten = 0,								// 死区使能
        .dtpr = 0,								// 死区时间=Tsys/(2^dtckps)*(dtpr+1)
        .dtckps = 4,							// 时钟分频
        .ofie = 0,								// 溢出中断
        .ufie = 0,								// 借位中断
    };

    struct mcpwm_config usr_mcpwm_cfg2 = {
        .ch = MCPWM_CH2,                        // 通道号
        .aligned_mode = MCPWM_EDGE_ALIGNED,     // 边沿对齐
        .frequency = 1000,                      // 1KHz
        .duty = 5000,                           // 占空比50%
        .h_pin = IO_PORTC_04,                   // 任意引脚
        .l_pin = IO_PORTC_05,                   // 任意引脚,不需要就填0xff
        .complementary_en = 0,                  // 0: 同步, 1: 互补
        .detect_port = MCPWM_FPIN_IO_PORTC_11,  // 故障保护检测IO
        .edge = MCPWM_EDGE_FAILL,				// 下降沿
        .irq_cb = usr_mcpwm_detect_callback,	// 故障保护回调
        .irq_priority = 1,                 	 	// 优先级默认为1
        .dten = 0,								// 死区使能
        .dtpr = 0,								// 死区时间=Tsys/(2^dtckps)*(dtpr+1)
        .dtckps = 4,							// 时钟分频
        .ofie = 0,								// 溢出中断
        .ufie = 0,								// 借位中断
    };

    // 初始化分配id
    usr_id0 = mcpwm_init(&usr_mcpwm_cfg0);
    usr_id1 = mcpwm_init(&usr_mcpwm_cfg1);
    usr_id2 = mcpwm_init(&usr_mcpwm_cfg2);

#if 0
    // 顺序启动 三通道有相位差
    mcpwm_start(usr_id0);
    mcpwm_start(usr_id1);
    mcpwm_start(usr_id2);
#else
    // 同时启动 三通道无相位差
    mcpwm_mux_start(BIT(usr_id0) | BIT(usr_id1) | BIT(usr_id2));
#endif

    // 频率配置接口测试
    mcpwm_set_frequency(usr_id0, MCPWM_EDGE_ALIGNED, 2000); // 2K
    mcpwm_set_frequency(usr_id1, MCPWM_EDGE_ALIGNED, 4000); // 4K
    mcpwm_set_frequency(usr_id2, MCPWM_EDGE_ALIGNED, 8000); // 8K

#if 0
    // 百分比配置占空比
    mcpwm_set_duty(usr_id0, 2000); // 20%
    mcpwm_set_duty(usr_id1, 4000); // 40%
    mcpwm_set_duty(usr_id2, 8000); // 80%
#else
    // pwm cnt计数配置占空比
    u32 id0_frequency = 2000; // 2K
    u32 id1_frequency = 4000; // 4K
    u32 id2_frequency = 8000; // 8K
    u32 id0_loop_cntmax = clk_get("mcpwm") / id0_frequency;
    u32 id1_loop_cntmax = clk_get("mcpwm") / id1_frequency;
    u32 id2_loop_cntmax = clk_get("mcpwm") / id2_frequency;
    mcpwm_set_duty_by_cnt(usr_id0, id0_loop_cntmax / 10 * 2); // 20%
    mcpwm_set_duty_by_cnt(usr_id1, id1_loop_cntmax / 10 * 4); // 40%
    mcpwm_set_duty_by_cnt(usr_id2, id2_loop_cntmax / 10 * 8); // 80%
#endif

    // 溢出/借位中断测试
    request_irq(IRQ_MCPWM_TMR_IDX, 0, mcpwm_timeout_callback, 0);
}

void mcpwm_test_demo_stop(void)
{
    mcpwm_deinit(usr_id0);
    mcpwm_deinit(usr_id1);
    mcpwm_deinit(usr_id2);

    unrequest_irq(IRQ_MCPWM_TMR_IDX);
}
