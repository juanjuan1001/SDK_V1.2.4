#ifndef  __GPADC_H__
#define  __GPADC_H__

#include "generic/typedef.h"
#include "asm/includes.h"

#define DMA_CONTI_BUF_LEN	(1200) //DMA单通道模式采集x个点起一次中断

//ADC采样数据位宽
#define ADC_SEL_6_BIT   (0x0)
#define ADC_SEL_8_BIT   (0x1)
#define ADC_SEL_10_BIT  (0x2)
#define ADC_SEL_12_BIT  (0x3)
#define ADC_BIT_SEL		ADC_SEL_12_BIT //adc工作采样宽度

#if (ADC_BIT_SEL == ADC_SEL_12_BIT)
#define AD_MAX_VALUE	(0xfffL)
#elif (ADC_BIT_SEL == ADC_SEL_10_BIT)
#define AD_MAX_VALUE	(0x3ffL)
#elif (ADC_BIT_SEL == ADC_SEL_8_BIT)
#define AD_MAX_VALUE	(0xffL)
#elif (ADC_BIT_SEL == ADC_SEL_6_BIT)
#define AD_MAX_VALUE	(0x3fL)
#endif

#define	ADC_VBG_CENTER  (800) //VBG基准值

#define ADC_CH_MASK_TYPE_SEL	0xffff0000
#define ADC_CH_MASK_CH_SEL	    0x0000ffff

#define ADC_IO_CH_PA0     (0x0) //ADC_IO_0
#define ADC_IO_CH_PA1     (0x8) //ADC_IO_8
#define ADC_IO_CH_PA2     (0x4) //ADC_IO_4
#define ADC_IO_CH_PA3     (0xc) //ADC_IO_12
#define ADC_IO_CH_PA4     (0x2) //ADC_IO_2
#define ADC_IO_CH_PA5     (0xa) //ADC_IO_10
#define ADC_IO_CH_PA6     (0x6) //ADC_IO_6
#define ADC_IO_CH_PA7     (0xe) //ADC_IO_14
#define ADC_IO_CH_PA8     (0x1) //ADC_IO_1
#define ADC_IO_CH_PA9     (0x9) //ADC_IO_9
#define ADC_IO_CH_PA10    (0x5) //ADC_IO_5
#define ADC_IO_CH_PA11    (0xd) //ADC_IO_13
#define ADC_IO_CH_PA12    (0x3) //ADC_IO_3
#define ADC_IO_CH_DP      (0xb) //ADC_IO_11
#define ADC_IO_CH_DM      (0x7) //ADC_IO_7

#define ADC_PMU_CH_VBG          (0b0000)
#define ADC_PMU_CH_VP17         (0b0001)
#define ADC_PMU_CH_VLCD_DIV_4   (0b0010)
#define ADC_PMU_CH_AVDDR_DIV_2  (0b0011)
#define ADC_PMU_CH_VTEMP        (0b0100)
#define ADC_PMU_CH_VPWR_DIV_4   (0b0101)
#define ADC_PMU_CH_AVDDCP_DIV_4 (0b0110)
#define ADC_PMU_CH_AVDDCP_DIV_2 (0b0111)
#define ADC_PMU_CH_AVDDCP_VB17  (0b1000)
#define ADC_PMU_CH_DCVDD        (0b1010)
#define ADC_PMU_CH_DVDD         (0b1011)
#define ADC_PMU_CH_WVDD         (0b1101)

#define ADC_DIFF_CH_SP_PA3_SN_PA2	(0x01) //ADC_DIFF_1
#define ADC_DIFF_CH_SP_PA4_SN_PA2	(0x02) //ADC_DIFF_2
#define ADC_DIFF_CH_SP_PA5_SN_PA2	(0x03) //ADC_DIFF_3
#define ADC_DIFF_CH_SP_PA2_SN_PA3	(0x10) //ADC_DIFF_4
#define ADC_DIFF_CH_SP_PA4_SN_PA3	(0x12) //ADC_DIFF_6
#define ADC_DIFF_CH_SP_PA5_SN_PA3	(0x13) //ADC_DIFF_7
#define ADC_DIFF_CH_SP_PA2_SN_PA4	(0x20) //ADC_DIFF_8
#define ADC_DIFF_CH_SP_PA3_SN_PA4	(0x21) //ADC_DIFF_9
#define ADC_DIFF_CH_SP_PA5_SN_PA4	(0x23) //ADC_DIFF_11
#define ADC_DIFF_CH_SP_PA2_SN_PA5	(0x30) //ADC_DIFF_12
#define ADC_DIFF_CH_SP_PA3_SN_PA5	(0x31) //ADC_DIFF_13
#define ADC_DIFF_CH_SP_PA4_SN_PA5	(0x32) //ADC_DIFF_14
#define ADC_DIFF_CH_VB				(0x44)

