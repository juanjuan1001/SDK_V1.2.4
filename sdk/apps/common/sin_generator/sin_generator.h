#ifndef _SIN_GENERATOR_H_
#define _SIN_GENERATOR_H_

#include "typedef.h"
#include "asm/hadc.h"

enum sin_source {
    SIN_GENERATOR_5K,
    SIN_GENERATOR_20K,
    SIN_GENERATOR_25K,
    SIN_GENERATOR_30K,
    SIN_GENERATOR_40K,
    SIN_GENERATOR_50K,
    SIN_GENERATOR_75K,
    SIN_GENERATOR_100K,
    SIN_GENERATOR_150K,
};

/*
 * @brief 关闭正弦信号发生器，关闭DAC
 */
void sin_generator_close(void);

/*
 * @brief 启动正弦信号发生器，启动DAC
 * @param enum sin_source sin: 正弦激励信号源频率
 * 		  enum hadc_pa_gain_sel gain: PA增益
 */
void sin_generator_start(enum sin_source sin, enum hadc_pa_gain_sel gain);

#endif
