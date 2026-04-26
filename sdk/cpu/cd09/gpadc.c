/**@file  		gpadc.c
* @brief        SARADC驱动及应用接口
* @details		ADC DRIVER / ADC API
* @author		JL
* @date     	2023-05-26
* @version  	V1.0
* @copyright    Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
 */

#include "typedef.h"
#include "asm/clock.h"
#include "gpadc_api.h"
#include "timer.h"
#include "asm/power_interface.h"
#include "jiffies.h"
#include "app_config.h"
#include "gpio.h"
#include "asm/efuse.h"
#include "asm/includes.h"
#include "malloc.h"

#define LOG_TAG_CONST   ADC
#define LOG_TAG		    "[SARADC]"
#include "debug.h"

#define ADC_MAX_CH  				(10)  //cpu模式采集队列支持的最大通道数
#define DMA_BUF_CH_UNIT				(2)   //单个dma_buf内存放单个通道数据组数
#define GET_DMA_CUR_USE_BUF_FLAG() 	((JL_ADC->DMA_CON0&BIT(13))>>13) //0:BUF0 1:BUF1
#define ADC_CLK						(300000L) //非DMA单通道采集时使用的时钟 250K~12M
#define ADC_DMA_CONTI_CLK			(12000000L) //DMA单通道采集时使用的时钟 250K~12M
#define SET_SENSE_GAIN(x)           SFR(JL_ADC->ANA_CON, 11, 2, x) //差分放大器增益 0:X1 1:X2 2:X4 3:X8

//cpu模式adc采集队列信息结构体
struct adc_info_t {
    u32 ch;
    s16 adc_value;
    s16 voltage;
};
struct adc_info_t adc_queue[ADC_MAX_CH];

//dma模式adc采集队列信息结构体
struct adc_dma_info_t {
    u32 ch;
    s16 adc_value;
    s16 voltage;
};
struct adc_dma_info_t adc_dma_queue[16];//dma_chl[0~15]

struct gpadc_info_t {
    u8 blocking;		  //adc blocking flag
    u8 init_ok;			  //adc init ok flag
    u8 cur_ch;			  //cpu_mode adc采集队列当前编号
    u8 dma_conti_mode;	  //dma 单通道采集模式标志
    u16 adc_vbg;		  //vbg ad_value
    u16 scan_timer;		  //扫描定时器
    u16 dma_ch_en;		  //dma_mode 16个通道的使能标志
    u16 dma_ch_num;		  //dma_mode 当前采集通道总数
    u16 *dma_badr_buf;	  //dma_mode dma_buf
    u16 *dma_conti_buf;   //dma_conti_mode dma_buf
    void (*dma_irq_callback)(u16 *buf, u32 len);
};
static struct gpadc_info_t gpadc_info;
#define __this (&gpadc_info)

const u32 dma_ch_table[] = {
    AD_CH_PA0,  0xffff, 				  0xffff, 			 		AD_CH_PA1, 	   //chl0
    AD_CH_PA8,  AD_DIFF_CH_SP_PA3_SN_PA2, 0xffff, 			 		AD_CH_PA9, 	   //chl1
    AD_CH_PA4,  AD_DIFF_CH_SP_PA4_SN_PA2, 0xffff, 			 		AD_CH_PA5, 	   //chl2
    AD_CH_PA12, AD_DIFF_CH_SP_PA5_SN_PA2, 0xffff, 			 		AD_CH_DP,  	   //chl3
    AD_CH_PA2,  AD_DIFF_CH_SP_PA2_SN_PA3, 0xffff, 					AD_CH_PA3, 	   //chl4
    AD_CH_PA10, 0xffff, 				  0xffff, 			 		AD_CH_PA11,	   //chl5
    AD_CH_PA6,  AD_DIFF_CH_SP_PA4_SN_PA3, 0xffff, 					AD_CH_PA7, 	   //chl6
    AD_CH_DM, 	AD_DIFF_CH_SP_PA5_SN_PA3, 0xffff, 					0xffff,		   //chl7
    AD_CH_PA1,  AD_DIFF_CH_SP_PA2_SN_PA4, 0xffff, 					AD_DIFF_CH_VB, //chl8
    AD_CH_PA9,  AD_DIFF_CH_SP_PA3_SN_PA4, AD_DIFF_CH_SP_PA3_SN_PA2, AD_DIFF_CH_VB, //chl9
    AD_CH_PA5,  0xffff, 				  AD_DIFF_CH_SP_PA4_SN_PA2, AD_DIFF_CH_VB, //chl10
    AD_CH_DP, 	AD_DIFF_CH_SP_PA5_SN_PA4, AD_DIFF_CH_SP_PA5_SN_PA2, AD_DIFF_CH_VB, //chl11
    AD_CH_PA3,  AD_DIFF_CH_SP_PA2_SN_PA5, AD_DIFF_CH_SP_PA2_SN_PA3, AD_DIFF_CH_VB, //chl12
    AD_CH_PA11, AD_DIFF_CH_SP_PA3_SN_PA5, 0xffff, 					AD_DIFF_CH_VB, //chl13
    AD_CH_PA7,  AD_DIFF_CH_SP_PA4_SN_PA5, AD_DIFF_CH_SP_PA4_SN_PA3, AD_DIFF_CH_VB, //chl14
    0xffff, 	 0xffff, 				  AD_DIFF_CH_SP_PA5_SN_PA3, AD_DIFF_CH_VB, //chl15
};