#define AD_HADC_TEST     (0x0<<16)
#define AD_AUDIO_TEST    (0x1<<16)
#define AD_PMU_TEST      (0x2<<16)
#define AD_X32K_TEST     (0x3<<16)
#define AD_PLL_TEST      (0x5<<16)
#define AD_IO_TEST       (0xe<<16)
#define AD_DIFF_TEST     (0xf<<16)

/*
 * 差分通道采集回来的电压V_DIFF=(VIP-VIN)/2+VCM
 * VIP为P端电压 VIN为N端电压 VCM为IOVDD/2
 */

enum AD_CH {
    //ANA通道（模拟通道只支持CPU采集，不支持DMA采集）
    AD_ANA_PMU_CH_VBG = (AD_PMU_TEST | ADC_PMU_CH_VBG),
    AD_ANA_PMU_CH_VP17,
    AD_ANA_PMU_CH_VLCD_DIV_4,
    AD_ANA_PMU_CH_AVDDR_DIV_4,
    AD_ANA_PMU_CH_VTEMP,
    AD_ANA_PMU_CH_VPWR_DIV_4,
    AD_ANA_PMU_CH_AVDDCP_DIV_4,
    AD_ANA_PMU_CH_AVDDCP_DIV_2,
    AD_ANA_PMU_CH_AVDDCP_VB17,
    AD_ANA_PMU_CH_DCVDD = (AD_PMU_TEST | ADC_PMU_CH_DCVDD),
    AD_ANA_PMU_CH_DVDD,
    AD_ANA_PMU_CH_WVDD = (AD_PMU_TEST | ADC_PMU_CH_WVDD),

    //IO通道（支持CPU采集和DMA采集）
    AD_CH_PA0 = (AD_IO_TEST | ADC_IO_CH_PA0),
    AD_CH_PA8,
    AD_CH_PA4,
    AD_CH_PA12,
    AD_CH_PA2,
    AD_CH_PA10,
    AD_CH_PA6,
    AD_CH_DM,
    AD_CH_PA1,
    AD_CH_PA9,
    AD_CH_PA5,
    AD_CH_DP,
    AD_CH_PA3,
    AD_CH_PA11,
    AD_CH_PA7,

    //DIFF通道（支持CPU采集和DMA采集）
    AD_DIFF_CH_SP_PA3_SN_PA2 = (AD_DIFF_TEST | ADC_DIFF_CH_SP_PA3_SN_PA2),
    AD_DIFF_CH_SP_PA4_SN_PA2,
    AD_DIFF_CH_SP_PA5_SN_PA2,
    AD_DIFF_CH_SP_PA2_SN_PA3 = (AD_DIFF_TEST | ADC_DIFF_CH_SP_PA2_SN_PA3),
    AD_DIFF_CH_SP_PA4_SN_PA3 = (AD_DIFF_TEST | ADC_DIFF_CH_SP_PA4_SN_PA3),
    AD_DIFF_CH_SP_PA5_SN_PA3,
    AD_DIFF_CH_SP_PA2_SN_PA4 = (AD_DIFF_TEST | ADC_DIFF_CH_SP_PA2_SN_PA4),
    AD_DIFF_CH_SP_PA3_SN_PA4,
    AD_DIFF_CH_SP_PA5_SN_PA4 = (AD_DIFF_TEST | ADC_DIFF_CH_SP_PA5_SN_PA4),
    AD_DIFF_CH_SP_PA2_SN_PA5 = (AD_DIFF_TEST | ADC_DIFF_CH_SP_PA2_SN_PA5),
    AD_DIFF_CH_SP_PA3_SN_PA5,
    AD_DIFF_CH_SP_PA4_SN_PA5,
    AD_DIFF_CH_VB = (AD_DIFF_TEST | ADC_DIFF_CH_VB),
};

#define ADC_CHANNEL_SEL(ch)         P33_CON_SET(P3_PMU_ADC0, 0, 4, ch)

#define VBG_BUFFER_EN(en)			p33_fast_access(P3_PMU_ADC0, BIT(4), en)
#define VBG_TEST_EN(en)				p33_fast_access(P3_PMU_ADC0, BIT(5), en)
#define VBG_BUFFER_AND_TEST_EN(en)  p33_fast_access(P3_PMU_ADC0, BIT(4)|BIT(5), en)

#define PMU_TOADC_EN(en)			p33_fast_access(P3_PMU_ADC1, BIT(1), en)
#define PMU_DET_OE(en)				p33_fast_access(P3_PMU_ADC1, BIT(0), en)
#define PMU_TOADC_AND_DET_EN(en)    p33_fast_access(P3_PMU_ADC1, BIT(0)|BIT(1), en)

enum {
    VBG_TEST_SEL_WBG04,
    VBG_TEST_SEL_MBG08,
    VBG_TEST_SEL_LVDBG,
    VBG_TEST_SEL_MVBG,
};
#define VBG_TEST_SEL(sel)			P33_CON_SET(P3_PMU_ADC0, 6, 2, sel)


#endif  /*GPADC_H*/
