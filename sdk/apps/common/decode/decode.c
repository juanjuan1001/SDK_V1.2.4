/*
* @file  		decode.c
* @brief        UMP3音频解码部分
* @details		UMP3 DECODER API
* @author		JL
* @date     	2023-06-09
* @version  	V1.0
* @copyright    Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
 */

#include "circular_buf.h"
#include "decode.h"
#include "app_config.h"
#include "asm/hwi.h"
#include "event.h"
#include "voice.h"
#include "asm/hadc.h"
#include "if_decoder_ctrl.h"
#include "resample_api.h"

#define LOG_TAG_CONST   DECODE
#define LOG_TAG	        "[UMP3_DEC]"
#include "debug.h"

#define DECODE_OUTPUT_DAC			0
#define DECODE_OUTPUT_APA			1
#define DECODE_OUTPUT_WAY			DECODE_OUTPUT_APA //音频播放输出配置

#if (DECODE_OUTPUT_WAY == DECODE_OUTPUT_APA)
#include "apa_api.h"
#define DECODE_REG_VOL_TAB			APA_REG_VOL_TAB
#define DECODE_GET_MAX_VOL			APA_GET_MAX_VOL
#define DECODE_SET_MAX_VOL			APA_SET_MAX_VOL
#define DECODE_GET_CUR_VOL			APA_GET_CUR_VOL
#define DECODE_SET_CUR_VOL			APA_SET_CUR_VOL
#define DECODE_CLR_DMA_BUF			APA_CLR_DMA_BUF
#define DECODE_SET_SR				APA_SET_SR
#define DECODE_GET_SR				APA_GET_SR
#define DECODE_IE_CTL				APA_IE_CTL
#define DECODE_DEFAULT_SR      		(32000) //初始化的采样率
#define decode_drv					apa_drv
#else
#include "dac_api.h"
#define DAC_PA_MUTE_PORT			IO_PORTA_01
#define DECODE_REG_VOL_TAB			DAC_REG_VOL_TAB
#define DECODE_GET_MAX_VOL			DAC_GET_MAX_VOL
#define DECODE_SET_MAX_VOL			DAC_SET_MAX_VOL
#define DECODE_GET_CUR_VOL			DAC_GET_CUR_VOL
#define DECODE_SET_CUR_VOL			DAC_SET_CUR_VOL
#define DECODE_CLR_DMA_BUF			DAC_CLR_DMA_BUF
#define DECODE_SET_SR				DAC_SET_SR
#define DECODE_GET_SR				DAC_GET_SR
#define DECODE_IE_CTL				DAC_IE_CTL
#define DECODE_DEFAULT_SR      		(32000) //初始化的采样率
#define decode_drv					dac_drv
#endif

#define DECODE_SOFT_ISR     		IRQ_SOFT0_IDX 	   //使用的软中断
#define DEC_THRESHOLD_VALUE 		(720*2L)   		   //decode_cbuf小于阈值则启动软中断解码
#define OBUF_SIZE           		(18*64*4L)   	   //pcm_buf_size每次解码输出18*64个点
#define MAX_SYS_VOL					(31)		  	   //最大音量等级
#define DEFUAL_SYS_VOL				(MAX_SYS_VOL)      //系统默认音量

struct _decode_info {
    cbuffer_t decode_cbuf;
    volatile u8 play_status;
    volatile u8 decode_status;
    SDFILE *decode_fp;
    u8 *ump3_decode_buf;
    u8 *pcm_obuf;
    audio_decoder_ops *ump3_ops;
    RS_STUCT_API *rs_ops;
    u8 *rs_buf;
    u8 init_ok;
};
static struct _decode_info decode_info;
#define __this (&decode_info)

//数字音量等级表
const u16 digital_vol_tab[MAX_SYS_VOL + 1] = {
    0,     93,   111,  132,  158,  189,  226,  270,   323,  386,   //9
    462,   552,  660,  789,  943,  1127, 1347, 1610,  1925, 2301,  //19
    2751,  3288, 3930, 4698, 5616, 6713, 8025, 9592, 11466, 15200, //29
    16000, 16384												   //31
};

