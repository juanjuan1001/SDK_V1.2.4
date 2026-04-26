#ifndef _EXTI_H_
#define _EXTI_H_

#include "typedef.h"


enum inside_signal_sel {
    TMR2_PWM = 8,
    TMR3_PWM,
    USB_DP_IN,
    USB_DM_IN,
    HADC_OPA_CMP,
    DAC_PA_CMP,
};
/**
 * 注意：JL_WAKEUP 区别于PMU管理的唤醒。可理解为一个独立的模块使用。但在低功耗的情况下，中断无效。
 */

//cd09
/* IO唤醒源: 8个唤醒源:
 *    事件0~7: GP_ICH0~GP_ICH7 (任意io,两个ich映射到同一io组成双边沿)
 * */
/*
 * @brief 使能IO口[唤醒/外部中断]
 * @parm port 端口 such as:IO_PORTA_00
 * @parm trigger_edge 检测边缘，1 下降沿，0 上升沿
 * @parm cbfun 中断回调函数
 * @parm irq_priority 中断优先级
 * @return 0 成功，< 0 失败
 */
//无 int exti_io_init(u32 port, u8 trigger_edge, void (*cbfun)(u32, u32, u32), u8 irq_priority);
/*
 * @brief 失能IO口[唤醒/外部中断]
 * @parm port 端口 such as:IO_PORTA_00
 * @return 0 成功，< 0 失败
 */
//无 iint exti_io_close(u32 port);
/*
 * @brief 使能ICH0~7口[唤醒/外部中断]
 * @parm port 端口 such as:IO_PORTA_00
 * @parm trigger_edge 检测边缘，1 下降沿，0 上升沿
 * @parm cbfun 中断回调函数
 * @parm irq_priority 中断优先级
 * @return 0 成功，< 0 失败
 */
int exti_ich_init(u32 port, u8 trigger_edge, void (*cbfun)(u32, u32, u32), u8 irq_priority); //注意功能复用
/**
 * @brief 关掉该引脚的中断功能
 * @param port 引脚号：IO_PORTA_00......
 * @return 0 成功，< 0 失败
 */
int exti_ich_close(u32 port);
/*
 * @brief 使能ICH0~7口[唤醒/外部中断]
 * @parm port 端口 such as:IO_PORTA_00
 * @parm trigger_edge 检测边缘，1 下降沿，0 上升沿
 * @parm cbfun 中断回调函数
 * @parm irq_priority 中断优先级
 * @return 0 成功，< 0 失败
 */
// int exti_init(u32 port, u8 trigger_edge, void (*cbfun)(u32, u32, u32), u8 irq_priority);
/**
 * @brief 关掉该引脚的中断功能
 * @param port 引脚号：IO_PORTA_00......
 * @return 0 成功，< 0 失败
 */
// int exti_uninit(u32 port);
void exti_change_callback(u32 port, void (*cbfun)(u32, u32, u32));
void exti_change_en_state(u32 port, u8 exti_en);//exti_en:1:en,0:disable
void exti_change_edge_state(u32 port, u8 edge);
u8 exti_get_edge_state(u32 port);

int inside_signal_ich_wkup_interrupt_init(enum inside_signal_sel sel, u8 edge, void (*cbfun)(u32, u32, u32), u8 irq_priority); //注意功能复用
int inside_signal_ich_wkup_interrupt_close(enum inside_signal_sel sel);
void inside_signal_irq_change_callback(enum inside_signal_sel sel, void (*cbfun)(u32, u32, u32));
void inside_signal_irq_change_en_state(enum inside_signal_sel sel, u8 wkup_en);//wkup_en:1:en,0:disable
void inside_signal_irq_change_edge_state(enum inside_signal_sel sel, u8 edge);
u8 inside_signal_irq_get_edge_state(enum inside_signal_sel sel);

#endif

