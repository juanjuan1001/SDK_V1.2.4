#include "system/includes.h"
#include "asm/efuse.h"
#include "asm/hadc.h"
#include "gpadc_api.h"
#include "syscfg_id.h"

#define LOG_TAG_CONST       HADC
#define LOG_TAG             "[HADC]"
#include "debug.h"

struct hadc_soft_flag_info hadc_soft_info;

/*
 * @brief   快速启动时切换关闭avbg快速上电模式
 */
void hadc_avbg_init_for_fast_boot(void)
{
    if (efuse_get_sfc_fast_boot() && (JL_ADDA->HADA_CON0 & BIT(23))) {
        SFR(JL_ADDA->HADA_CON0, 22,  1,  1);//HADC_IBIAS_EN
        SFR(JL_ADDA->HADA_CON0, 24,  1,  0);//HADC_VBG_FASTON_EN
    }
}

/*
 * @brief   快速启动时切换hadc avddcp电源时钟
 * @note    软关机快速上电时调用
 */
void hadc_avddcp_init_for_fast_boot(void)
{
    SFR(JL_CLOCK->PRP_CON1,  0,  1,  1);
    SFR(JL_CLOCK->PRP_CON1,  1,  7, 47);
    SFR(JL_MODE->MODE_CON,   0,  1,  0);

    SFR(JL_ADDA->AVDD_CON0,  7,  1,  0);//AVDDR_PUMPCLKREF_SLT 0:pll clk 1:rc clk
    SFR(JL_ADDA->AVDD_CON0, 10,  1,  1);//AVDDCP_PLLCLKREF_EN_11V
    SFR(JL_ADDA->AVDD_CON0, 16,  2,  1);//AVDDR_PUMP_AMP_IBIAS_S 0:1
    SFR(JL_ADDA->AVDD_CON0, 18,  2,  1);//AVDDR_PUMP_NONLAP_S 0:1
}

/*
 * @brief   初始化avddcp电源
 * @en      en: 1:开启 0:关闭
 * @mode    模式有VPWR直通,VDDIOX2,和正常升压
 * @avddcp_vol avddcp电压档位选择
 * @note    avddcp电源最先初始化
 * @note    切换模式时,应先关闭再切
 */