//音频文件列表
const char *const tone_table[] = {
    TONE_NUM_0, 	   TONE_NUM_1,      TONE_NUM_2,      TONE_NUM_3,      TONE_NUM_4,
    TONE_NUM_5,        TONE_NUM_6,      TONE_NUM_7,      TONE_NUM_8,      TONE_NUM_9,
    TONE_ALARM, 	   TONE_AVERAGE,    TONE_BAI,        TONE_BASEWORLD,  TONE_BEGIN,
    TONE_CONTINUEHIGH, TONE_DIAN,       TONE_END,        TONE_ERROR,      TONE_HIGH,
    TONE_KP,		   TONE_LOW,        TONE_LOWPOWER,   TONE_MAIBO,      TONE_MMHG,
    TONE_NORMAL,	   TONE_NORMALHIGH, TONE_PLEASESEAT, TONE_REGISTER,   TONE_SHI,
    TONE_SHOUSUOYA,    TONE_SHUZHANGYA, TONE_THANKS,     TONE_ZHUYI,
};

#if (DECODE_OUTPUT_WAY == DECODE_OUTPUT_DAC)
/*
 * @brief dac音频输出需外接PA，此函数用于控制PA是否mute
 * @param mute 0:正常输出 1:mute住
 */
static void dac_pa_mute_enable(u8 mute)
{
    gpio_set_mode(IO_PORT_SPILT(DAC_PA_MUTE_PORT), !mute);
}
#endif

/*
 * @brief 解码器解码输入函数
 * @param addr: 解码数据地址，buf: 解码数据存入buf，len：解码数据长度
 * @return 返回解码数据长度
 */
static int ump3_dec_input(void *priv, u32 addr, void *buf, int len, u8 type)
{
    fseek(__this->decode_fp, addr, SEEK_SET);
    return fread(__this->decode_fp, buf, len);
}

/*
 * @brief 解码器解码数据输出函数
 */
static u32 ump3_dec_output(void *priv, void *data, int len)
{
    if (__this->rs_buf) {
        return __this->rs_ops->run((u32 *)__this->rs_buf, (short *)data, len);
    } else {
        return cbuf_write(&(__this->decode_cbuf), (u8 *)data, len);
    }
}

/*
 * @brief 重采样数据输出函数
 */
static int resample_output(void *priv, void *data, int len)
{
    return cbuf_write(&(__this->decode_cbuf), (u8 *)data, len);
}

/*
 * @brief 写数和触发解码器解码函数
 * @param buf: 写数存入的buf指针，len: 写入数据长度
 */
static int decode_isr_callback(void *priv, void *buf, u32 len)
{
    u32 buf_len;

    memset(buf, 0, len);

    if (__this->play_status != MAD_PLAY) {
        return 0;
    }

    buf_len = __this->decode_cbuf.data_len;
    buf_len = buf_len >= len ? len : buf_len;
    cbuf_read(&(__this->decode_cbuf), buf, buf_len);

    if (__this->decode_cbuf.data_len < DEC_THRESHOLD_VALUE) {
        if (__this->decode_status == DEC_ING) {
            bit_set_swi(DECODE_SOFT_ISR);
        }
    }

    decode_drv->vol_ctrl(buf, buf_len);

    //解码结束并且缓存消耗完成时,发送播放结束消息
    if ((buf_len == 0) && (__this->decode_status == DEC_END)) {
        __this->play_status = MAD_END;
        sys_msg_event_notify(MSG_EVENT_DECODE_END, 0);
    }
    return len;
}

/*
 * @brief 解码器中断回调，运行解码器
 */
___interrupt
static void decode_run(void)
{
    if (__this->decode_status == DEC_ING) {
        if (__this->ump3_ops->run(__this->ump3_decode_buf, 0)) {//0:正常播放 1:快进 2:快退
            __this->decode_status = DEC_END;
        }
    }
    bit_clr_swi(DECODE_SOFT_ISR);
}

/*
 * @brief 初始化解码器
 */
