#ifndef  __GPADC_API_H__
#define  __GPADC_API_H__


#include "asm/gpadc.h"

/*
 * @brief SARADC INIT
 */
void adc_init(void);

/*
 * @brief SARADC UNINIT
 */
void adc_uninit(void);

/*
 * @brief SARADC CLK INIT
 */
void adc_clk_init(int clk);

/*
 * @brief 获取对应通道的ADC值
 * @ch ADC通道
 * @return 返回通道ADC值
 * @note 需确保通道有存在于CPU或DMA的采集队列,否则需先通过adc_add_sample_ch/adc_dma_add_sample_ch注册通道
 */
s16 adc_get_value(enum AD_CH ch);

/*
 * @brief 获取对应通道的电压
 * @ch ADC通道
 * @return 返回通道的电压/mV
 * @note 需确保通道有存在于CPU或DMA的采集队列,否则需先通过adc_add_sample_ch/adc_dma_add_sample_ch注册通道
 */
s32 adc_get_voltage(enum AD_CH ch);

/*
 * @brief 将ADC数据转换为实际电压
 * @adc_vbg MVBG通道ADC值
 * @adc_raw_value 转换通道的ADC值
 * @return 返回转换后的电压/mV
 */
s32 adc_value_to_voltage(u16 adc_vbg, s16 adc_raw_value);

/*
 * @brief 添加ch到CPU模式ADC采集队列
 * @ch ADC通道（ANA通道，IO通道和DIFF通道）
 */
void adc_add_sample_ch(enum AD_CH ch);

/*
 * @brief 删除CPU模式ADC采集队列里的ch
 * @ch ADC通道（ANA通道，IO通道和DIFF通道）
 */
void adc_remove_sample_ch(enum AD_CH ch);

/*
 * @brief 添加ch到DMA模式ADC采集队列
 * @ch ADC通道（仅支持IO通道和DIFF通道）
 */
void adc_dma_add_sample_ch(enum AD_CH ch);

/*
 * @brief 删除DMA模式ADC采集队列里的ch
 * @ch ADC通道（仅支持IO通道和DIFF通道）
 */
void adc_dma_remove_sample_ch(enum AD_CH ch);

/*
 * @brief ADC进入DMA单通道连续采集模式（该模式下CPU模式采样无法进行）
 * @ch ADC通道（仅支持IO通道和DIFF通道）gpadc_dma_irq_callback: DMA单通道中断回调函数
 * @note 注意需要与adc_dma_exit_single_ch_sample()成对使用
 * @note 回调函数16bits的buf，有效数据为bit16~4，低4位无效，读取时需>>4
 */
void adc_dma_enter_single_ch_sample(enum AD_CH ch, void (*gpadc_dma_irq_callback)(u16 *buf, u32 len));

/*
 * @brief ADC退出DMA单通道连续采集模式（恢复DMA多通道模式采集和CPU模式采集）
 * @note 注意需要与adc_dma_enter_single_ch_sample()成对使用
 */
void adc_dma_exit_single_ch_sample(enum AD_CH ch);

/*
 * @brief 阻塞CPU模式采集，打断ADC转换队列，立即获取对应通道的ADC值
 * @ch ADC通道
 * @return 返回通道ADC值
 * @note 不支持中断调用
 */
s16 adc_get_value_by_blocking(enum AD_CH ch, u8 times);

/*
 * @brief 阻塞CPU模式采集，打断ADC转换队列，立即获取对应通道的电压值，内部做了滤波取平均处理
 * @ch ADC通道
 * @return 返回通道ADC电压
 * @note 不支持中断调用
 */
s16 adc_get_voltage_by_blocking(enum AD_CH ch, u8 times);


#endif  /*GPADC_API_H*/