void hadc_avddcp_config(u8 en, enum avddcp_mode_sel mode, enum avddcp_vol_sel avddcp_vol)
{
    if (en) {
        if (mode == AVDDCP_MODE_SEL_NORMAL) {
            SFR(JL_CLOCK->PRP_CON1,  0,  1,  1);
            SFR(JL_CLOCK->PRP_CON1,  1,  7,  47);
            SFR(JL_MODE->MODE_CON,   0,  1,  0);

            SFR(JL_ADDA->AVDD_CON0, 18,  2,  1);//AVDDR_PUMP_NONLAP_S 0:1
            SFR(JL_ADDA->AVDD_CON0,  7,  1,  0);//AVDDR_PUMPCLKREF_SLT 0:pll clk 1:rc clk
            SFR(JL_ADDA->AVDD_CON0, 16,  2,  1);//AVDDR_PUMP_AMP_IBIAS_S 0:1
            SFR(JL_ADDA->AVDD_CON1,  0,  4,  0b1000);//AVDDCP_CLKTUNE_S3-0
            SFR(JL_ADDA->AVDD_CON1,  4,  3,  0b110);//AVDDCP_CLK_S2-0
            SFR(JL_ADDA->AVDD_CON0, 20,  3,  avddcp_vol);//AVDDCP_VOL_S2-0
            delay_us(100);
            p33_or_1byte(P3_CHG_PUMP, BIT(0));//AVDDCP_VBG_EN
            JL_ADDA->AVDD_CON0 |= BIT(1) | BIT(6);//AVDDCP_SOFT_STARTUP_EN; //AVDDCP_PUMP_AMP_EN;
            JL_ADDA->AVDD_CON0 |= BIT(8) | BIT(9) | BIT(10);//AVDDCP_CLKDIV_EN //AVDDCP_RCCLK_EN //AVDDCP_STD48M_EN;
            delay_us(100);
            JL_ADDA->AVDD_CON0 |= BIT(0) | BIT(2);  //AVDDCP_PUMP_EN; //AVDDCP_PUMPST_RN
            delay_us(1500);
            SFR(JL_ADDA->AVDD_CON0,  3,  1,  1);    //AVDDCP_REGULATOR_MODE
            delay_us(500);
            SFR(JL_ADDA->AVDD_CON0,  1,  1,  0);    //AVDDCP_SOFT_STARTUP_EN
            delay_us(100);
        } else {
            p33_or_1byte(P3_CHG_PUMP, BIT(0));  //AVDDCP_VBG_EN
            SFR(JL_ADDA->AVDD_CON0,  1,  1,  1);//AVDDCP_SOFT_STARTUP_EN
            SFR(JL_ADDA->AVDD_CON0,  3,  1,  1);//AVDDCP_REGULATOR_MODE
            SFR(JL_ADDA->AVDD_CON0,  6,  1,  1);//AVDDCP_PUMP_AMP_EN
            delay_us(300);
            p33_or_1byte(P3_CHG_PUMP, BIT(2));  //VPWR_AVDDCP_SHORT_EN
            SFR(JL_ADDA->AVDD_CON0,  5,  1,  1);//AVDDCP_PUMPX2_SET
            SFR(JL_ADDA->AVDD_CON0,  1,  1,  0);//AVDDCP_SOFT_STARTUP_EN
            SFR(JL_ADDA->AVDD_CON0,  3,  1,  0);//AVDDCP_REGULATOR_MODE
            SFR(JL_ADDA->AVDD_CON0,  6,  1,  0);//AVDDCP_PUMP_AMP_EN
            p33_and_1byte(P3_CHG_PUMP, (u8)(~BIT(0)));//AVDDCP_VBG_EN
            delay_us(1000);//for 2.2uF
            p33_or_1byte(P3_CHG_PUMP, BIT(3));  //VPWR_AVDDCP_STRONG_SHORT_EN
            delay_us(100);
        }
    } else {
        /* p33_and_1byte(P3_CHG_PUMP, (u8)(~BIT(0)));//AVDDCP_VBG_EN */
        /* p33_and_1byte(P3_CHG_PUMP, (u8)(~BIT(2)));//VPWR_AVDDCP_SHORT_EN */
        /* p33_and_1byte(P3_CHG_PUMP, (u8)(~BIT(3)));//VPWR_AVDDCP_STRONG_SHORT_EN */
        p33_and_1byte(P3_CHG_PUMP, (u8)(~(BIT(0) | BIT(2) | BIT(3))));
        JL_ADDA->AVDD_CON0 &= ~(BIT(1) | BIT(6) | BIT(9) | BIT(10));
        JL_ADDA->AVDD_CON0 &= ~BIT(8) ; //AVDDCP_CLKDIV_EN
        JL_ADDA->AVDD_CON0 &= ~(BIT(0) | BIT(2)); //AVDDCP_PUMP_EN; //AVDDCP_PUMPST_RN
        SFR(JL_ADDA->AVDD_CON0,  3,  1,  0);//AVDDCP_REGULATOR_MODE
        SFR(JL_ADDA->AVDD_CON0,  7,  1,  0);//AVDDR_PUMPCLKREF_SLT
        SFR(JL_ADDA->AVDD_CON0,  5,  1,  0);//AVDDCP_PUMPX2_SET
        SFR(JL_CLOCK->PRP_CON1,  0,  1,  0);
    }
}

/*
 * @brief   avddr电源校准
 * @avddr_vol avddr电压档位选择
 * @note    未校准时调用
 */
