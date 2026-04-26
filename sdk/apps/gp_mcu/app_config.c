/*********************************************************************************************
    *   Filename        : log_config.c
    *   Description     : Optimized Code & RAM (编译优化Log配置)
    *   Author          : Bingquan
    *   Email           : caibingquan@zh-jieli.com
    *   Last modifiled  : 2019-03-18 14:45
    *   Copyright:(c)JIELI  2011-2019  @ , All Rights Reserved.
*********************************************************************************************/
#include "system/includes.h"
#include "app_config.h"

#ifdef CONFIG_DEBUG_ENABLE
#define LOG_DEBUG_EN            1
#else
#define LOG_DEBUG_EN            0
#endif
#define CONFIG_DEBUG_EN(x)      (x & LOG_DEBUG_EN)

//异常中断，asser打印开启
#ifdef CONFIG_RELEASE_ENABLE
const int config_asser          = 0;
#else
const int config_asser          = 1;
#endif

/*
 sfc频率配置:96M or 80M or 64M
 IOVDD>1.65V SFC配置不超过64Mhz
 IOVDD>2.3V SFC配置不超过80Mhz
 IOVDD>2.7V SFC配置不超过96Mhz
 */
const u32 const_config_sfc_freq = 64000000;

/*
 存在校准记录后,上电晶振启动等待时间配置
  0: 上电时不再确认晶振起振与重新校准
 >0: 上电时等待时间内(ms)晶振不起振则重新校准
 */
const u32 config_xosc_wait_time = 1000;

const int vm_max_page_align_size_config   = TCFG_VM_SIZE * 1024L; //page对齐vm管理空间最大值配置
const int vm_max_sector_align_size_config = TCFG_VM_SIZE * 1024L; //sector对齐vm管理空间最大值配置

//gptimer
const u8 lib_gptimer_src_lsb_clk = 0; //时钟源选择lsb_clk, 单位:MHz,  1:选则lsb_clk, 0:选择std_clk
const u8 lib_gptimer_src_std_clk = 12; //时钟源选择std_x_clk, 单位:MHz  可选时钟有12MHz,24MHz,48MHz
const u8 lib_gptimer_timer_mode_en = 1; //gptimer timer功能使能
const u8 lib_gptimer_pwm_mode_en = 1; //gptimer pwm功能使能
const u8 lib_gptimer_capture_mode_en = 1; //gptimer capture功能使能
const u8 lib_gptimer_auto_tid_en = 1; //gptimer_tid 内部自动分配使能

//printf 是否支持 %f，打印浮点数
const int printf_support_float = 1;

const char log_tag_const_v_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_w_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_e_OFF AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);

const char log_tag_const_v_ON AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_d_ON AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_c_ON AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_i_ON AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_ON AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_ON AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_SETUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_d_SETUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_c_SETUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_i_SETUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_SETUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_SETUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_d_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_c_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_i_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_APP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_LCD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_LCD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_LCD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_LCD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_LCD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_LCD AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_EVENT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_EVENT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_EVENT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_EVENT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_w_EVENT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_EVENT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_c_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_w_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_CLOCK AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_w_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_LRC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_w_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_LP_TIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);


const char log_tag_const_v_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_PMU AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_ICACHE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_ICACHE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_ICACHE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_ICACHE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_ICACHE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_ICACHE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_DEBUG AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_KEY AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_KEY AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_KEY AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_KEY AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_w_KEY AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_KEY AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_VM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_w_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_GPIO AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_w_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_EXTI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_w_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_IIC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_w_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_UART AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_SPI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_USB AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_WKUP AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_HADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_HADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_HADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_HADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_HADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_HADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_TMR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_TMR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_TMR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_TMR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_w_TMR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_TMR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_MM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_MM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_MM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_MM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_MM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_MM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_SDFILE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_SDFILE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_SDFILE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_SDFILE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_SDFILE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_SDFILE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_RDEC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_RDEC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_RDEC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_RDEC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_RDEC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_RDEC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_GPTIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_GPTIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_GPTIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_GPTIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_GPTIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_GPTIMER AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_MCPWM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_MCPWM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_MCPWM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_MCPWM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_MCPWM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_MCPWM AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_IRFLT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_IRFLT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_IRFLT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_IRFLT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_IRFLT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_IRFLT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_RTC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_RTC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_RTC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_RTC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_RTC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_RTC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_PERI AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_DECODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_DECODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_DECODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_DECODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_DECODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_DECODE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_BAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_BAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_BAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_BAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_BAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_BAT AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_AT_CHAR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_AT_CHAR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_AT_CHAR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_AT_CHAR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_AT_CHAR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_AT_CHAR AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_ADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_ADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_ADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_ADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_ADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_ADC AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

const char log_tag_const_v_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_d_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_c_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(FALSE);
const char log_tag_const_i_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_w_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);
const char log_tag_const_e_UPDATE AT(.LOG_TAG_CONST) = CONFIG_DEBUG_EN(TRUE);