/*
 * @brief 把IO通道和DIFF通道转换成DMA通道序号
 * @ch ADC通道（IO通道和DIFF通道）
 * @return 返回DMA通道序号
 */
u8 cpu_2_dma_ch(enum AD_CH ch)
{
    for (u8 i = 0; i < ARRAY_SIZE(dma_ch_table); i++) {
        if (dma_ch_table[i] == ch) {
            if (!(__this->dma_ch_en & BIT(i / 4))) {
                return i;
            }
        }
    }
    return ARRAY_SIZE(dma_ch_table);
}

/*
 * @brief 关闭DMA模式
 */
void adc_dma_close(void)
{
    __this->dma_ch_en = 0;
    JL_ADC->DMA_CON0 = 0;
    JL_ADC->DMA_CON1 = 0;
    JL_ADC->DMA_CON2 = 0;
    JL_ADC->DMA_BADR = 0;
}

/*
 * @brief 获取DMA模式下采集通道总数
 * @return 返回DMA采集总通道
 */
u8 get_adc_dma_sample_ch_num(void)
{
    u8 ch_num = 0;
    for (u8 i = 0; i < 16; i++) {
        if (__this->dma_ch_en & BIT(i)) {
            ch_num++;
        }
    }
    return ch_num;
}

/*
 * @brief DMA模式配置，在增减DMA采样通道之后都需重新配置
 */
void adc_dma_config(void)
{
    u16 sp_rate;

    local_irq_disable();
    if (JL_ADC->DMA_CON1 & 0xffff) {
        SFR(JL_ADC->DMA_CON0, 0, 1, 0); //dma ie
        while (!(JL_ADC->DMA_CON0 & BIT(15))) {}
    }
    SFR(JL_ADC->DMA_CON1, 0, 16, 0); //chl en
    local_irq_enable();

    __this->dma_ch_num = get_adc_dma_sample_ch_num();

    if (!__this->dma_ch_num) {
        return;
    }

    //dma_buf init
    if (__this->dma_badr_buf) {
        free(__this->dma_badr_buf);
        __this->dma_badr_buf = NULL;
    }
    __this->dma_badr_buf = (u16 *)malloc(__this->dma_ch_num * DMA_BUF_CH_UNIT * 2 * 2);
    if (__this->dma_badr_buf) {
        SFR(JL_ADC->DMA_CON0, 16, 16, __this->dma_ch_num * DMA_BUF_CH_UNIT);
        JL_ADC->DMA_BADR = (u32)__this->dma_badr_buf;
    } else {
        log_error("dma malloc err!!!\n");
        adc_dma_close();
        return;
    }

    //sp_rate config
    sp_rate = (__this->dma_ch_num + 1) * (10 + ADC_BIT_SEL * 2) + (19 - __this->dma_ch_num);
    SFR(JL_ADC->DMA_CON1, 16, 16, sp_rate * 2);

    SFR(JL_ADC->DMA_CON0, 2, 1, 0);  //chl log en
    SFR(JL_ADC->DMA_CON0, 26, 1, 0); //dma priority
    SFR(JL_ADC->DMA_CON0, 12, 1, 1); //buf clr
    SFR(JL_ADC->DMA_CON0, 14, 1, 1); //clr pend
    SFR(JL_ADC->DMA_CON0, 0, 1, 1); //dma ie
    //初始化dma采集,关掉数字EN,dma_adr恢复到dile状态,内部状态机回到IDLE.这样状态机和dma_adr就对得上
    SFR(JL_ADC->ADC_CON, 31, 1, 0);
    SFR(JL_ADC->ADC_CON, 31, 1, 1);
    SFR(JL_ADC->DMA_CON1, 0, 16, __this->dma_ch_en); //kst
}

