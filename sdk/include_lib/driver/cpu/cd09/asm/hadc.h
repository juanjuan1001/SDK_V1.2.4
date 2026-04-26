#ifndef __CPU_HADC_H__
#define __CPU_HADC_H__

#include "typedef.h"

#define HADC_NORMAL_DMA_POINTS  20

//avddcp使用模式
enum avddcp_mode_sel {
    AVDDCP_MODE_SEL_NORMAL, //正常升压
    AVDDCP_MODE_SEL_VPWR,   //VPWR直通模式
};

//avddcp电源档位
enum avddcp_vol_sel {
    AVDDCP_VOL_SEL_3P40,
    AVDDCP_VOL_SEL_3P45,
    AVDDCP_VOL_SEL_3P50,
    AVDDCP_VOL_SEL_3P55,
    AVDDCP_VOL_SEL_3P60,//default
    AVDDCP_VOL_SEL_3P65,
    AVDDCP_VOL_SEL_3P70,
    AVDDCP_VOL_SEL_3P75,
};

//avddr电源档位
enum avddr_vol_sel {
    AVDDR_VOL_SEL_2P7,
    AVDDR_VOL_SEL_2P8,
    AVDDR_VOL_SEL_2P9,
    AVDDR_VOL_SEL_3P0,//default
    AVDDR_VOL_SEL_3P1,
    AVDDR_VOL_SEL_3P2,
    AVDDR_VOL_SEL_3P3,
    AVDDR_VOL_SEL_3P4,
};

//HADC 模拟IO N端通道选择
enum hadc_muxn_sel {
    HADC_MUXN_SEL_AIN0,
    HADC_MUXN_SEL_AIN1,
    HADC_MUXN_SEL_AIN2,
    HADC_MUXN_SEL_AIN3,
    HADC_MUXN_SEL_AIN4,
    HADC_MUXN_SEL_AIN5,
    HADC_MUXN_SEL_AIN6,
    HADC_MUXN_SEL_AIN7,
    HADC_MUXN_SEL_TEMP,
    HADC_MUXN_SEL_VCM,
    HADC_MUXN_SEL_VBGBUF,
    HADC_MUXN_SEL_VBIMN,
    HADC_MUXN_SEL_VBIMP,
    HADC_MUXN_SEL_AVSS,
};

//HADC 模拟IO P端通道选择
enum hadc_muxp_sel {
    HADC_MUXP_SEL_AIN0,
    HADC_MUXP_SEL_AIN1,
    HADC_MUXP_SEL_AIN2,
    HADC_MUXP_SEL_AIN3,
    HADC_MUXP_SEL_AIN4,
    HADC_MUXP_SEL_AIN5,
    HADC_MUXP_SEL_AIN6,
    HADC_MUXP_SEL_AIN7,
    HADC_MUXP_SEL_TEMP,
    HADC_MUXP_SEL_VCM,
    HADC_MUXP_SEL_VBGBUF,
    HADC_MUXP_SEL_VBIMN,
    HADC_MUXP_SEL_VBIMP,
    HADC_MUXP_SEL_AVSS,
};

//HADC PGA 增益选择
enum hadc_gain_sel {
    HADC_GAIN_SEL_X1,
    HADC_GAIN_SEL_X4,
    HADC_GAIN_SEL_X8 = 3,
    HADC_GAIN_SEL_X16 = 5,
    HADC_GAIN_SEL_X32 = 7,
    HADC_GAIN_SEL_X64 = 9,
    HADC_GAIN_SEL_X128 = 11,
};

//HADC OPA 正端输入选择
enum hadc_opa_muxp_sel {
    HADC_OPA_MUXP_SEL_AIN3,
    HADC_OPA_MUXP_SEL_AIN4,
    HADC_OPA_MUXP_SEL_DACBIAS,
    HADC_OPA_MUXP_SEL_VBGBUF,
    HADC_OPA_MUXP_SEL_DISABLE,
};

//HADC OPA 负端输入选择
enum hadc_opa_muxn_sel {
    HADC_OPA_MUXN_SEL_AIN3,
    HADC_OPA_MUXN_SEL_AIN4,
    HADC_OPA_MUXN_SEL_AIN5,
    HADC_OPA_MUXN_SEL_AIN6,
    HADC_OPA_MUXN_SEL_R250,
    HADC_OPA_MUXN_SEL_R1K,
    HADC_OPA_MUXN_SEL_DISABLE,
};