static void hadc_avddr_trim(enum avddr_vol_sel avddr_vol)
{
    u8 avddr_ref;
    u16 vbg_value, avddr_value, min_offset, avddr_voltage, offset_voltage;
    min_offset = 0xffff;
    vbg_value = adc_get_value_by_blocking(AD_ANA_PMU_CH_VBG, 8);
    SFR(JL_ADDA->AVDD_CON0, 15,  1,  1);
    SFR(JL_ADDA->AVDD_CON0, 29,  3,  AVDDR_VOL_SEL_3P0);//AVDDR_VOL_S2-0
    for (u32 i = 0; i < 16; i++) {
        SFR(JL_ADDA->AVDD_CON0, 25,  4,  i);//AVDDR_REF_S3-0
        mdelay(5);
        avddr_value = adc_get_value_by_blocking(AD_ANA_PMU_CH_AVDDR_DIV_4, 8) * 4;
        avddr_voltage = adc_value_to_voltage(vbg_value, avddr_value);
        offset_voltage = (avddr_voltage > 3000) ? (avddr_voltage - 3000) : (3000 - avddr_voltage);
        if (offset_voltage < min_offset) {
            min_offset = offset_voltage;
            avddr_ref = i;
        }
    }
    SFR(JL_ADDA->AVDD_CON0, 15,  1,  0);
    SFR(JL_ADDA->AVDD_CON0, 29,  3,  avddr_vol);//AVDDR_VOL_S2-0
    if (min_offset > 100) {
        SFR(JL_ADDA->AVDD_CON0, 25,  4,  0b0111);//AVDDR_REF_S3-0
        log_info("avddr trim err! min_offset: %d mV", min_offset);
    } else {
        SFR(JL_ADDA->AVDD_CON0, 25,  4,  avddr_ref);//AVDDR_REF_S3-0
        syscfg_write(VM_AVDDR_REF, (void *)&avddr_ref, 1);
        log_info("avddr trim succ! avddr_ref: %d", avddr_ref);
    }
}

/*
 * @brief   初始化avddr电源
 * @en      en: 1:开启 0:关闭
 * @avddr_vol avddr电压档位选择
 * @note    在avddcp电源初始化之后调用
 */
#define AVDDR_RISE_TIME_US  500 //AVDDR上升时间,外挂电容大于105时需调节
void hadc_avddr_config(u8 en, enum avddr_vol_sel avddr_vol)
{
    int ret;
    u8 avddr_ref;
    if (en) {
        avddr_ref = 0b0111;
        ret = syscfg_read(VM_AVDDR_REF, (void *)&avddr_ref, 1);
        log_info("VM_AVDDR_REF : %d, ref: %d\n", ret, avddr_ref);
        SFR(JL_ADDA->AVDD_CON0, 23,  2,  1);//AVDDR_IBIAS_S1-0
        if (ret == 1) {
            SFR(JL_ADDA->AVDD_CON0, 25,  4,  avddr_ref);//AVDDR_REF_S3-0
            SFR(JL_ADDA->AVDD_CON0, 29,  3,  avddr_vol);//AVDDR_VOL_S2-0
        } else {
            SFR(JL_ADDA->AVDD_CON0, 25,  4,  0);//AVDDR_REF_S3-0
            SFR(JL_ADDA->AVDD_CON0, 29,  3,  AVDDR_VOL_SEL_3P0);//AVDDR_VOL_S2-0
        }
        JL_ADDA->AVDD_CON0 |= BIT(12) | BIT(13);//AVDDR_EN //AVDDR_VLMTEN
        delay_us(AVDDR_RISE_TIME_US);
        SFR(JL_ADDA->AVDD_CON0, 14,  1,  1);//AVDDR_IFULLE
        delay_us(100);
        if (ret != 1) {
            hadc_avddr_trim(avddr_vol);
        }
    } else {
        JL_ADDA->AVDD_CON0 &= ~(BIT(12) | BIT(13));//AVDDR_EN //AVDDR_VLMTEN
        JL_ADDA->AVDD_CON0 &= ~(BIT(14));//AVDDR_IFULLE
    }
}

/*
 * @brief   初始化avbg电源
 * @en      1:开启 0:关闭
 * @faston_en 1:开启快速上电 0:关闭快速上电
 * @with_cap 1:有外挂电容, 0:无外挂电容
 * @note    在avddr初始化之后调用
 */