/*
 * @brief 获取对应通道的ADC值
 * @ch ADC通道
 * @return 返回通道ADC值
 */
s16 adc_get_value(enum AD_CH ch)
{
    for (u8 i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            return adc_queue[i].adc_value;
        }
    }
    for (u8 i = 0; i < 16; i++) {
        if (adc_dma_queue[i].ch == ch) {
            return adc_dma_queue[i].adc_value;
        }
    }
    return 0;
}

/*
 * @brief 获取对应通道的电压
 * @ch ADC通道
 * @return 返回通道的电压/mV
 */
s32 adc_get_voltage(enum AD_CH ch)
{
    for (u8 i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            return adc_queue[i].voltage;
        }
    }
    for (u8 i = 0; i < 16; i++) {
        if (adc_dma_queue[i].ch == ch) {
            return adc_dma_queue[i].voltage;
        }
    }
    return 0;
}

/*
 * @brief 将ADC数据转换为实际电压
 * @adc_vbg MVBG通道ADC值
 * @adc_raw_value 转换通道的ADC值
 * @return 返回转换后的电压/mV
 */
s32 adc_value_to_voltage(u16 adc_vbg, s16 adc_raw_value)
{
    s32 vbg_volt = ADC_VBG_CENTER;
    s32 ch_volt;
    if (adc_vbg == 0) {
        adc_vbg = 1; //防止div0异常
    }
    ch_volt = adc_raw_value * vbg_volt / adc_vbg;
    return ch_volt;
}

/*
 * @brief CPU模式下，获取ADC采样数据
 * @return 返回ADC采样数据
 */
s16 adc_get_res(void)
{
    s16 adc_res = JL_ADC->RES;
    switch (ADC_BIT_SEL) {
    case ADC_SEL_6_BIT:
        adc_res >>= 6;
        break;
    case ADC_SEL_8_BIT:
        adc_res >>= 4;
        break;
    case ADC_SEL_10_BIT:
        adc_res >>= 2;
        break;
    case ADC_SEL_12_BIT:
        break;
    }
    return adc_res;
}

/*
 * @brief CPU模式下，获取采集队列中下一个通道的队列编号
 * @return 返回下一个采集通道编号
 */
u32 adc_get_next_ch(void)
{
    u8 i;
    if (__this->cur_ch == ADC_MAX_CH) {
        i = 0;
    } else {
        i = __this->cur_ch + 1;
    }
    for (; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch) {
            return i;
        }
    }
    return ADC_MAX_CH;
}

/*
 * @brief CPU模式下，ADC切换采集通道
 * @ch ADC通道（ANA通道，IO通道和DIFF通道）
 */
