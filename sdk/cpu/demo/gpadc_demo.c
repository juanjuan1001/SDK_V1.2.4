#include "gpadc_api.h"
#include "asm/clock.h"
#include "gpio.h"
#include "system/timer.h"

void gpadc_cpu_mode_demo(void)
{
    printf("\n@_0 gpadc cpu mode demo test start\n");

    printf("@_0 cpu add ch PA5\n");
    adc_add_sample_ch(AD_CH_PA5);
    printf("@_0 cpu add ch VPWR\n");
    adc_add_sample_ch(AD_ANA_PMU_CH_VPWR_DIV_4);

    mdelay(10);

    printf("@_0 PA5 adc res: %d\n", adc_get_value(AD_CH_PA5));
    printf("@_0 PA5 voltage: %dmV\n", adc_get_voltage(AD_CH_PA5));

    printf("@_0 VPWR adc res: %d\n", adc_get_value(AD_ANA_PMU_CH_VPWR_DIV_4));
    printf("@_0 VPWR voltage: %dmV\n", adc_get_voltage(AD_ANA_PMU_CH_VPWR_DIV_4) * 4);

    printf("@_0 cpu remove ch PA5\n");
    adc_remove_sample_ch(AD_CH_PA5);
    printf("@_0 cpu remove ch VPWR\n");
    adc_remove_sample_ch(AD_ANA_PMU_CH_VPWR_DIV_4);
}

void gpadc_dma_mode_demo(void)
{
    printf("\n@_1 gpadc cpu mode demo test start\n");

    printf("@_1 dma add ch PA5\n");
    adc_dma_add_sample_ch(AD_CH_PA5);
    printf("@_1 dma add ch DIFF SP_PA2_SN_PA3\n");
    adc_dma_add_sample_ch(AD_DIFF_CH_SP_PA2_SN_PA3);

    mdelay(10);

    printf("@_1 PA5 adc res: %d\n", adc_get_value(AD_CH_PA5));
    printf("@_1 PA5 voltage: %dmV\n", adc_get_voltage(AD_CH_PA5));

    printf("@_1 DIFF SP_PA2_SN_PA3 adc res: %d\n", adc_get_value(AD_DIFF_CH_SP_PA2_SN_PA3));
    printf("@_1 DIFF SP_PA2_SN_PA3 voltage: %dmV\n", adc_get_voltage(AD_DIFF_CH_SP_PA2_SN_PA3));

    printf("@_1 dma remove ch PA5\n");
    adc_dma_remove_sample_ch(AD_CH_PA5);
    printf("@_1 dma remove ch DIFF SP_PA2_SN_PA3\n");
    adc_dma_remove_sample_ch(AD_DIFF_CH_SP_PA2_SN_PA3);
}

void gpadc_blocking_mode_demo(void)
{
    printf("\n@_2 gpadc blocking mode demo test start\n");
    printf("@_2 PA8 adc res: %d\n", adc_get_value_by_blocking(AD_CH_PA8, 8));
    printf("@_2 PA8 voltage: %dmV\n", adc_get_voltage_by_blocking(AD_CH_PA8, 8));
}

void gpadc_dma_conti_mode_callback(u16 *buf, u32 len)
{
    /* printf("len: %d\n", len); */
}

void gpadc_dma_conti_mode_demo(void)
{
    printf("\n@_3 gpadc blocking mode demo test start\n");
    printf("@_3 enter sample DIFF SP_PA2_SN_PA3\n");
    adc_dma_enter_single_ch_sample(AD_DIFF_CH_SP_PA2_SN_PA3, gpadc_dma_conti_mode_callback);
    mdelay(1);
    printf("@_3 exit sample DIFF SP_PA2_SN_PA3\n");
    adc_dma_exit_single_ch_sample(AD_DIFF_CH_SP_PA2_SN_PA3);
}

//gpadc测试demo
void gpadc_test_demo(void)
{
    printf("\n***** GPADC module test begin *****\n");

    adc_init();

    //cpu模式测试
    gpadc_cpu_mode_demo();

    //dma模式测试
    gpadc_dma_mode_demo();

    //打断模式测试
    gpadc_blocking_mode_demo();

    //dma单通道测试
    gpadc_dma_conti_mode_demo();

    printf("\n***** GPADC module test end *****\n");
}

