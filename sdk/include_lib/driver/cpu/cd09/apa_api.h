#ifndef __APA_API_H__
#define __APA_API_H__

#include "includes.h"
// #include "dac_cpu.h"

/*method of application
1.dac_drv->ioctrl(&31,DAC_SET_MAX_VOL);
2.dac_drv->ioctrl(&31,DAC_SET_CUR_VOL);
3.dac_drv->ioctrl(vol_tab,DAC_REG_VOL_TAB);
4.dac_drv->open(info);
5.dac_drv->close();
*/
#define APA_CLK_320M    1
#define APA_CLK_240M    2 //default
#define APA_CLK_HSB     3

#define APA_PWM_MODE    0 //APA输出音频信号
#define APA_IO_MODE     1 //APA做IO输出高低电平

#define APA_PWM_MODE1   0 //APA_PWM 调制模式1（单端/差分，推荐单端）
#define APA_PWM_MODE2   1 //APA_PWM 调制模式2（差分）

typedef int (*apa_func)(void *priv, void *buf, u32 len);

typedef struct _APA_INF {
    u8 ac_dit;
    u8 apa_p_mode;
    u8 apa_n_mode;
    u8 pwm_mode;
    u8 pwm_clk;
    u16 def_sr;//默认采样率
    apa_func apa_input;
    u8 dcc_en;
    void *priv;
} APA_INF;

enum {              //传入参数      功能解释
    APA_REG_VOL_TAB,//u16 *        注册音量表
    APA_GET_MAX_VOL,//u8 *         获取最大音量
    APA_SET_MAX_VOL,//u8 *         设置最大音量
    APA_GET_CUR_VOL,//u8 *         获取当前音量
    APA_SET_CUR_VOL,//u8 *         设置当前音量
    APA_CLR_DMA_BUF,//NULL         清除DMABUF数据
    APA_SET_SR,     //u16 *        设置采样率
    APA_GET_SR,     //u16 *        获取采样率
    APA_IE_CTL,     //u8 *         控制IE
};

struct apa_driver {
//==================================================================
//函数名：  dac_drv->open
//作者：    xiebin
//日期：    2018/06/04
//功能：    打开dac模块
//输入参数：info:初始化DAC的参数
//返回值：  0:正确 !0:错误
//==================================================================
    s32(*open)(APA_INF *info);
//==================================================================
//函数名：  dac_drv->close
//作者：    xiebin
//日期：    2018/06/04
//功能：    关闭dac模块
//输入参数：void
//返回值：  0:正确 !0:错误
//==================================================================
    s32(*close)(void);
//==================================================================
//函数名：  dac_drv->analog_open
//作者：    xiebin
//日期：    2018/06/04
//功能：    打开dac模拟模块
//输入参数：void
//返回值：  0:正确 !0:错误
//==================================================================
    s32(*analog_open)(void);
//==================================================================
//函数名：  dac_drv->analog_close
//作者：    xiebin
//日期：    2018/06/04
//功能：    关闭dac模拟模块
//输入参数：void
//返回值：  0:正确 !0:错误
//==================================================================
    s32(*analog_close)(void);
//==================================================================
//函数名：  dac_drv->ioctrl
//作者：    xiebin
//日期：    2018/06/04
//功能：    根据不同的命令执行不同的操作
//输入参数：parm 参数
//          cmd 命令
//返回值：  0:正确 !0:错误
//==================================================================
    s32(*ioctrl)(void *parm, u32 cmd);
//==================================================================
//函数名：  dac_drv->vol_ctrl
//作者：    xiebin
//日期：    2018/06/04
//功能：    根据音量表转换DAC的值
//输入参数：buf dac数据
//          len 数据长度
//返回值：  0:正确 !0:错误
//==================================================================
    s32(*vol_ctrl)(void *buf, u32 len);
};

extern const struct apa_driver *apa_drv;
extern void set_apap_output_status(u8 val);
extern void set_apan_output_status(u8 val);

#endif  //__APA_API_H__