void adc_sample(enum AD_CH ch)
{
    u16 adc_ch_sel = ch & ADC_CH_MASK_CH_SEL;
    switch (ch & ADC_CH_MASK_TYPE_SEL) {
    case AD_HADC_TEST:
        SFR(JL_ADC->ADC_CON, 3, 3, 0b001);
        SFR(JL_ADC->ADC_CON, 10, 3, AD_HADC_TEST >> 16);
        break;
    case AD_AUDIO_TEST:
        SFR(JL_ADC->ADC_CON, 3, 3, 0b001);
        SFR(JL_ADC->ADC_CON, 10, 3, AD_AUDIO_TEST >> 16);
        break;
    case AD_PMU_TEST:
        SFR(JL_ADC->ADC_CON, 3, 3, 0b001);
        SFR(JL_ADC->ADC_CON, 10, 3, AD_PMU_TEST >> 16);
        ADC_CHANNEL_SEL(adc_ch_sel);
        break;
    case AD_X32K_TEST:
        SFR(JL_ADC->ADC_CON, 3, 3, 0b001);
        SFR(JL_ADC->ADC_CON, 10, 3, AD_X32K_TEST >> 16);
        break;
    case AD_PLL_TEST:
        SFR(JL_ADC->ADC_CON, 3, 3, 0b001);
        SFR(JL_ADC->ADC_CON, 10, 3, AD_PLL_TEST >> 16);
        break;
    case AD_IO_TEST:
        SFR(JL_ADC->ADC_CON, 3, 3, 0b010);
        SFR(JL_ADC->ADC_CON, 6, 4, adc_ch_sel);
        break;
    case AD_DIFF_TEST:
        SFR(JL_ADC->ADC_CON, 3, 3, 0b100);
        if (adc_ch_sel == ADC_DIFF_CH_VB) {
            SFR(JL_ADC->ANA_CON, 0, 2, 0); //sp sn disable
            SFR(JL_ADC->ANA_CON, 6, 2, 3); //vp vn enable
        } else {
            SFR(JL_ADC->ANA_CON, 6, 2, 0); //vp vn disable
            SFR(JL_ADC->ANA_CON, 0, 2, 3); //sp sn enable
            SFR(JL_ADC->ANA_CON, 4, 2, (ch & 0xf0) >> 4); //sn sel
            SFR(JL_ADC->ANA_CON, 2, 2, (ch & 0x0f)); //sp sel
        }
        break;
    }
}

/*
 * @brief SARADC 中断处理函数
 */
___interrupt
static void adc_isr(void)
{
    u16 buf_len = __this->dma_ch_num * DMA_BUF_CH_UNIT;
    u32 dma_adc_value = 0;
    u8 dma_ch = 0;

    //dma mode
    if (JL_ADC->DMA_CON0 & BIT(15)) {
        SFR(JL_ADC->DMA_CON0, 14, 1, 1); //clr pending
        SFR(JL_ADC->DMA_CON0, 0, 1, 0); //ie
        if (__this->dma_conti_mode && __this->dma_irq_callback) {
            __this->dma_irq_callback(__this->dma_conti_buf + (!GET_DMA_CUR_USE_BUF_FLAG() * DMA_CONTI_BUF_LEN), DMA_CONTI_BUF_LEN);
        } else {
            for (u8 i = 0; i < 16; i++) {
                if ((__this->dma_ch_en & BIT(i))) {
                    for (u8 j = 0; j < buf_len; j++) {
                        if (j % __this->dma_ch_num == dma_ch) {
                            dma_adc_value += (__this->dma_badr_buf[(!GET_DMA_CUR_USE_BUF_FLAG()) * buf_len + j] >> 4);
                        }
                    }
                    adc_dma_queue[i].adc_value = dma_adc_value / DMA_BUF_CH_UNIT;
                    if ((adc_dma_queue[i].ch & ADC_CH_MASK_TYPE_SEL) == AD_DIFF_TEST) {
                        adc_dma_queue[i].adc_value -= (AD_MAX_VALUE / 2);
                        adc_dma_queue[i].adc_value <<= 1;
                    }
                    adc_dma_queue[i].voltage = adc_value_to_voltage(__this->adc_vbg, adc_dma_queue[i].adc_value);
                    dma_adc_value = 0;
                    dma_ch++;
                }
            }
        }
        SFR(JL_ADC->DMA_CON0, 0, 1, 1); //ie
    }

    //cpu mode
    if (JL_ADC->ADC_CON & BIT(15)) {
        SFR(JL_ADC->ADC_CON, 13, 1, 0); //ie
        if ((!__this->dma_conti_mode) && (!__this->blocking)) {
            if (__this->cur_ch < ADC_MAX_CH) {
                adc_queue[__this->cur_ch].adc_value = adc_get_res();
                if ((adc_queue[__this->cur_ch].ch & ADC_CH_MASK_TYPE_SEL) == AD_DIFF_TEST) {
                    adc_queue[__this->cur_ch].adc_value -= (AD_MAX_VALUE / 2);
                    adc_queue[__this->cur_ch].adc_value <<= 1;
                }
                if (adc_queue[__this->cur_ch].ch == AD_ANA_PMU_CH_VBG) {
                    __this->adc_vbg = adc_queue[__this->cur_ch].adc_value; //update vbg value
                }
                adc_queue[__this->cur_ch].voltage = adc_value_to_voltage(__this->adc_vbg, adc_queue[__this->cur_ch].adc_value);
            }

            __this->cur_ch = adc_get_next_ch();
            if (__this->cur_ch < ADC_MAX_CH) {
                adc_sample(adc_queue[__this->cur_ch].ch);
                SFR(JL_ADC->ADC_CON, 14, 1, 1); //clr pending
                SFR(JL_ADC->ADC_CON, 13, 1, 1); //ie
            }
        }
    }
    asm("csync");
}

