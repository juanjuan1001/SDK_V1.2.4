#include "system/includes.h"
#include "asm/hadc.h"
#include <math.h>

#define LOG_TAG_CONST       HADC
#define LOG_TAG             "[HADC]"
#include "debug.h"

//时钟配置: std12m hsb_clk -> div 1/3/5/7 -> div 1/2/4/8 -> hadc_clk
#define HADC_CLK_STD12M     1
#define HADC_CLK_HSB_CLK    2
#define HADC_CLK_PAT_CLK    3

//DMA输出数据选择
#define DSM_OUT             1
#define CIC0_DOUT           2
#define CIC1_DOUT           3

#define MEAN_EN             1//cic0输出数据平均使能,常设置为1
#define WORK_MODE           0//0:CRCC 1:NCRCC; 常设为0
#define CIC0_GAIN           64
#define CIC1_GAIN           64

static const u8 hadc_clk_div0[4] = {1, 3, 5, 7};
static const u8 hadc_clk_div1[4] = {1, 2, 4, 8};

static const u8 cic0_dsr_order_to_shift[3][247] = {
    {
        18, 17, 17, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13,
        13, 13, 12, 12, 12, 12, 12, 12, 12, 11, 11, 11, 11, 11, 11, 11,
        11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  9,  9,  9,  9,  9,
        9,  9,  9,  9,  9,  9,  9,  9,  9,  8,  8,  8,  8,  8,  8,  8,
        8,  8,  8,  8,  8,  8,  8,  8,  8,  7,  7,  7,  7,  7,  7,  7,
        7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5,  5,
        5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
        5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,
        4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
        4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
        4,  4,  4,  4,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
        3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
        3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
        3,  3,  3,  3,  3,  3,  3,
    },
    {
        15, 14, 13, 13, 12, 12, 11, 11, 11, 10, 10, 10,  9,  9,  9,  8,
        8,  8,  8,  7,  7,  7,  7,  7,  7,  6,  6,  6,  6,  6,  6,  5,
        5,  5,  5,  5,  5,  5,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,
        3,  3,  3,  3,  3,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  2,
        2,  2,  2,  2,  2,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
        2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
        3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,
        4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
        4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
        5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
        5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
        5,  5,  5,  5,  5,  5,  5,
    },
    {
        12, 11, 10,  9,  9,  8,  7,  7,  7,  6,  6,  5,  5,  5,  4,  4,
        4,  3,  3,  3,  2,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0,
        0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
        3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  4,  4,
        4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,
        7,  7,  7,  7,  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,
        8,  8,  8,  8,  8,  8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,
        9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10,
        10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
        10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
        11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12,
        12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13, 13, 13, 13, 13,
        13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
        13, 13, 13, 13, 13, 13, 13,
    },
};

static const u8 cic0_dsr_order_to_shdir[3][247] = {
    {
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,
    },
    {
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,
    },
    {
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
        1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,
    },
};

static const u8 cic1_dsr_order_to_shift[3][63] = {
    {
        3,  1,  1,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  3,  3,  4,
        4,  4,  4,  4,  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,
        6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  7,  7,  7,  7,
        7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
    },
    {
        2,  0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  7,  7,  7,  7,  8,
        8,  8,  8,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 11,
        11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        12, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    },
    {
        1,  2,  3,  5,  6,  7,  7,  8,  9,  9, 10, 10, 11, 11, 11, 12,
        12, 12, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 16,
        16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18,
        18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
    },
};

static const u8 cic1_dsr_order_to_shdir[3][63] = {
    {
        1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    },
    {
        1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    },
    {
        1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    },
};

struct hadc_info_t {
    u8 hadc_init;
    u32 clock;
    int *dual_buffer;
    u32 buffer_len;
    void (*isr_cb)(int *adc_buffer, u32 points);
};

static int hadc_run_buffer[HADC_NORMAL_DMA_POINTS * 2] AT(.noinit);
static struct hadc_info_t hadc_info;
#define __this  (&hadc_info)

__attribute__((interrupt("")))
void hadc_isr(void)
{
    u8 buf_used;
    if ((JL_HADC->HADC_CON0 & BIT(7)) && (JL_HADC->HADC_CON0 & BIT(5))) {
        buf_used = (JL_HADC->HADC_CON0 & BIT(4)) ? 0 : 1;
        JL_HADC->HADC_CON0 |= BIT(6);//CPND
        if (__this->isr_cb) {
            __this->isr_cb((int *)((u32)__this->dual_buffer +  __this->buffer_len * buf_used), __this->buffer_len / sizeof(int));
        }
    }
}

static void hadc_clk_init(u32 clk_out)
{
    u8 div0, div1;
    u32 tmp_clk;
    for (div0 = 0; div0 < 4; div0++) {
        for (div1 = 0; div1 < 4; div1++) {
            tmp_clk = 12000000L / hadc_clk_div0[div0] / hadc_clk_div1[div1];
            if (tmp_clk == (clk_out * 4)) {
                goto __hadc_clk_set;
            }
        }
    }
    ASSERT(0, "hadc clock set err!\n");

__hadc_clk_set:
    SFR(JL_CLOCK->PRP_CON0, 22, 2, HADC_CLK_STD12M);
    SFR(JL_CLOCK->PRP_CON0, 24, 2, div0);
    SFR(JL_CLOCK->PRP_CON0, 26, 2, div1);
    log_info("HADC clk: %d, div0: %d, div1: %d", clk_out, div0, div1);
}