void decode_init(void)
{
    u8 vol;

    if (__this->init_ok) {
        return;
    }

    vol = DEFUAL_SYS_VOL;
    __this->play_status = MAD_STOP;
    __this->ump3_ops = get_ump3_ops();
    assert_d(__this->ump3_ops);

    __this->rs_ops = get_resample_context();
    assert_d(__this->rs_ops);

    request_irq(DECODE_SOFT_ISR, 0, (void *)decode_run, 0);

#if (DECODE_OUTPUT_WAY == DECODE_OUTPUT_APA)
    APA_INF info = {
        .pwm_mode = APA_PWM_MODE2,
        .pwm_clk = APA_CLK_240M,
        .def_sr = DECODE_DEFAULT_SR,
        .priv = NULL,
        .apa_input = (apa_func)decode_isr_callback,
        .apa_p_mode = APA_PWM_MODE,
        .apa_n_mode = APA_PWM_MODE,
        .dcc_en = 1,
        .ac_dit = 0,
    };
#else
    DAC_INF info = {
        .priv = NULL,
        .def_sr = DECODE_DEFAULT_SR,
        .dac_input = (dac_func)decode_isr_callback,
        .dac_mode = 0,
        .dcc_en = 1,
    };
    hadc_avddcp_config(1, AVDDCP_MODE_SEL_NORMAL, AVDDCP_VOL_SEL_3P60);
    hadc_avddr_config(1, AVDDR_VOL_SEL_3P0);
    hadc_avbg_config(1, 0, 1);
    hadc_avcm_config(1, 1);
    hadc_pa_config(1, 0, HADC_PA_MUXP_SEL_AVBG, HADC_PA_MUXN_DACO_BIAS, HADC_PA_GAIN_SEL_M10db);
    dac_pa_mute_enable(1);
#endif

    decode_drv->ioctrl((void *)&vol, DECODE_SET_MAX_VOL);
    decode_drv->ioctrl((void *)&vol, DECODE_SET_CUR_VOL);
    decode_drv->ioctrl((void *)digital_vol_tab, DECODE_REG_VOL_TAB);
    decode_drv->open(&info);

    __this->init_ok = 1;
}

/*
 * @brief 解码器停止工作，释放资源
 */
void decode_uninit(void)
{
    if (__this->init_ok == 0) {
        return;
    }

    a_player_stop(0);

    decode_drv->close();

#if (DECODE_OUTPUT_WAY == DECODE_OUTPUT_DAC)
    hadc_avddcp_config(0, AVDDCP_MODE_SEL_NORMAL, AVDDCP_VOL_SEL_3P60);
    hadc_avddr_config(0, AVDDR_VOL_SEL_3P0);
    hadc_avbg_config(0, 0, 0);
    hadc_avcm_config(0, 0);
    hadc_pa_config(0, 0, HADC_PA_MUXP_SEL_AVBG, HADC_PA_MUXN_DACO_BIAS, HADC_PA_GAIN_SEL_M10db);
    dac_pa_mute_enable(1);
#endif

    unrequest_irq(DECODE_SOFT_ISR);

    __this->init_ok = 0;
}

/*
 * @brief 暂停/启动 音频播放函数
 * @return 返回当前播放状态
 */
u8 a_player_pause(void)
{
    u8 res;

    res = __this->play_status;

    if (__this->play_status != MAD_PAUSE && __this->play_status != MAD_PLAY) {
        return res;
    }

    if (__this->play_status == MAD_PAUSE) {
        __this->play_status = MAD_PLAY;
    } else if (__this->play_status == MAD_PLAY) {
        __this->play_status = MAD_PAUSE;
    }

    return res;
}

/*
 * @brief 结束音频播放，关闭播放器
 * @param wait: 1: 等待当前音频播放结束 0：不等待
 */
void a_player_stop(u8 wait)
{
    if (MAD_STOP == __this->play_status) {
        return;
    }

    while (wait && (__this->play_status == MAD_PLAY));

    __this->play_status = MAD_STOP;

#if (DECODE_OUTPUT_WAY == DECODE_OUTPUT_DAC)
    dac_pa_mute_enable(wait);
#endif

    cbuf_clear(&(__this->decode_cbuf));

    if (__this->decode_fp) {
        fclose(__this->decode_fp);
        __this->decode_fp = NULL;
    }

    if (__this->ump3_decode_buf) {
        free(__this->ump3_decode_buf);
        __this->ump3_decode_buf = NULL;
    }

    if (__this->pcm_obuf) {
        free(__this->pcm_obuf);
        __this->pcm_obuf = NULL;
    }

    if (__this->rs_buf) {
        free(__this->rs_buf);
        __this->rs_buf = NULL;
    }
}