void hadc_avbg_config(u8 en, u8 faston_en, u8 with_cap)
{
    if (en) {
        SFR(JL_ADDA->HADA_CON1,  0,  4,  efuse_get_hadc_vbg_trim());//HADC_VBG_VSEL
        SFR(JL_ADDA->HADA_CON0, 22,  1,  1);//HADC_IBIAS_EN
        SFR(JL_ADDA->HADA_CON0,  2,  1,  1);//HADC_CHOP_BG_CLK_EN
        SFR(JL_ADDA->HADA_CON0, 21,  1,  with_cap);//HADC_VBG_WITHCAP_EN
        SFR(JL_ADDA->HADA_CON0, 24,  1,  faston_en);//HADC_VBG_FASTON_EN
        SFR(JL_ADDA->HADA_CON0, 23,  1,  1);//HADC_VBG_EN
        SFR(JL_ADDA->HADA_CON2,  8,  1,  1);//HADC_VBGBUF_EN
        SFR(JL_ADDA->HADA_CON2,  9,  1,  0);//HADC_VBGBUF_ISEL
        if (faston_en) {
            delay_us(1500);
            SFR(JL_ADDA->HADA_CON0, 24,  1,  0);//HADC_VBG_FASTON_EN
        }
    } else {
        SFR(JL_ADDA->HADA_CON0, 23,  1,  0);//HADC_VBG_EN
        SFR(JL_ADDA->HADA_CON0, 22,  1,  0);//HADC_IBIAS_EN
        SFR(JL_ADDA->HADA_CON0,  2,  1,  0);//HADC_CHOP_BG_CLK_EN
        SFR(JL_ADDA->HADA_CON0, 21,  1,  0);//HADC_VBG_WITHCAP_EN
        SFR(JL_ADDA->HADA_CON0, 24,  1,  0);//HADC_VBG_FASTON_EN
        SFR(JL_ADDA->HADA_CON2,  8,  1,  0);//HADC_VBGBUF_EN
    }
}

/*
 * @brief   初始化avcm电源
 * @en      en: 1:开启 0:关闭
 * @with_cap 1:有外挂电容, 0:无外挂电容
 * @note    在avbg初始化之后调用
 */
void hadc_avcm_config(u8 en, u8 with_cap)
{
    if (en) {
        SFR(JL_ADDA->HADA_CON0,  6,   1,  1);//HADC_CHOP_VREFBUF_CLK_EN
        SFR(JL_ADDA->HADA_CON0,  7,   1,  1);//HADC_CHOP_VREFGEN_CLK_EN
        SFR(JL_ADDA->HADA_CON0, 20,   1,  with_cap);//HADC_VCM_CAP_EN
        SFR(JL_ADDA->HADA_CON0, 25,   1,  1);//HADC_VREF_EN
        SFR(JL_ADDA->HADA_CON0, 30,   1,  0);//HADC_VREF_BW_CTRL
        SFR(JL_ADDA->HADA_CON0, 31,   1,  0);//HADC_VREF_IB_CTRL
        SFR(JL_ADDA->HADA_CON1,  4,   2,  3);//HADC_VREF_CTRL,参考电压 0:0.5V 1:1V 2:1.5V 3:2V
    } else {
        SFR(JL_ADDA->HADA_CON0, 25,   1,  0);//HADC_VREF_EN
        SFR(JL_ADDA->HADA_CON0,  6,   1,  0);//HADC_CHOP_VREFBUF_CLK_EN
        SFR(JL_ADDA->HADA_CON0,  7,   1,  0);//HADC_CHOP_VREFGEN_CLK_EN
        SFR(JL_ADDA->HADA_CON0, 20,   1,  0);//HADC_VCM_CAP_EN
        SFR(JL_ADDA->HADA_CON0, 30,   1,  0);//HADC_VREF_BW_CTRL
        SFR(JL_ADDA->HADA_CON0, 31,   1,  0);//HADC_VREF_IB_CTRL
        SFR(JL_ADDA->HADA_CON1,  4,   2,  0);//HADC_VREF_CTRL
    }
}

/*
 * @brief   初始化内部温度传感器
 * @en      en: 1:开启 0:关闭
 * @note    需要采集内部温度传感器通道时开启
 */
void hadc_temp_sensor_config(u8 en)
{
    SFR(JL_ADDA->HADA_CON0, 29,  1,  en);// HADC_TEMP_SENSOR_EN
}