//HADC OPA 负端测量体脂通道选择
enum hadc_opa_muxi_sel {
    HADC_OPA_MUXI_SEL_AIN2,
    HADC_OPA_MUXI_SEL_AIN6,
    HADC_OPA_MUXI_SEL_AIN7_DACPA,
    HADC_OPA_MUXI_SEL_DISABLE,
};

//HADC OPA 输出端测量体脂信号通道选择
enum hadc_opa_muxo_sel {
    HADC_OPA_MUXO_SEL_AIN3,
    HADC_OPA_MUXO_SEL_AIN4,
    HADC_OPA_MUXO_SEL_AIN5,
    HADC_OPA_MUXO_SEL_AIN6,
    HADC_OPA_MUXO_SEL_R250,
    HADC_OPA_MUXO_SEL_R1K,
    HADC_OPA_MUXO_SEL_DISABLE,
};

//HADC PA 正端输入选择
enum hadc_pa_muxp_sel {
    HADC_PA_MUXP_SEL_AVBG,
    HADC_PA_MUXP_SEL_AIN2,
    HADC_PA_MUXP_SEL_DACO_BIAS,
};

//HADC PA 负端输入选择
enum hadc_pa_muxn_sel {
    HADC_PA_MUXN_DACO_BIAS,
    HADC_PA_MUXN_AIN4,
    HADC_PA_MUXN_SEL_DISABLE, //断开N端
};

//HADC PA GAIN选择
enum hadc_pa_gain_sel {
    HADC_PA_GAIN_SEL_M14db,
    HADC_PA_GAIN_SEL_M12db,
    HADC_PA_GAIN_SEL_M10db,
    HADC_PA_GAIN_SEL_M8db,
    HADC_PA_GAIN_SEL_M6db,
    HADC_PA_GAIN_SEL_M4db,
    HADC_PA_GAIN_SEL_M2db,
    HADC_PA_GAIN_SEL_M0db,
    HADC_PA_GAIN_SEL_DISABLE,
};

//HADC VBIM 通道选择
enum hadc_vbim_chl_sel {
    HADC_VBIM_CHL_SEL_R250,
    HADC_VBIM_CHL_SEL_R1K,
    HADC_VBIM_CHL_SEL_EXTERN,//AIN6 & AIN3
};

//ass debug clk out
enum ass_debug_clk_sel {
    ASS_DBG_CLKO_SEL_DACC_SYNC = 1,
    ASS_DBG_CLKO_SEL_DACC,
    ASS_DBG_CLKO_SEL_APA_DSM_CLK,
    ASS_DBG_CLKO_SEL_HADC_CLK,
    ASS_DBG_CLKO_SEL_HADC_ANA_CLK,
    ASS_DBG_CLKO_SEL_HADC_CIC0_CLK,
    ASS_DBG_CLKO_SEL_HADC_CIC1_CLK,
    ASS_DBG_CLKO_SEL_HADC_CHOP_DMS_CLK,
    ASS_DBG_CLKO_SEL_HADC_CHOP_PGA_CLK,
    ASS_DBG_CLKO_SEL_HADC_CHOP_REF_CLK,
    ASS_DBG_CLKO_SEL_AVDDCP_REF_CLKO,
};

//ass debug data out
enum ass_debug_dato_sel {
    ASS_DBG_DATO_SEL_DSMO = 1,//{1'b0, dsmo[[0][3:0]]}
    ASS_DBG_DATO_SEL_PAP,//{3'b0, apa_nout, apa_pout}
    ASS_DBG_DATO_SEL_HADC,//{3'b0, hadcdi, hadc_ana_rst}
};

//CIC0抽取滤波器阶数选择
enum cic0_order {
    CIC0_3ORDER,
    CIC0_4ORDER,
    CIC0_5ORDER,
};

//CIC1抽取滤波器阶数选择
enum cic1_order {
    CIC1_2ORDER,
    CIC1_3ORDER,
    CIC1_4ORDER,
};

//电源初始化顺序
//avddcp --> avddr --> avbg --> avcm