/*
 * @brief 通过文件索引，播放音频文件
 * @param index: 音频文件索引
 * @return DEC_FAIL: 解码失败 DEC_SUCC: 解码成功
 */
u8 a_player_tonebyindex(enum INDEX_TONE index)
{
    u16 sample_rate, dac_sr;
    u32 need_buf_size;
    RS_PARA_STRUCT rs_para;

    RS_IO_CONTEXT rs_io = {
        .priv = NULL,
        .output = resample_output,
    };

    AUDIO_DECODE_PARA modevalue = {
        .mode = 1,
    };

    struct if_decoder_io ump3_dec_io = {
        .priv = NULL,
        .input = ump3_dec_input,
        .output = ump3_dec_output,
    };

    decode_init();

    a_player_stop(0);

    need_buf_size = __this->ump3_ops->need_dcbuf_size();
    __this->ump3_decode_buf = (u8 *)malloc(need_buf_size);
    if (__this->ump3_decode_buf == NULL) {
        log_error("ump3 decode buf malloc failed!!!\n");
        goto __player_err;
    }

    __this->pcm_obuf = (u8 *)malloc(OBUF_SIZE);
    if (__this->pcm_obuf == NULL) {
        log_error("pcm obuf malloc failed!!!\n");
        goto __player_err;
    }

    cbuf_init(&(__this->decode_cbuf), __this->pcm_obuf, OBUF_SIZE);

    __this->decode_fp = fopen(tone_table[index], "r");
    if (__this->decode_fp == NULL) {
        log_error("open file %s err!\n", tone_table[index]);
        goto __player_err;
    }
    log_info("tone index: %d, path: %s\n", index, tone_table[index]);

    __this->ump3_ops->open(__this->ump3_decode_buf, &ump3_dec_io, NULL);
    __this->ump3_ops->dec_confing(__this->ump3_decode_buf, SET_DECODE_MODE, &modevalue);
    if (__this->ump3_ops->format_check(__this->ump3_decode_buf)) {
        free(__this->ump3_decode_buf);
        log_error("ump3 format check failed!!!\n");
        goto __player_err;
    }

    sample_rate = (__this->ump3_ops->get_dec_inf(__this->ump3_decode_buf))->sr;
    decode_drv->ioctrl(&sample_rate, DECODE_SET_SR);
    decode_drv->ioctrl(&dac_sr, DECODE_GET_SR);
    if (sample_rate != dac_sr) {
        log_info("sample_rate: %d, dac_sr: %d", sample_rate, dac_sr);
        need_buf_size = __this->rs_ops->need_buf();
        __this->rs_buf = (u8 *)malloc(need_buf_size);
        if (__this->rs_buf == NULL) {
            log_error("rs buf malloc failed!!!\n");
            goto __player_err;
        }
        rs_para.insample = sample_rate;
        rs_para.outsample = dac_sr;
        rs_para.quality = 8;
        if (__this->rs_ops->open((u32 *)__this->rs_buf, &rs_para, &rs_io)) {
            log_error("rs open failed!!!\n");
            goto __player_err;
        }
    }

#if (DECODE_OUTPUT_WAY == DECODE_OUTPUT_DAC)
    dac_pa_mute_enable(0);
#endif

    __this->decode_status = DEC_ING;
    __this->play_status = MAD_PLAY;
    u8 ie_en = 1;
    decode_drv->ioctrl((void *)&ie_en, DECODE_IE_CTL);
    return DEC_SUCC;

__player_err:
    if (__this->decode_fp) {
        fclose(__this->decode_fp);
        __this->decode_fp = NULL;
    }

    if (__this->ump3_decode_buf) {
        free(__this->ump3_decode_buf);
        __this->ump3_decode_buf = NULL;
    }

    if (__this->pcm_obuf) {
        free(__this->pcm_obuf);
        __this->pcm_obuf = NULL;
    }

    if (__this->rs_buf) {
        free(__this->rs_buf);
        __this->rs_buf = NULL;
    }
    return DEC_FAIL;
}