/*
 * @brief   初始化PGA寄存器
 * @en      en: 1:开启 0:关闭
 * @muxp    PGA P端输入通道选择
 * @muxn    PGA N端输入通道选择
 * @gain    PGA 放大倍数选择
 * @note    切换通道时,应先关闭再切通道
 */
void hadc_pga_config(u8 en, enum hadc_muxp_sel muxp, enum hadc_muxn_sel muxn, enum hadc_gain_sel gain)
{
    SFR(JL_ADDA->HADA_CON0,  0,  1,  0);    //HADC_PGA_EN
    SFR(JL_ADDA->HADA_CON0,  1,  1,  0);    //HADC_CHANNEL_EN
    SFR(JL_ADDA->HADA_CON0,  3,  1,  0);    //HADC_CHOP_PGA1_CLK_EN
    SFR(JL_ADDA->HADA_CON0,  4,  1,  0);    //HADC_CHOP_PGA2_CLK_EN
    SFR(JL_ADDA->HADA_CON0,  5,  1,  0);    //HADC_CHOP_SDM_CLK_EN
    SFR(JL_ADDA->HADA_CON0, 27,  1,  0);    //HADC_SDM_EN
    SFR(JL_ADDA->HADA_CON0, 28,  1,  0);    //HADC_SDM_GAIN
    if (en) {
        SFR(JL_ADDA->HADA_CON0,  0,  1,  1);    //HADC_PGA_EN
        SFR(JL_ADDA->HADA_CON0,  1,  1,  1);    //HADC_CHANNEL_EN
        SFR(JL_ADDA->HADA_CON0,  3,  1,  1);    //HADC_CHOP_PGA1_CLK_EN
        SFR(JL_ADDA->HADA_CON0,  4,  1,  1);    //HADC_CHOP_PGA2_CLK_EN
        SFR(JL_ADDA->HADA_CON0,  5,  1,  1);    //HADC_CHOP_SDM_CLK_EN
        SFR(JL_ADDA->HADA_CON0,  8,  4,  muxn); //HADC_MUXN_SEL[3:0]
        SFR(JL_ADDA->HADA_CON0, 12,  4,  muxp); //HADC_MUXP_SEL[3:0]
        SFR(JL_ADDA->HADA_CON0, 16,  4,  gain); //HADC_PGA_GAIN[3:0]
        SFR(JL_ADDA->HADA_CON0, 26,  1,  0);    //HADC_SDM_MODE
        SFR(JL_ADDA->HADA_CON0, 27,  1,  1);    //HADC_SDM_EN -- ADC_EN
        SFR(JL_ADDA->HADA_CON0, 28,  1,  0);    //HADC_SDM_GAIN -- ADC_GAIN
        SFR(JL_ADDA->HADA_CON1, 24,  4,  0);    //HADC_PGA_OSTRIM1[3:0]
        SFR(JL_ADDA->HADA_CON1, 28,  4,  0);    //HADC_PGA_OSTRIM2[3:0]
        SFR(JL_ADDA->HADA_CON2,  0,  1,  0);    //HADC_OSTRIM_P1_EN
        SFR(JL_ADDA->HADA_CON2,  1,  1,  0);    //HADC_OSTRIM_P2_EN
    }
}

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
void hadc_opa_config(u8 en, u8 cmp_en, enum hadc_opa_muxp_sel muxp, enum hadc_opa_muxn_sel muxn, enum hadc_opa_muxi_sel muxi, enum hadc_opa_muxo_sel muxo)
{
    SFR(JL_ADDA->HADA_CON1,  6,  1,  0);        //HADC_OPA_EN
    SFR(JL_ADDA->HADA_CON1,  7,  3,  0);        //HADC_OPA_ISEL[2:0]
    SFR(JL_ADDA->HADA_CON1, 10,  1,  0);        //HADC_OPA_MUXI_EN
    SFR(JL_ADDA->HADA_CON1, 11,  2,  0);        //HADC_OPA_MUXI_SEL[1:0]
    SFR(JL_ADDA->HADA_CON1, 13,  1,  0);        //HADC_OPA_MUXN_EN
    SFR(JL_ADDA->HADA_CON1, 14,  3,  0);        //HADC_OPA_MUXN_SEL[2:0]
    SFR(JL_ADDA->HADA_CON1, 21,  1,  0);        //HADC_OPA_MUXP_EN
    SFR(JL_ADDA->HADA_CON1, 22,  2,  0);        //HADC_OPA_MUXP_SEL[1:0]
    SFR(JL_ADDA->HADA_CON1, 17,  1,  0);        //HADC_OPA_MUXO_EN
    SFR(JL_ADDA->HADA_CON1, 18,  3,  0);        //HADC_OPA_MUXO_SEL[2:0]
    SFR(JL_ADDA->HADA_CON2,  2,  1,  0);        //HADC_OPA_CMP_EN
    SFR(JL_ADDA->DAA_CON0,  12,  1,  0);        //SDDAC_DACBIAS_2_OPA_EN
    if (en) {
        SFR(JL_ADDA->HADA_CON1,  7,  3,  4);        //HADC_OPA_ISEL[2:0]
        if (muxn != HADC_OPA_MUXN_SEL_DISABLE) {
            SFR(JL_ADDA->HADA_CON1, 13,  1,  1);    //HADC_OPA_MUXN_EN
            SFR(JL_ADDA->HADA_CON1, 14,  3,  muxn); //HADC_OPA_MUXN_SEL[2:0]
        }
        if (muxi != HADC_OPA_MUXI_SEL_DISABLE) {
            SFR(JL_ADDA->HADA_CON1, 10,  1,  1);    //HADC_OPA_MUXI_EN
            SFR(JL_ADDA->HADA_CON1, 11,  2,  muxi); //HADC_OPA_MUXI_SEL[1:0]
        }
        if (muxp != HADC_OPA_MUXP_SEL_DISABLE) {
            if (muxp == HADC_OPA_MUXP_SEL_DACBIAS) {
                SFR(JL_ADDA->DAA_CON0, 14,  1,  1); //SDDAC_DACBIAS_EN
                SFR(JL_ADDA->DAA_CON0, 12,  1,  1); //SDDAC_DACBIAS_2_OPA_EN SDDAC的直流电压输出给HADC OPA正端的使能开关
            }
            SFR(JL_ADDA->HADA_CON1, 21,  1,  1);    //HADC_OPA_MUXP_EN
            SFR(JL_ADDA->HADA_CON1, 22,  2,  muxp); //HADC_OPA_MUXP_SEL[1:0]
        }
        if (muxp != HADC_OPA_MUXO_SEL_DISABLE) {
            SFR(JL_ADDA->HADA_CON1, 17,  1,  1);    //HADC_OPA_MUXO_EN
            SFR(JL_ADDA->HADA_CON1, 18,  3,  muxo); //HADC_OPA_MUXO_SEL[2:0]
        }
        SFR(JL_ADDA->HADA_CON2,  2,  1,  cmp_en);   //HADC_OPA_CMP_EN
        SFR(JL_ADDA->HADA_CON1,  6,  1,  1);        //HADC_OPA_EN
    }
}

