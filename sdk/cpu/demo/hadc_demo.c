#include "asm/hadc.h"

static void hadc_isr_callback(int *buffer, u32 buffer_len)
{
    for (u32 i = 0; i < buffer_len; i++) {
        printf("hdac res : %d\n", buffer[i]);
    }
}

static const struct hadc_platform_data hadc_test_data = {
    .clock = 500000L,
    .cic[0].en = 1,
    .cic[0].dsr = 155,
    .cic[0].order = CIC0_4ORDER,
    .cic[1].en = 1,
    .cic[1].dsr = 16,
    .cic[1].order = CIC1_2ORDER,
    .isr_cb = hadc_isr_callback,
    .points = 1,
};

void hadc_demo_test_start(void)
{
    hadc_avddcp_config(1, AVDDCP_MODE_SEL_NORMAL, AVDDCP_VOL_SEL_3P60);
    hadc_avddr_config(1, AVDDR_VOL_SEL_3P0);
    hadc_avbg_config(1, 1, 1);
    hadc_avcm_config(1, 1);
    hadc_pga_config(1, HADC_MUXP_SEL_AIN0, HADC_MUXN_SEL_AIN1, HADC_GAIN_SEL_X1);
    hadc_open((struct hadc_platform_data *)(&hadc_test_data));
    hadc_start();
}

void hadc_demo_test_stop(void)
{
    hadc_avddcp_config(0, AVDDCP_MODE_SEL_NORMAL, AVDDCP_VOL_SEL_3P60);
    hadc_avddr_config(0, AVDDR_VOL_SEL_3P0);
    hadc_avbg_config(0, 0, 0);
    hadc_avcm_config(0, 0);
    hadc_pga_config(0, HADC_MUXP_SEL_AIN0, HADC_MUXN_SEL_AIN1, HADC_GAIN_SEL_X1);
    hadc_close();
}