/*
 * @brief 添加ch到CPU模式ADC采集队列
 * @ch ADC通道（ANA通道，IO通道和DIFF通道）
 */
void adc_add_sample_ch(enum AD_CH ch)
{
    u8 i;
    for (i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            break;
        } else if (!adc_queue[i].ch) {
            switch (ch & ADC_CH_MASK_TYPE_SEL) {
            case AD_HADC_TEST:
            case AD_AUDIO_TEST:
            case AD_PMU_TEST:
            case AD_X32K_TEST:
            case AD_PLL_TEST:
            case AD_IO_TEST:
            case AD_DIFF_TEST:
                adc_queue[i].ch = ch;
                //en usb 2 saradc
                if (ch == AD_CH_DP) {
                    SFR(JL_PORTUSB->CON, 1, 1, 1);
                } else if (ch == AD_CH_DM) {
                    SFR(JL_PORTUSB->CON, 3, 1, 1);
                }
                break;
            default:
                log_error("adc add sample ch err!!!!\n");
                break;
            }
            break;
        }
    }
    if (i == (ADC_MAX_CH - 1) && adc_queue[i].ch != ch) {
        log_error("adc add err, queue full!!!!\n");
    }
}

/*
 * @brief 添加ch到DMA模式ADC采集队列
 * @ch ADC通道（IO通道和DIFF通道）
 */
void adc_dma_add_sample_ch(enum AD_CH ch)
{
    u8 dma_ch_sel = cpu_2_dma_ch(ch);

    if (dma_ch_sel == ARRAY_SIZE(dma_ch_table)) {
        log_error("dma adc add err\n");
        return;
    } else {
        __this->dma_ch_en |= BIT(dma_ch_sel / 4);
        SFR(JL_ADC->DMA_CON2, dma_ch_sel / 4 * 2, 2, dma_ch_sel % 4);
    }

    //en usb 2 saradc
    if (ch == AD_CH_DP) {
        SFR(JL_PORTUSB->CON, 1, 1, 1);
    } else if (ch == AD_CH_DM) {
        SFR(JL_PORTUSB->CON, 3, 1, 1);
    }

    adc_dma_queue[dma_ch_sel / 4].ch = ch;
    adc_dma_config();
}

/*
 * @brief 删除CPU模式ADC采集队列里的ch
 * @ch ADC通道（ANA通道，IO通道和DIFF通道）
 */
void adc_remove_sample_ch(enum AD_CH ch)
{
    for (u8 i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            adc_queue[i].ch = 0;
            adc_queue[i].voltage = 0;
            if (ch == AD_CH_DP) {
                SFR(JL_PORTUSB->CON, 1, 1, 0);
            } else if (ch == AD_CH_DM) {
                SFR(JL_PORTUSB->CON, 3, 1, 0);
            }
            break;
        }
    }
}

/*
 * @brief 删除DMA模式ADC采集队列里的ch
 * @ch ADC通道（IO通道和DIFF通道）
 */
void adc_dma_remove_sample_ch(enum AD_CH ch)
{
    for (u8 i = 0; i < 16; i++) {
        if (adc_dma_queue[i].ch == ch) {
            adc_dma_queue[i].ch = 0;
            adc_dma_queue[i].voltage = 0;
            if (ch == AD_CH_DP) {
                SFR(JL_PORTUSB->CON, 1, 1, 0);
            } else if (ch == AD_CH_DM) {
                SFR(JL_PORTUSB->CON, 3, 1, 0);
            }
            __this->dma_ch_en &= ~BIT(i);
            adc_dma_config();
            break;
        }
    }
}

