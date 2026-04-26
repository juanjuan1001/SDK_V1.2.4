#ifndef __DAC_API_H__
#define __DAC_API_H__

#include "includes.h"

/*method of application
1.dac_drv->ioctrl(&31,DAC_SET_MAX_VOL);
2.dac_drv->ioctrl(&31,DAC_SET_CUR_VOL);
3.dac_drv->ioctrl(vol_tab,DAC_REG_VOL_TAB);
4.dac_drv->open(info);
5.dac_drv->close();
*/

#define DAC_MODE_PWM_APA        0//DACP,DACN输出,差分驱动喇叭
#define DAC_MODE_DSM_PWM_APA    1//DACP,DACN输出,差分驱动喇叭,有隔直电容是DSM可加直流偏置(1024)
#define DAC_MODE_DSM_APA        2//DACP,DACN输出,差分驱动喇叭,有隔直电容是DSM可加直流偏置(1024)
#define DAC_MODE_DSM_RDAC       3//PA0,PA1输出,单端或差分驱动喇叭;单端驱动时,必须加隔直电容;
//差分驱动时,有隔直电容是DSM可加直流偏置(1024)

typedef void (*dac_func)(void *priv, void *buf, u32 len);

enum {
    APA_DT_1_23,//1.23ns
    APA_DT_1_73,//1.73ns
    APA_DT_2_420,//2.42ns
    APA_DT_2_421,//2.42ns
    APA_DT_2_05,//2.05ns
    APA_DT_2_87,//2.87ns
    APA_DT_3_620,//3.62ns
    APA_DT_3_621,//3.62ns
};

typedef struct _DAC_INF {
    u32 def_sr;//默认采样率
    dac_func dac_input;
    void *priv;
    u8 dac_mode;
    u8 fs_2M_sr;
    u8 dcc_en;
} DAC_INF;

enum {              //传入参数      功能解释
    DAC_REG_VOL_TAB,//u16 *        注册音量表
    DAC_GET_MAX_VOL,//u8 *         获取最大音量
    DAC_SET_MAX_VOL,//u8 *         设置最大音量
    DAC_GET_CUR_VOL,//u8 *         获取当前音量
    DAC_SET_CUR_VOL,//u8 *         设置当前音量
    DAC_CLR_DMA_BUF,//NULL         清除DMABUF数据
    DAC_SET_SR,     //u16 *        设置采样率
    DAC_GET_SR,     //u16 *        获取采样率
    DAC_IE_CTL,     //u8 *         控制IE
};

struct dac_driver {
//==================================================================
//函数名：  dac_drv->open
//作者：    xiebin
//日期：    2018/06/04
//功能：    打开dac模块
//输入参数：info:初始化DAC的参数
//返回值：  0:正确 !0:错误
//==================================================================
    s32(*open)(DAC_INF *info);
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
//输入参数：info:初始化DAC的参数
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

extern const struct dac_driver *dac_drv;

#endif  //__DAC_API_H__