static void hadc_cic_init(struct cic_data *cic0, struct cic_data *cic1)
{
    u8 dma_data_sel;
    s8 cic_order, cic_iw_ceil, cic_delta, cic_shdir, cic_shift;
    float cic_log2, cic_iw;

    if (cic0->en && cic1->en) {
        dma_data_sel = CIC1_DOUT;
    } else if (cic0->en) {
        dma_data_sel = CIC0_DOUT;
    } else {
        dma_data_sel = DSM_OUT;
    }

    SFR(JL_HADC->HADC_CON0, 12, 2, dma_data_sel);

    //...... cic0 sw calculate ......
    if (cic0->en) {
        ASSERT(((cic0->dsr >= 8) && (cic0->dsr <= 254)), "CIC0 DSR(8~254) ERR: %d!\n", cic0->dsr);
#if 0
        cic_log2 = log2(cic0->dsr);
        cic_iw   = 1 + (cic0->order + 3) * cic_log2;
        cic_iw_ceil = (char) ceil(cic_iw);
        cic_delta   = cic_iw_ceil - (24 + 2 + 2); //2bit precision bit
        cic_shift   = cic_delta;
        cic_shdir   = 0;
        if (cic_delta < 0) {
            cic_shdir = 1;
            cic_shift = 0 - cic_delta;
        }
#else
        cic_shift = cic0_dsr_order_to_shift[cic0->order][cic0->dsr - 8];
        cic_shdir = cic0_dsr_order_to_shdir[cic0->order][cic0->dsr - 8];
#endif
        log_info("CIC0 [%d], order: %d, shift:%d, shdir:%d", cic0->dsr, cic0->order, cic_shift, cic_shdir);
        SFR(JL_HADC->HADC_CON1, 16, 1, cic_shdir);//cic0_shift direction, 0:right shift 1:left shift
        SFR(JL_HADC->HADC_CON1, 11, 5, cic_shift);//cic0_shift
        SFR(JL_HADC->HADC_CON1,  9, 2, cic0->order);
        SFR(JL_HADC->HADC_CON1,  1, 8, cic0->dsr);
        SFR(JL_HADC->HADC_CON1,  0, 1, 1);
    } else {
        SFR(JL_HADC->HADC_CON1, 16, 1, 0);
        SFR(JL_HADC->HADC_CON1, 11, 5, 0);
        SFR(JL_HADC->HADC_CON1,  9, 2, 0);
        SFR(JL_HADC->HADC_CON1,  1, 8, 0);
        SFR(JL_HADC->HADC_CON1,  0, 1, 0);
    }

    // ...... cic1 sw calculate ......
    if (cic1->en) {
        ASSERT(((cic1->dsr >= 2) && (cic1->dsr <= 64)), "CIC1 DSR(2~64) ERR: %d!\n", cic1->dsr);
#if 0
        cic_log2 = log2(cic1->dsr);
        cic_iw   = 24 + (cic1->order + 2) * cic_log2;
        cic_iw_ceil = (char) ceil(cic_iw);
        cic_delta   = cic_iw_ceil - (24 + 2 + 3); //3bit precision bit
        cic_shift   = cic_delta;
        cic_shdir   = 0;
        if (cic_delta < 0) {
            cic_shdir = 1;
            cic_shift = 0 - cic_delta;
        }
#else
        cic_shift = cic1_dsr_order_to_shift[cic1->order][cic1->dsr - 2];
        cic_shdir = cic1_dsr_order_to_shdir[cic1->order][cic1->dsr - 2];
#endif
        log_info("CIC1 [%d], order: %d, shift:%d, shdir:%d", cic1->dsr, cic1->order, cic_shift, cic_shdir);
        SFR(JL_HADC->HADC_CON1, 31, 1, cic_shdir);//cic1_shift direction, 0:right shift 1:left shift
        SFR(JL_HADC->HADC_CON1, 26, 5, cic_shift);//cic1_shift
        SFR(JL_HADC->HADC_CON1, 24, 2, cic1->order);
        SFR(JL_HADC->HADC_CON1, 18, 6, cic1->dsr - 1);
        SFR(JL_HADC->HADC_CON1, 17, 1, 1);
    } else {
        SFR(JL_HADC->HADC_CON1, 31, 1, 0);
        SFR(JL_HADC->HADC_CON1, 26, 5, 0);
        SFR(JL_HADC->HADC_CON1, 24, 2, 0);
        SFR(JL_HADC->HADC_CON1, 18, 6, 0);
        SFR(JL_HADC->HADC_CON1, 17, 1, 1);
    }

    //......................... Fo calculating .................
    u32 fo, ivld_clk;
    ivld_clk = (WORK_MODE == 0) ? 5 : 0;
    if (cic0->en && cic1->en) {
        fo = __this->clock / ((cic0->dsr * 4 + ivld_clk) * cic1->dsr);
        log_info("[ CIC0 + CIC1 ], Fo = %d Hz, OSR = %d .......", fo, ((cic0->dsr * 4 + ivld_clk) * cic1->dsr));
    } else if (cic0->en) {
        fo = __this->clock / (cic0->dsr * 4 + ivld_clk);
        log_info("[ CIC0 ], Fo = %d Hz OSR = %d .......", fo, (cic0->dsr * 4 + ivld_clk));
    } else {
        log_info("[ DSM ], Fo = %d Hz .......", __this->clock);
    }
}

