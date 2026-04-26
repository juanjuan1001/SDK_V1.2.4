/*
 * @file  		sin_generator.c
 * @brief        DAC正弦信号发生器应用接口
 * @details		SIN_GENERATOR API
 * @author		JL
 * @date     	2023-06-19
 * @version  	V1.0
 * @copyright    Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
 */

#include "sin_generator.h"
#include "dac_api.h"
#include "app_config.h"

#define MAX_SYS_VOL			(31)		  	//最大音量等级
#define DEFUAL_SYS_VOL		(MAX_SYS_VOL)   //系统默认音量

//数字音量等级表
const u16 dac_digital_vol_tab[MAX_SYS_VOL + 1] = {
    0,     93,   111,  132,  158,  189,  226,  270,   323,  386,   //9
    462,   552,  660,  789,  943,  1127, 1347, 1610,  1925, 2301,  //19
    2751,  3288, 3930, 4698, 5616, 6713, 8025, 9592, 11466, 15200, //29
    16000, 16384												   //31
};

// -6db (2M模式)
const s8 sin_source[80] = {
    0x01, 0x00, 0x09, 0x0a, 0xd4, 0x13, 0x1f, 0x1d, 0xb5, 0x25, 0x5d, 0x2d, 0xe7, 0x33, 0x29, 0x39,
    0x04, 0x3d, 0x5c, 0x3f, 0x26, 0x40, 0x5d, 0x3f, 0x03, 0x3d, 0x29, 0x39, 0xe6, 0x33, 0x5e, 0x2d,
    0xb5, 0x25, 0x1f, 0x1d, 0xd2, 0x13, 0x09, 0x0a, 0x00, 0x00, 0xf7, 0xf5, 0x2d, 0xec, 0xe0, 0xe2,
    0x4b, 0xda, 0xa3, 0xd2, 0x19, 0xcc, 0xd7, 0xc6, 0xfd, 0xc2, 0xa4, 0xc0, 0xda, 0xbf, 0xa3, 0xc0,
    0xfd, 0xc2, 0xd7, 0xc6, 0x1a, 0xcc, 0xa4, 0xd2, 0x4a, 0xda, 0xe0, 0xe2, 0x2d, 0xec, 0xf7, 0xf5,
};

const u32 dac_f[9] = {5, 20, 25, 30, 40, 50, 75, 100, 150};

static u32 sin_buf_len;
static s8 *sin_buf_ptr = NULL;
static u32 sin_source_len;

/*
 * @brief DAC中断回调函数
 * @param priv: 回调参数，dac_buf: dac写数存入的buf指针，len: 写入数据长度
 */
__attribute__((noinline))
static void dac_isr_callback(s8 *priv, s8 *dac_buf, u32 len)
{
    u32 offset;
    u8 i, j, k;

    if (sin_buf_len < len) {
        memcpy(dac_buf, sin_buf_ptr, sin_buf_len);
        offset = sin_buf_len;
        len -= offset;
        sin_buf_len = sin_source_len;
        sin_buf_ptr = priv;
        j = len / sin_buf_len;
        k = len % sin_buf_len;
        for (i = 0; i < j; i++) {
            memcpy(dac_buf + offset, sin_buf_ptr, sin_buf_len);
            offset += sin_buf_len;
        }
        memcpy(dac_buf + offset, sin_buf_ptr, k);
        sin_buf_len -= k;
        sin_buf_ptr += k;
    } else {
        memcpy(dac_buf, sin_buf_ptr, len);
        sin_buf_len -= len;
        sin_buf_ptr += len;
    }
}

/*
 * @brief 关闭正弦信号发生器，关闭DAC
 */
void sin_generator_close(void)
{
    u8 ie_en = 0;
    hadc_pa_config(0, 0, HADC_PA_MUXP_SEL_AVBG, HADC_PA_MUXN_DACO_BIAS, HADC_PA_GAIN_SEL_DISABLE);
    hadc_avddcp_config(0, AVDDCP_MODE_SEL_NORMAL, AVDDCP_VOL_SEL_3P60);
    hadc_avddr_config(0, AVDDR_VOL_SEL_3P0);
    hadc_avbg_config(0, 1, 1);
    dac_drv->close();
    dac_drv->ioctrl((void *)&ie_en, DAC_IE_CTL);
    dac_drv->analog_close();
}

/*
 * @brief 启动正弦信号发生器，启动DAC
 * @param enum sin_source sin: 正弦激励信号源频率
 * 		  enum hadc_pa_gain_sel gain: PA增益
 */
__attribute__((noinline))
void sin_generator_start(enum sin_source sin, enum hadc_pa_gain_sel gain)
{
    u8 ie_en = 1;
    u8 vol = DEFUAL_SYS_VOL;
    u32 dac_sr;

    sin_source_len = ARRAY_SIZE(sin_source);
    sin_buf_ptr = (s8 *)sin_source;
    sin_buf_len = sin_source_len;

    dac_sr = dac_f[sin] * 1000 * (sin_source_len / 2);
    DAC_INF info = {
        .priv = (void *)sin_source,
        .def_sr = dac_sr,
        .dac_input = (dac_func)dac_isr_callback,
        .dac_mode = 1,
        .fs_2M_sr = 12000000L / dac_sr - 1,
        .dcc_en = 1,
    };

    sin_generator_close();

    hadc_avddcp_config(1, AVDDCP_MODE_SEL_NORMAL, AVDDCP_VOL_SEL_3P60);
    hadc_avddr_config(1, AVDDR_VOL_SEL_3P0);
    hadc_avbg_config(1, 1, 1);
    hadc_pa_config(1, 0, HADC_PA_MUXP_SEL_AVBG, HADC_PA_MUXN_DACO_BIAS, gain);

    dac_drv->ioctrl((void *)&vol, DAC_SET_MAX_VOL);
    dac_drv->ioctrl((void *)&vol, DAC_SET_CUR_VOL);
    dac_drv->ioctrl((void *)dac_digital_vol_tab, DAC_REG_VOL_TAB);

    dac_drv->open(&info);
    dac_drv->ioctrl((void *)&ie_en, DAC_IE_CTL);
    dac_drv->analog_open();
}