/*
 * @brief   初始化PA寄存器
 * @en      en: 1:开启 0:关闭
 * @cmp_en  比较器模式使能
 * @muxp    OPA P端输入通道选择
 * @muxn    OPA N端输入通道选择
 * @note    切换通道时,应先关闭再切通道
 */
void hadc_pa_config(u8 en, u8 cmp_en, enum hadc_pa_muxp_sel muxp, enum hadc_pa_muxn_sel muxn, enum hadc_pa_gain_sel gain)
{
    SFR(JL_ADDA->DAA_CON0, 13,  1,  0);//SDDAC_DACBIAS_2_PA_EN
    SFR(JL_ADDA->DAA_CON0, 15,  1,  0);//SDDAC_DACVCM_EN
    SFR(JL_ADDA->DAA_CON0, 16,  1,  0);//SDDAC_LPF_EN
    SFR(JL_ADDA->DAA_CON0, 17,  1,  0);//SDDAC_LPF_GAIN_EN
    SFR(JL_ADDA->DAA_CON0, 22,  1,  0);//SDDAC_PA_CMP_EN
    SFR(JL_ADDA->DAA_CON0, 23,  1,  0);//SDDAC_PA_EN
    SFR(JL_ADDA->DAA_CON0, 27,  1,  0);//SDDAC_PA_MUXN_EN
    SFR(JL_ADDA->DAA_CON0, 28,  1,  0);//SDDAC_PA_MUXP_EN
    if (en) {
        SFR(JL_ADDA->DAA_CON0,  2,  1,  1);//SDDAC_DAC_IBIAS_EN
        SFR(JL_ADDA->DAA_CON0, 24,  3,  4);//SDDAC_PA_ISEL PA偏置电流选择位
        SFR(JL_ADDA->DAA_CON0, 22,  1,  cmp_en);//SDDAC_PA_CMP_EN PA运放作比较器使用时的使能开关
        switch (muxp) {
        case HADC_PA_MUXP_SEL_AVBG:
            SFR(JL_ADDA->DAA_CON0, 15,  1,  1);//SDDAC_DACVCM_EN
            break;
        case HADC_PA_MUXP_SEL_AIN2:
            SFR(JL_ADDA->DAA_CON0, 28,  1,  1);//SDDAC_PA_MUXP_EN
            break;
        case HADC_PA_MUXP_SEL_DACO_BIAS:
            SFR(JL_ADDA->DAA_CON0, 14,  1,  1);//SDDAC_DACBIAS_EN
            SFR(JL_ADDA->DAA_CON0, 13,  1,  1);//SDDAC_DACBIAS_2_PA_EN
            break;
        }
        switch (muxn) {
        case HADC_PA_MUXN_AIN4:
            SFR(JL_ADDA->DAA_CON0, 16,  1,  0);//SDDAC_LPF_EN
            SFR(JL_ADDA->DAA_CON0, 27,  1,  1);//SDDAC_PA_MUXN_EN
            break;
        case HADC_PA_MUXN_DACO_BIAS:
            SFR(JL_ADDA->DAA_CON0, 27,  1,  0);//SDDAC_PA_MUXN_EN
            SFR(JL_ADDA->DAA_CON0, 16,  1,  1);//SDDAC_LPF_EN
            break;
        case HADC_PA_MUXN_SEL_DISABLE:
            SFR(JL_ADDA->DAA_CON0, 27,  1,  0);//SDDAC_PA_MUXN_EN
            SFR(JL_ADDA->DAA_CON0, 16,  1,  0);//SDDAC_LPF_EN
            break;
        }
        if ((cmp_en == 0) && (gain < HADC_PA_GAIN_SEL_DISABLE)) {
            SFR(JL_ADDA->DAA_CON0, 18,  3,  gain);//SDDAC_LPF_GAIN_SEL
            SFR(JL_ADDA->DAA_CON0, 17,  1,  1);//SDDAC_LPF_GAIN_EN
        }
        SFR(JL_ADDA->DAA_CON0, 23,  1,  1);
    }
}

/*
 * @brief   初始化BIM信号通道选择
 * @en      1:开启 0:关闭
 * @chl     BIM模块通道选择
 * @note    切换通道时,应先关闭再切通道
 */
void hadc_vbim_config(u8 en, enum hadc_vbim_chl_sel chl)
{
    SFR(JL_ADDA->HADA_CON2,  4,  2,  chl);  //HADC_OPA_VBIM_RES
    SFR(JL_ADDA->HADA_CON2,  3,  1,  en);    //HADC_OPA_VBIM_EN
}

/*
 * @brief   关闭HADC所有模拟信号
 * @note    需要省功耗时调用
 */
void hadc_analog_all_off(void)
{
    p33_tx_1byte(P3_CHG_PUMP, 0);
    JL_ADDA->DAA_CON0 = 0;
    JL_ADDA->HADA_CON0 = 0;
    JL_ADDA->HADA_CON1 = 0;
    JL_ADDA->HADA_CON2 = 0;
    JL_ADDA->AVDD_CON0 = 0;
    JL_ADDA->AVDD_CON1 = 0;
}