/*
 * @brief ADC进入DMA单通道连续采集模式（该模式下CPU模式采样无法进行）
 * @ch ADC通道（仅支持IO通道和DIFF通道）gpadc_dma_irq_callback: DMA单通道中断回调函数
 * @note 注意需要与adc_dma_exit_single_ch_sample()成对使用
 */
void adc_dma_enter_single_ch_sample(enum AD_CH ch, void (*gpadc_dma_irq_callback)(u16 *buf, u32 len))
{
    u16 sp_rate;
    u8 dma_ch_sel = cpu_2_dma_ch(ch);

    if (__this->dma_conti_mode) {
        log_error("dma conti working ! ! !\n");
        return;
    }

    if (dma_ch_sel == ARRAY_SIZE(dma_ch_table)) {
        log_error("dma adc add err\n");
        return;
    } else {
        SFR(JL_ADC->DMA_CON0, 0, 1, 0); //dma ie
        SFR(JL_ADC->DMA_CON2, dma_ch_sel / 4 * 2, 2, dma_ch_sel % 4);
    }

    //en usb 2 saradc
    if (ch == AD_CH_DP) {
        SFR(JL_PORTUSB->CON, 1, 1, 1);
    } else if (ch == AD_CH_DM) {
        SFR(JL_PORTUSB->CON, 3, 1, 1);
    }

    __this->dma_conti_mode = 1;
    __this->dma_irq_callback = gpadc_dma_irq_callback;
    SFR(JL_ADC->DMA_CON1, 0, 16, 0);

    if (__this->dma_conti_buf) {
        free(__this->dma_conti_buf);
        __this->dma_conti_buf = NULL;
    }
    __this->dma_conti_buf = (u16 *)malloc(DMA_CONTI_BUF_LEN * 2 * 2);
    if (__this->dma_conti_buf) {
        SFR(JL_ADC->DMA_CON0, 16, 16, DMA_CONTI_BUF_LEN);
        JL_ADC->DMA_BADR = (u32)__this->dma_conti_buf;
    } else {
        log_error("dma malloc err!!!\n");
        adc_dma_close();
        return;
    }

    SFR(JL_ADC->ADC_CON, 13, 1, 0); //ie
    while (!(JL_ADC->ADC_CON & BIT(15))) {}
    if (__this->dma_ch_en) {
        SFR(JL_ADC->DMA_CON0, 0, 1, 0); //dma ie
        SFR(JL_ADC->DMA_CON1, 0, 16, 0); //chl en
        while (!(JL_ADC->DMA_CON0 & BIT(15))) {}
    }
    adc_clk_init(ADC_DMA_CONTI_CLK);
    sp_rate = 10 + ADC_BIT_SEL * 2;
    SFR(JL_ADC->DMA_CON1, 16, 16, sp_rate);

    SFR(JL_ADC->DMA_CON0, 1, 1, 1);
    SFR(JL_ADC->DMA_CON0, 12, 1, 1); //buf clr
    SFR(JL_ADC->DMA_CON0, 14, 1, 1); //clr pend
    SFR(JL_ADC->DMA_CON0, 0, 1, 1); //dma ie
    SFR(JL_ADC->DMA_CON1, 0, 16, BIT(dma_ch_sel / 4)); //kst
}

/*
 * @brief ADC退出DMA单通道连续采集模式（恢复DMA多通道模式采集和CPU模式采集）
 * @note 注意需要与adc_dma_enter_single_ch_sample()成对使用
 */
void adc_dma_exit_single_ch_sample(enum AD_CH ch)
{
    SFR(JL_ADC->DMA_CON0, 0, 1, 0); //dma ie
    SFR(JL_ADC->DMA_CON1, 0, 16, 0); //chl en
    while (!(JL_ADC->DMA_CON0 & BIT(15))) {}
    SFR(JL_ADC->DMA_CON0, 1, 1, 0);
    adc_clk_init(ADC_CLK);

    SET_SENSE_GAIN(0);
    SFR(JL_ADC->DMA_CON0, 0, 1, 0); //dma ie
    for (u8 i = 0; i < 16; i++) {
        if (adc_dma_queue[i].ch == ch) {
            adc_dma_queue[i].ch = 0;
            if (ch == AD_CH_DP) {
                SFR(JL_PORTUSB->CON, 1, 1, 0);
            } else if (ch == AD_CH_DM) {
                SFR(JL_PORTUSB->CON, 3, 1, 0);
            }
            break;
        }
    }

    if (__this->dma_conti_buf) {
        free(__this->dma_conti_buf);
        __this->dma_conti_buf = NULL;
    }
    __this->dma_irq_callback = NULL;

    adc_dma_config();

    __this->dma_conti_mode = 0;
    __this->cur_ch = ADC_MAX_CH;
}