//HADC输出频率计算方法
//ivld_clk = (WORK_MODE == 0) ? 5 : 0;
//过采样率:OSR = (dsr0 * 4 + ivld_clk) * dsr1
//输出频率:Fo  = clock / OSR
//例如当clock=500K,dsr0=155,dsr1=16时, Fo = 500K / ((155*4 + 5) * 16) = 50Hz @ivld_clk=5
//例如当clock=600K,dsr0=155,dsr1=16时, Fo = 600K / ((155*4 + 5) * 16) = 60Hz @ivld_clk=5

//建议使用的CIC表如下：
//|----------------------|--------cic0---------|-------cic1----------|--------|----------|
//| WORK_MODE | CLK(kHz) | EN0 | DSR0 | ORDER0 | EN1 | DSR1 | ORDER1 |   OSR  |   Fo(Hz) |
//|     0     |   500    |  1  |  15  |   4    |  0  |  2   |   2    |    65  |    7692  |
//|     0     |   500    |  1  |  31  |   4    |  0  |  2   |   2    |   129  |    3875  |
//|     0     |   500    |  1  |  63  |   4    |  0  |  2   |   2    |   257  |    1945  |
//|     0     |   500    |  1  |  254 |   4    |  0  |  2   |   2    |  1021  |     489  |
//|     0     |   500    |  1  |  254 |   4    |  1  |  4   |   2    |  4084  |     122  |
//|     0     |   500    |  1  |  254 |   4    |  1  |  8   |   2    |  8168  |    61.2  |
//|     0     |   500    |  1  |  254 |   4    |  1  | 16   |   2    | 16336  |    30.6  |
//|     0     |   500    |  1  |  254 |   4    |  1  | 32   |   2    | 32672  |    15.3  |
//|     0     |   600    |  1  |  30  |   4    |  1  | 16   |   2    |  2000  |     300  |
//|     0     |   600    |  1  |  155 |   4    |  1  | 16   |   2    | 10000  |      60  |
//|     0     |   500    |  1  |  155 |   4    |  1  | 16   |   2    | 10000  |      50  |
//|--------------------------------------------------------------------------------------|

struct cic_data {
    u8 en;      //cic滤波器使能
    u8 dsr;     //cic滤波器抽取因子选择 dsr0(8~254) dsr1(2~64)
    u8 order;   //order越大,需要的稳定时间越久
    u8 gain;    //cic滤波器输出增益 64@0db (0~127)
};

//!!!注意:HADC的满幅值在±2^29左右!!!
struct hadc_platform_data {
    u32 clock;//工作频率,常设置为500K,最大为1M
    struct cic_data cic[2];//cic0 cic1
    void (*isr_cb)(int *adc_buffer, u32 adc_len);//adc_buffer为数据指针,adc_len为数据长度(points*4)
    u32 points;//点数,一次中断采集多少个点的ADC数据
};

struct hadc_soft_flag_info {
    u8 avddcp_short_en: 1;
    u8 avddcp_vol_sel: 3;
    u8 avddcp_delay_sel: 2;
    u8 fast_adc: 1;
    u8 avddcp_hadc_sel: 1;
    u8 avddcp_clktune_sel: 4;
    u8 avddcp_clkdiv_sel: 1;
    u8 avddr_vol_sel: 3;
    u8 avddr_ref_sel: 4;
};
extern struct hadc_soft_flag_info hadc_soft_info;

/*
 * @brief   初始化HADC模块
 * @data    需要配置的参数结构体
 * @note    初始化hadc,初始化后要开启hadc需要调用hadc_start
 */
extern void hadc_open(struct hadc_platform_data *data);

/*
 * @brief   关闭HADC模块
 * @note    关闭hadc,关闭后再使用时需要调用hadc_open
 */
extern void hadc_close(void);

/*
 * @brief   暂停HADC数字逻辑
 * @note    暂停hadc,暂停后再继续可以调用hadc_start
 */
extern void hadc_stop(void);

/*
 * @brief   启动HADC数字逻辑
 * @note    开始运行hadc,hadc_open后调用
 */
extern void hadc_start(void);

/*
 * @brief   开启hadc的debug信号
 * @note    测试使用
 */
extern void hadc_debug(void);

/*
 * @brief   切换hadc的信号输出频率
 * @cic0    cic0滤波器设置参数
 * @cic1    cic1滤波器设置参数
 * @note    使用过程中切换信号输出频率时调用,一般流程如下:
            hadc_stop();
            hadc_set_out_freq(cic0, cic1);
            hadc_start();
 */