static void hadc_digital_init(struct hadc_platform_data *cfg)
{
    ASSERT((cfg->points >= 1), "points LEN ERR: %d!\n", cfg->points);
    __this->buffer_len = cfg->points * sizeof(int);
    if ((__this->buffer_len * 2) <= sizeof(hadc_run_buffer)) {
        __this->dual_buffer = hadc_run_buffer;
    } else {
        __this->dual_buffer = malloc(__this->buffer_len * 2);//双buffer
    }
    __this->isr_cb = cfg->isr_cb;
    ASSERT(__this->dual_buffer, "hadc malloc err, malloc len : %d!\n", __this->buffer_len * 2);

    JL_HADC->HADC_CON0 = BIT(6);
    JL_HADC->HADC_CON1 = 0;

    // MEMORY size and ADDRESS setting
    JL_HADC->HADC_LEN = cfg->points;
    JL_HADC->HADC_ADR = (u32)(__this->dual_buffer);
    JL_HADC->HADC_COP = 31;

    SFR(JL_HADC->HADC_CON0, 30, 1, 0);// cp_en
    SFR(JL_HADC->HADC_CON0, 23, 7, CIC1_GAIN);//cic1_gain
    SFR(JL_HADC->HADC_CON0, 16, 7, CIC0_GAIN);//cic0_gain
    SFR(JL_HADC->HADC_CON0, 11, 1, 1);//hadc_ref_clk_en
    SFR(JL_HADC->HADC_CON0, 10, 1, 1);//hadc_pga_clk_en
    SFR(JL_HADC->HADC_CON0,  9, 1, 1);//hadc_sdm_clk_en
    SFR(JL_HADC->HADC_CON0,  8, 1, 1);//hadc_clk_en
    SFR(JL_HADC->HADC_CON0,  3, 1, 0);//dma_cnt_clr
    SFR(JL_HADC->HADC_CON0,  2, 1, MEAN_EN);//hadc_mean_en
    SFR(JL_HADC->HADC_CON0,  1, 1, WORK_MODE);
    if (cfg->isr_cb) {
        request_irq(IRQ_HADC_IDX, 0, hadc_isr, 0);
        SFR(JL_HADC->HADC_CON0,  5, 1, 1);//hadc_ie
    } else {
        unrequest_irq(IRQ_HADC_IDX);
    }
    hadc_cic_init(&cfg->cic[0], &cfg->cic[1]);
}

void hadc_open(struct hadc_platform_data *data)
{
    if ((data != NULL) && (__this->hadc_init == 0)) {
        __this->clock = data->clock;
        hadc_clk_init(data->clock);
        hadc_digital_init(data);
        __this->hadc_init = 1;
    }
}

void hadc_close(void)
{
    if (__this->hadc_init) {
        JL_HADC->HADC_CON0 = BIT(6);
        JL_HADC->HADC_CON1 = 0;
        unrequest_irq(IRQ_HADC_IDX);
        if (__this->dual_buffer != hadc_run_buffer) {
            free(__this->dual_buffer);
        }
        __this->dual_buffer = 0;
        __this->hadc_init = 0;
    }
}

void hadc_set_out_freq(struct cic_data *cic0, struct cic_data *cic1)
{
    if ((cic0 == NULL) || (cic1 == NULL)) {
        log_error("cic0 or cic1 parm err!\n");
        return;
    }
    hadc_cic_init(cic0, cic1);
}

void hadc_start(void)
{
    SFR(JL_HADC->HADC_CON0,  0, 1, 1);//hadc_enable
}

void hadc_stop(void)
{
    SFR(JL_HADC->HADC_CON0,  0, 1, 0);//hadc_disable
}

void hadc_debug(void)
{
#if 0
    SFR(JL_ASS->CLK_CON, 16,  4,  ASS_DBG_CLKO_SEL_HADC_CLK);
    SFR(JL_ASS->CLK_CON, 20,  3,  ASS_DBG_DATO_SEL_HADC);
    gpio_mux_out(IO_PORTA_07, GPIO_OUT_COMP2_ASS_DBG_CLK);
    gpio_mux_out(IO_PORTA_08, GPIO_OUT_COMP3_ASS_DBG_D0);
    gpio_mux_out(IO_PORTA_09, GPIO_OUT_COMP4_ASS_DBG_D1);
#endif
}