/*
 * @brief CPU模式下，ADC定时通道扫描采集函数
 */
void adc_scan(void *priv)
{
    if ((__this->cur_ch != ADC_MAX_CH) || __this->blocking || __this->dma_conti_mode) {
        return;
    }

    __this->cur_ch = adc_get_next_ch();
    if (__this->cur_ch == ADC_MAX_CH) {
        return;
    }
    adc_sample(adc_queue[__this->cur_ch].ch);
    SFR(JL_ADC->ADC_CON, 14, 1, 1); //clr pending
    SFR(JL_ADC->ADC_CON, 13, 1, 1); //ie
}

/*
 * @brief 阻塞CPU模式采集，打断ADC转换队列，立即获取对应通道的ADC值
 * @ch ADC通道
 * @return 返回通道ADC值
 */
s16 adc_get_value_by_blocking(u32 ch, u8 times)
{
    s32 adc_v = 0;
    u8 sample_cnt = times; //连续采集8次
    __this->blocking = 1;
    SFR(JL_ADC->ADC_CON, 13, 1, 0); //ie
    adc_sample(ch);
    while (sample_cnt--) {
        SFR(JL_ADC->ADC_CON, 14, 1, 1); //clr pending
        while (!(JL_ADC->ADC_CON & BIT(15)));
        adc_v += adc_get_res();
    }
    __this->blocking = 0;
    adc_v /= times;
    if ((ch & ADC_CH_MASK_TYPE_SEL) == AD_DIFF_TEST) {
        adc_v -= (AD_MAX_VALUE / 2);
        adc_v <<= 1;
    }
    __this->cur_ch = ADC_MAX_CH;
    return adc_v;
}

/*
 * @brief 阻塞CPU模式采集，打断ADC转换队列，立即获取对应通道的电压值，内部做了滤波取平均处理
 * @ch ADC通道
 * @return 返回通道ADC值
 */
s16 adc_get_voltage_by_blocking(u32 ch, u8 times)
{
    return adc_value_to_voltage(__this->adc_vbg, adc_get_value_by_blocking(ch, times));
}

/*
 * @brief SARADC CLK INIT
 */
void adc_clk_init(int clk)
{
    int lsb_clk;
    u8 i;
    lsb_clk = clk_get("lsb");
    for (i = 1; i < 0x80; i++) {
        if ((lsb_clk / (i * 2)) <= clk) {
            break;
        }
    }
    SFR(JL_ADC->ADC_CON, 16, 7, i);
}

/*
 * @brief 校准SARADC模拟部分校准
 */
void adc_cal_test(void)
{
    for (int i = 0; i < 20; i++) {
        if (i < 10) {
            SFR(JL_ADC->ADC_CON, 25, 1, 0);
        } else {
            SFR(JL_ADC->ADC_CON, 25, 1, 1);
        }
        SFR(JL_ADC->ADC_CON, 16, 7, i % 4);
        SFR(JL_ADC->ADC_CON, 31, 1, 0);
        SFR(JL_ADC->ADC_CON, 31, 1, 1);
        JL_ADC->ADC_CON |= BIT(2); //cal_kst
        /* delay(100); */
        asm("nop");
        asm("nop");
    }
}

/*
 * @brief SARADC UNINIT
 */
void adc_uninit(void)
{
    JL_ADC->ADC_CON = 0;
    JL_ADC->ANA_CON = 0;
    JL_ADC->DMA_CON0 = 0;
    JL_ADC->DMA_CON1 = 0;
    JL_ADC->DMA_CON2 = 0;
    JL_ADC->DMA_BADR = 0;
    VBG_BUFFER_AND_TEST_EN(0);
    PMU_TOADC_AND_DET_EN(0);
    if (__this->scan_timer) {
        usr_timer_del(__this->scan_timer);
        __this->scan_timer = 0;
    }
    __this->init_ok = 0;
}