extern void hadc_set_out_freq(struct cic_data *cic0, struct cic_data *cic1);

/*
 * @brief   快速启动时切换关闭avbg快速上电模式
 */
void hadc_avbg_init_for_fast_boot(void);

/*
 * @brief   快速启动时切换hadc avddcp电源时钟
 * @note    软关机快速上电时调用
 */
void hadc_avddcp_init_for_fast_boot(void);

/*
 * @brief   初始化avddcp电源
 * @en      1:开启 0:关闭
 * @mode    模式有VPWR直通,VDDIOX2,和正常升压
 * @avddcp_vol avddcp电压档位选择,mode = AVDDCP_MODE_SEL_NORMAL 时有效
 * @note    avddcp电源最先初始化
 * @note    切换模式时,应先关闭再切
 */
extern void hadc_avddcp_config(u8 en, enum avddcp_mode_sel mode, enum avddcp_vol_sel avddcp_vol);

/*
 * @brief   初始化avddr电源
 * @en      1:开启 0:关闭
 * @avddr_vol avddr电压档位选择
 * @note    在avddcp电源初始化之后调用
 */
extern void hadc_avddr_config(u8 en, enum avddr_vol_sel avddr_vol);

/*
 * @brief   初始化avbg电源
 * @en      1:开启 0:关闭
 * @faston_en 1:开启快速上电 0:关闭快速上电
 * @with_cap 1:有外挂电容, 0:无外挂电容
 * @note    在avddr初始化之后调用,正常上电后稳定时间需要800ms
 */
extern void hadc_avbg_config(u8 en, u8 faston_en, u8 with_cap);

/*
 * @brief   初始化avcm电源
 * @en      en: 1:开启 0:关闭
 * @with_cap 1:有外挂电容, 0:无外挂电容
 * @note    在avbg初始化之后调用
 */
extern void hadc_avcm_config(u8 en, u8 with_cap);

/*
 * @brief   初始化内部温度传感器
 * @en      1:开启 0:关闭
 * @note    需要采集内部温度传感器通道时开启
 */
extern void hadc_temp_sensor_config(u8 en);

/*
 * @brief   初始化PGA寄存器
 * @en      1:开启 0:关闭
 * @muxp    PGA P端输入通道选择
 * @muxn    PGA N端输入通道选择
 * @gain    PGA 放大倍数选择
 * @note    切换通道时,应先关闭再切通道
 */
extern void hadc_pga_config(u8 en, enum hadc_muxp_sel muxp, enum hadc_muxn_sel muxn, enum hadc_gain_sel gain);

/*
 * @brief   初始化OPA寄存器
 * @en      en: 1:开启 0:关闭
 * @cmp_en  比较器模式使能
 * @muxp    OPA P端输入通道选择
 * @muxn    OPA N端输入通道选择
 * @muxi    OPA N端体脂信号输入通道选择
 * @gain    OPA O端输出通道选择
 * @note    切换通道时,应先关闭再切通道
 */
extern void hadc_opa_config(u8 en, u8 cmp_en, enum hadc_opa_muxp_sel muxp, enum hadc_opa_muxn_sel muxn, enum hadc_opa_muxi_sel muxi, enum hadc_opa_muxo_sel muxo);

/*
 * @brief   初始化PA寄存器
 * @en      1:开启 0:关闭
 * @cmp_en  比较器模式使能
 * @muxp    PA P端输入通道选择
 * @muxn    PA N端输入通道选择
 * @gain    PA 增益选择
 * @note    切换通道时,应先关闭再切通道
 *          做比较器时,gain无效
 */
extern void hadc_pa_config(u8 en, u8 cmp_en, enum hadc_pa_muxp_sel muxp, enum hadc_pa_muxn_sel muxn, enum hadc_pa_gain_sel gain);

/*
 * @brief   初始化BIM信号通道选择
 * @en      1:开启 0:关闭
 * @chl     BIM模块通道选择
 * @note    切换通道时,应先关闭再切通道
 */
extern void hadc_vbim_config(u8 en, enum hadc_vbim_chl_sel chl);

/*
 * @brief   关闭HADC所有模拟信号
 * @note    需要省功耗时调用
 */
extern void hadc_analog_all_off(void);

#endif