/*
 * @brief SARADC INIT
 */
void adc_init(void)
{
    if (__this->init_ok) {
        return;
    }

    gpadc_info.cur_ch = ADC_MAX_CH;

    JL_ADC->ADC_CON = 0x1; //关闭自动复位
    JL_ADC->ANA_CON = 0;
    JL_ADC->DMA_CON0 = 0;
    JL_ADC->DMA_CON1 = 0;
    JL_ADC->DMA_CON2 = 0;
    JL_ADC->DMA_BADR = 0;

    SFR(JL_ADC->ANA_CON, 8, 1, 1); //sense en
    SFR(JL_ADC->ANA_CON, 9, 2, 2);
    SET_SENSE_GAIN(0);
    SFR(JL_ADC->ANA_CON, 13, 1, 1); //vcm en
    SFR(JL_ADC->ANA_CON, 14, 1, 1);
    SFR(JL_ADC->ANA_CON, 15, 2, 2);
    SFR(JL_ADC->ANA_CON, 17, 2, 2);
    SFR(JL_ADC->ANA_CON, 19, 2, 3);
    SFR(JL_ADC->ANA_CON, 21, 2, 3);
    SFR(JL_ADC->ADC_CON, 31, 1, 1);

    adc_cal_test();

    adc_clk_init(ADC_CLK);
    SFR(JL_ADC->ADC_CON, 23, 2, ADC_BIT_SEL);

    VBG_TEST_SEL(VBG_TEST_SEL_MBG08);
    /* VBG_BUFFER_EN(1); */
    /* VBG_TEST_EN(1); */
    VBG_BUFFER_AND_TEST_EN(1);
    /* PMU_TOADC_EN(1); */
    /* PMU_DET_OE(1); */
    PMU_TOADC_AND_DET_EN(1);

    request_irq(IRQ_SARADC_IDX, 0, adc_isr, 0);

    adc_add_sample_ch(AD_ANA_PMU_CH_VBG);

#ifdef TCFG_BAT_DET_IO
#if (TCFG_BAT_DET_IO != NO_CONFIG_PORT)
    gpio_set_mode(IO_PORT_SPILT(TCFG_BAT_DET_IO), PORT_INPUT_FLOATING);
    gpio_set_function(IO_PORT_SPILT(TCFG_BAT_DET_IO), PORT_FUNC_GPADC);
#endif
    adc_add_sample_ch(TCFG_BAT_AD_CHANNEL);
#endif

    if (!__this->scan_timer) {
        adc_scan(NULL);
        __this->scan_timer = usr_timer_add(NULL, adc_scan, 10, 0);
    }
    __this->init_ok = 1;
}

/*
 * @brief when clock sw, adc clk reinitialize
 */
static u16 dma_ch_en;
static void clock_critical_enter(void)
{
    if (__this->init_ok) {
        __this->blocking = 1;
        dma_ch_en = (u16)JL_ADC->DMA_CON1;
        SFR(JL_ADC->ADC_CON, 13, 1, 0); //ie
        SFR(JL_ADC->DMA_CON0, 0, 1, 0); //dma ie
        SFR(JL_ADC->DMA_CON1, 0, 16, 0); //chl en
        while (!(JL_ADC->ADC_CON & BIT(15))) {}
        if (dma_ch_en) {
            while (!(JL_ADC->DMA_CON0 & BIT(15))) {}
        }
    }
}
static void clock_critical_exit(void)
{
    if (__this->init_ok) {
        if (__this->dma_conti_mode) {
            SFR(JL_ADC->DMA_CON0, 1, 1, 0);
            adc_clk_init(ADC_DMA_CONTI_CLK);
            SFR(JL_ADC->DMA_CON0, 1, 1, 1);
        } else {
            adc_clk_init(ADC_CLK);
        }
        if (dma_ch_en) {
            SFR(JL_ADC->DMA_CON0, 0, 1, 1); //dma ie
            SFR(JL_ADC->DMA_CON1, 0, 16, dma_ch_en); //kst
        }
        __this->blocking = 0;
        __this->cur_ch = ADC_MAX_CH;
    }
}
LSB_CRITICAL_HANDLE_REG(saradc, clock_critical_enter, clock_critical_exit)
