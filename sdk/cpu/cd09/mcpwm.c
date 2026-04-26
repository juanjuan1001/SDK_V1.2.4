#include "asm/mcpwm.h"
#include "asm/clock.h"
#include "gpio.h"
#include "spinlock.h"
#include "malloc.h"

#define LOG_TAG_CONST   MCPWM
#define LOG_TAG         "[MCPWM]"
#include "debug.h"

#define MCPWM_CLK   clk_get("mcpwm")
DEFINE_SPINLOCK(mcpwm_lock);

static struct mcpwm_info_t *mcpwm_info[MCPWM_NUM_MAX] = {NULL};

AT(.volatile_ram_code)
static MCPWM_TIMERx_REG *mcpwm_get_timerx_reg(mcpwm_ch_type ch)
{
    ASSERT((u32)ch < MCPWM_CH_MAX, "func:%s(), line:%d\n", __func__, __LINE__);
    MCPWM_TIMERx_REG *reg = NULL;
    reg = (MCPWM_TIMERx_REG *)(MCPWM_TMR_BASE_ADDR + ch * MCPWM_TMR_OFFSET);
    return reg;
}

AT(.volatile_ram_code)
static MCPWM_CHx_REG *mcpwm_get_chx_reg(mcpwm_ch_type ch)
{
    ASSERT((u32)ch < MCPWM_CH_MAX, "func:%s(), line:%d\n", __func__, __LINE__);
    MCPWM_CHx_REG *reg = NULL;
    reg = (MCPWM_CHx_REG *)(MCPWM_CH_BASE_ADDR + ch * MCPWM_CH_OFFSET);
    return reg;
}

static void mcpwm_reg_info_dump(int mcpwm_cfg_id)
{
    int id = mcpwm_cfg_id;
    u32 ch = (u32)mcpwm_info[id]->cfg.ch;
    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
    MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);
    log_info("tmr%d con = 0x%x", ch, timer_reg->tmr_con);
    log_info("tmr%d pr = 0x%x", ch, timer_reg->tmr_pr);
    log_info("pwm ch%d_con0 = 0x%x", ch, ch_reg->ch_con0);
    log_info("pwm ch%d_con1 = 0x%x", ch, ch_reg->ch_con1);
    log_info("pwm ch%d_cmph = 0x%x, pwm ch%d_cmpl = 0x%x", ch, ch_reg->ch_cmph, ch, ch_reg->ch_cmpl);
    log_info("pwm ch%d_fpin = 0x%x", ch, JL_MCPWM->FPIN_CON);
    log_info("MCPWM_CON0 = 0x%x", JL_MCPWM->MCPWM_CON0);
    log_info("mcpwm clk = %d", MCPWM_CLK);
}

static void (*mcpwm_cb_table[MCPWM_CH_MAX])(u32 ch);
static void mcpwm_cb(u32 ch)
{
    if (mcpwm_cb_table[ch]) {
        mcpwm_cb_table[ch](ch);
    }
    asm("csync");
}
___interrupt
static void mcpwm_fpin_cb()
{
    MCPWM_CHx_REG *ch_reg = NULL;
    for (u8 ch = 0; ch < MCPWM_CH_MAX; ch++) {
        ch_reg = mcpwm_get_chx_reg(ch);
        if (ch_reg->ch_con1 & BIT(MCPWM_CH_FPND)) {
            JL_MCPWM->MCPWM_CON0 &= ~BIT(ch + MCPWM_CON_PWM_EN);
            JL_MCPWM->MCPWM_CON0 &= ~BIT(ch + MCPWM_CON_TMR_EN);
            if (ch_reg->ch_con1 & BIT(MCPWM_CH_INTEN)) {
                ch_reg->ch_con1 &= ~BIT(MCPWM_CH_INTEN); //关闭故障保护输入IE使能
                mcpwm_cb(ch);
            }
        }
    }
}
static void mcpwm_cfg_info_load(int mcpwm_cfg_id)
{
    ASSERT(mcpwm_info[mcpwm_cfg_id] != NULL, "func:%s(), line:%d\n", __func__, __LINE__);
    int id = mcpwm_cfg_id;
    u32 ch = (u32)mcpwm_info[id]->cfg.ch;
    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);

    mcpwm_set_frequency(id, mcpwm_info[id]->cfg.aligned_mode, mcpwm_info[id]->cfg.frequency);

    mcpwm_set_duty(id, mcpwm_info[id]->cfg.duty);

    u16 ch_con0 = 0;
    u16 ch_con1 = 0;

    if (mcpwm_info[id]->cfg.dten) {
        ch_con0 |= BIT(MCPWM_CH_DTEN) | mcpwm_info[id]->cfg.dtpr << MCPWM_CH_DTPR | mcpwm_info[id]->cfg.dtckps << MCPWM_CH_DTCKPS;
    }

    if (mcpwm_info[id]->cfg.complementary_en && (!mcpwm_info[id]->cfg.dten)) {            //是否互补
        ch_con0 &= ~(BIT(MCPWM_CH_L_INV) | BIT(MCPWM_CH_H_INV));
        ch_con0 |= BIT(MCPWM_CH_L_INV); //L_INV
    } else {
        ch_con0 &= ~(BIT(MCPWM_CH_L_INV) | BIT(MCPWM_CH_H_INV));
    }
    ch_con1 &= ~(0b111 << MCPWM_CH_TMRSEL);
    ch_con1 |= (ch << MCPWM_CH_TMRSEL); //sel mctmr
    //H:
    if (mcpwm_info[id]->cfg.h_pin < IO_MAX_NUM) {      //任意引脚
        ch_con0 |= BIT(MCPWM_CH_H_EN);     //H_EN
        gpio_set_mode(IO_PORT_SPILT(mcpwm_info[id]->cfg.h_pin), PORT_OUTPUT_LOW);
        gpio_set_function(IO_PORT_SPILT(mcpwm_info[id]->cfg.h_pin), PORT_FUNC_MCPWM0_H + 2 * ch);
    }
    //L:
    if (mcpwm_info[id]->cfg.l_pin < IO_MAX_NUM) {      //任意引脚
        ch_con0 |= BIT(MCPWM_CH_L_EN);     //L_EN
        gpio_set_mode(IO_PORT_SPILT(mcpwm_info[id]->cfg.l_pin), PORT_OUTPUT_LOW);
        gpio_set_function(IO_PORT_SPILT(mcpwm_info[id]->cfg.l_pin), PORT_FUNC_MCPWM0_L + 2 * ch);
    }

    if (mcpwm_info[id]->cfg.detect_port < MCPWM_FPIN_NONE) { //需要开启故障保护功能
        ASSERT(mcpwm_info[id]->cfg.edge != MCPWM_EDGE_DEFAULT, "func:%s(), line:%d\n", __func__, __LINE__);
        u32 fpin_con = JL_MCPWM->FPIN_CON;
        asm("csync");
        if (mcpwm_info[id]->cfg.edge) { //上升沿
            if (mcpwm_info[id]->cfg.detect_port == MCPWM_FPIN_IO_PORTC_11) {
                gpio_set_mode(IO_PORT_SPILT(IO_PORTC_11), PORT_INPUT_PULLDOWN_10K);
            }
            fpin_con |=  BIT(MCPWM_FPIN_EDGE + ch);//上升沿触发
        } else {
            if (mcpwm_info[id]->cfg.detect_port == MCPWM_FPIN_IO_PORTC_11) {
                gpio_set_mode(IO_PORT_SPILT(IO_PORTC_11), PORT_INPUT_PULLUP_10K);
            }
            fpin_con &=  ~BIT(MCPWM_FPIN_EDGE + ch);//下升沿触发
        }
        fpin_con |=  BIT(MCPWM_FPIN_FLT_EN + ch);//开启滤波
        fpin_con |= (0b111111 << MCPWM_FPIN_FLT_PR); //滤波时间 = 16 * 64 / lsb_clk (单位：s)
        /* gpio_set_function(IO_PORT_SPILT(mcpwm_info[id]->cfg.detect_port), PORT_FUNC_MCPWM0_FP + ch); */
        mcpwm_cb_table[ch] = mcpwm_info[id]->cfg.irq_cb;
        request_irq(IRQ_MCPWM_CHX_IDX, mcpwm_info[id]->cfg.irq_priority, mcpwm_fpin_cb, 0);

        ch_con1 |= BIT(MCPWM_CH_FCLR) | BIT(MCPWM_CH_INTEN) | BIT(MCPWM_CH_FPINEN) | BIT(MCPWM_CH_FPINAUTO) | (mcpwm_info[id]->cfg.detect_port << MCPWM_CH_FPINSEL);
        spin_lock(&mcpwm_lock);
        JL_MCPWM->FPIN_CON = fpin_con;
        spin_unlock(&mcpwm_lock);
    }

    spin_lock(&mcpwm_lock);
    ch_con0 |= 0b11 << MCPWM_CH_CMP_LD;
    ch_reg->ch_con0 = ch_con0;
    ch_reg->ch_con1 = ch_con1;
    spin_unlock(&mcpwm_lock);
}

int mcpwm_init(struct mcpwm_config *mcpwm_cfg)
{
    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(mcpwm_cfg->ch);
    MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(mcpwm_cfg->ch);

    int cfg_id = -1;
    for (u32 i = 0; i < MCPWM_NUM_MAX; i++) {
        if (mcpwm_info[i]) {
            continue;
        }
        if (mcpwm_info[i] == NULL) {
            mcpwm_info[i] = (struct mcpwm_info_t *)malloc(sizeof(struct mcpwm_info_t));
            ASSERT(mcpwm_info[i] != NULL, "func:%s(), line:%d\n", __func__, __LINE__);
            cfg_id = i;
            break;
        }
    }
    if (cfg_id == -1) {
        log_info("mcpwm_config_id is null!!!\n");
        return cfg_id;
    }

    if (mcpwm_cfg->ofie) {
        timer_reg->tmr_con |= BIT(MCPWM_TMR_OFTE);
    }

    if (mcpwm_cfg->ufie) {
        timer_reg->tmr_con |= BIT(MCPWM_TMR_UFIE);
    }

    timer_reg->tmr_cnt = 0;

    mcpwm_info[cfg_id]->ch_reg = ch_reg;
    mcpwm_info[cfg_id]->timer_reg = timer_reg;
    memcpy(&mcpwm_info[cfg_id]->cfg, mcpwm_cfg, sizeof(struct mcpwm_config));
    log_info("mcpwm_info[%d]->ch_reg = 0x%x\n", cfg_id, (u32)mcpwm_info[cfg_id]->ch_reg);
    log_info("mcpwm_info[%d]->timer_reg = 0x%x\n", cfg_id, (u32)mcpwm_info[cfg_id]->timer_reg);
    log_info("mcpwm_info[%d]->cfg.ch = %d\n", cfg_id, (u32)mcpwm_info[cfg_id]->cfg.ch);
    log_info("mcpwm_info[%d]->cfg.alifned_mode = %d\n", cfg_id, (u32)mcpwm_info[cfg_id]->cfg.aligned_mode);
    log_info("mcpwm_info[%d]->cfg.frequency = %d\n", cfg_id, (u32)mcpwm_info[cfg_id]->cfg.frequency);
    log_info("mcpwm_info[%d]->cfg.duty = %d\n", cfg_id, (u32)mcpwm_info[cfg_id]->cfg.duty);
    log_info("mcpwm_info[%d]->cfg.h_pin = %d\n", cfg_id, (u32)mcpwm_info[cfg_id]->cfg.h_pin);
    log_info("mcpwm_info[%d]->cfg.l_pin = %d\n", cfg_id, (u32)mcpwm_info[cfg_id]->cfg.l_pin);
    log_info("mcpwm_info[%d]->cfg.complementary_en = %d\n", cfg_id, (u32)mcpwm_info[cfg_id]->cfg.complementary_en);
    log_info("mcpwm_info[%d]->cfg.detect_port = %d\n", cfg_id, (u32)mcpwm_info[cfg_id]->cfg.detect_port);
    log_info("mcpwm_info[%d]->cfg.irq_cb = %d\n", cfg_id, (u32)mcpwm_info[cfg_id]->cfg.irq_cb);
    log_info("mcpwm_info[%d]->cfg.irq_priority = %d\n", cfg_id, (u32)mcpwm_info[cfg_id]->cfg.irq_priority);
    return cfg_id;

}

void mcpwm_deinit(int mcpwm_cfg_id)
{
    ASSERT(mcpwm_info[mcpwm_cfg_id] != NULL, "func:%s(), line:%d\n", __func__, __LINE__);
    int id = mcpwm_cfg_id;
    u32 ch = (u32)mcpwm_info[id]->cfg.ch;
    MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);
    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);

    u32 mcpwm_con = JL_MCPWM->MCPWM_CON0;
    asm("csync");
    mcpwm_con &= ~BIT(ch + MCPWM_CON_PWM_EN);
    mcpwm_con &= ~BIT(ch + MCPWM_CON_TMR_EN);
    /* if ((JL_MCPWM->MCPWM_CON0 & 0xff) == 0) { */
    /*     mcpwm_con &= ~BIT(MCPWM_CON_CLK_EN); */
    /* } */
    spin_lock(&mcpwm_lock);
    JL_MCPWM->MCPWM_CON0 = mcpwm_con;
    spin_unlock(&mcpwm_lock);
    gpio_disable_function(IO_PORT_SPILT(mcpwm_info[mcpwm_cfg_id]->cfg.h_pin), PORT_FUNC_MCPWM0_H + 2 * ch);
    gpio_disable_function(IO_PORT_SPILT(mcpwm_info[mcpwm_cfg_id]->cfg.l_pin), PORT_FUNC_MCPWM0_L + 2 * ch);
    if (mcpwm_info[id]->cfg.detect_port < MCPWM_FPIN_NONE) { //有故障保护功能
        if (mcpwm_info[id]->cfg.detect_port == MCPWM_FPIN_IO_PORTC_11) {
            gpio_set_mode(IO_PORT_SPILT(IO_PORTC_11), PORT_HIGHZ);
        }
        spin_lock(&mcpwm_lock);
        ch_reg->ch_con1 = BIT(MCPWM_CH_FCLR);
        spin_unlock(&mcpwm_lock);
    }
    free(mcpwm_info[mcpwm_cfg_id]);
    mcpwm_info[mcpwm_cfg_id] = NULL;
}

void mcpwm_start(int mcpwm_cfg_id)
{
    ASSERT(mcpwm_info[mcpwm_cfg_id] != NULL, "func:%s(), line:%d\n", __func__, __LINE__);
    int id = mcpwm_cfg_id;
    u32 ch = (u32)mcpwm_info[id]->cfg.ch;
    MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);
    mcpwm_cfg_info_load(id);
    u32 mcpwm_con = JL_MCPWM->MCPWM_CON0;
    asm("csync");
    /* mcpwm_con |= BIT(MCPWM_CON_CLK_EN); */
    mcpwm_con |= BIT(ch + MCPWM_CON_TMR_EN);
    mcpwm_con |= BIT(ch + MCPWM_CON_PWM_EN);
    spin_lock(&mcpwm_lock);
    JL_MCPWM->MCPWM_CON0 = mcpwm_con;
    timer_reg->tmr_cnt = 0;
    mcpwm_reg_info_dump(id);
    spin_unlock(&mcpwm_lock);
}

void mcpwm_mux_start(u8 mcpwm_cfg_id_mux)
{
    for (u8 i = 0; i < 8; i++) {
        if (mcpwm_cfg_id_mux & BIT(i)) {
            ASSERT(mcpwm_info[i] != NULL, "func:%s(), line:%d\n", __func__, __LINE__);
            int id = i;
            u32 ch = (u32)mcpwm_info[id]->cfg.ch;
            MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);
            mcpwm_cfg_info_load(id);
            u32 mcpwm_con = JL_MCPWM->MCPWM_CON0;
            asm("csync");
            mcpwm_con |= BIT(ch + MCPWM_CON_PWM_EN);
            spin_lock(&mcpwm_lock);
            JL_MCPWM->MCPWM_CON0 = mcpwm_con;
            timer_reg->tmr_cnt = 0;
            spin_unlock(&mcpwm_lock);
        }
    }
    JL_MCPWM->MCPWM_CON0 |= (mcpwm_cfg_id_mux << MCPWM_CON_TMR_EN);
}

void mcpwm_pause(int mcpwm_cfg_id)
{
    int id = mcpwm_cfg_id;
    u32 ch = (u32)mcpwm_info[id]->cfg.ch;
    u32 mcpwm_con = JL_MCPWM->MCPWM_CON0;
    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);
    asm("csync");
    mcpwm_con &= ~BIT(ch + MCPWM_CON_PWM_EN);
    mcpwm_con &= ~BIT(ch + MCPWM_CON_TMR_EN);
    ch_reg->ch_con1 &= ~BIT(MCPWM_CH_INTEN); //关闭故障保护输入IE使能
    /* if ((JL_MCPWM->MCPWM_CON0 & 0xff) == 0) { */
    /*     mcpwm_con &= ~BIT(MCPWM_CON_CLK_EN); */
    /* } */
    spin_lock(&mcpwm_lock);
    JL_MCPWM->MCPWM_CON0 = mcpwm_con;
    spin_unlock(&mcpwm_lock);
    gpio_disable_function(IO_PORT_SPILT(mcpwm_info[mcpwm_cfg_id]->cfg.h_pin), PORT_FUNC_MCPWM0_H + 2 * ch);
    gpio_disable_function(IO_PORT_SPILT(mcpwm_info[mcpwm_cfg_id]->cfg.l_pin), PORT_FUNC_MCPWM0_L + 2 * ch);
}

void mcpwm_resume(int mcpwm_cfg_id)
{
    mcpwm_start(mcpwm_cfg_id);
}

void mcpwm_set_frequency(int mcpwm_cfg_id, mcpwm_aligned_mode_type align, u32 frequency)
{
    ASSERT(mcpwm_info[mcpwm_cfg_id] != NULL, "func:%s(), line:%d\n", __func__, __LINE__);
    int id = mcpwm_cfg_id;
    u32 ch = (u32)mcpwm_info[id]->cfg.ch;
    MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);

    u32 tmr_con = timer_reg->tmr_con;
    u16 tmr_pr = 0;
    asm("csync");

    u32 i = 0;
    u32 mcpwm_div_clk = 0;
    u32 mcpwm_tmr_pr = 0;
    u32 mcpwm_fre_min = 0;
    u32 clk = MCPWM_CLK;
    for (i = 0; i < 16; i++) {
        mcpwm_fre_min = clk / (65536 * (1 << i));
        if ((frequency >= mcpwm_fre_min) || (i == 15)) {
            break;
        }
    }
    tmr_con |= (i << MCPWM_TMR_CKPS); //div 2^i
    mcpwm_div_clk = clk / (1 << i);
    if (frequency == 0) {
        mcpwm_tmr_pr = 0;
    } else {
        if (align == MCPWM_CENTER_ALIGNED) { //中心对齐
            mcpwm_tmr_pr = mcpwm_div_clk / (frequency * 2) - 1;
        } else {
            mcpwm_tmr_pr = mcpwm_div_clk / frequency - 1;
        }
    }
    tmr_pr = mcpwm_tmr_pr;
    //timer mode
    if (align == MCPWM_CENTER_ALIGNED) { //中心对齐
        tmr_con |= 0b10; //递增-递降循环模式，中心对齐
    } else {
        tmr_con |= 0b01; //递增模式，边沿对齐
    }
    spin_lock(&mcpwm_lock);
    timer_reg->tmr_con = tmr_con;
    timer_reg->tmr_pr = tmr_pr;
    spin_unlock(&mcpwm_lock);
}

AT(.volatile_ram_code)
void mcpwm_set_duty_by_cnt(int mcpwm_cfg_id, u16 duty)
{
    ASSERT(mcpwm_info[mcpwm_cfg_id] != NULL, "func:%s(), line:%d\n", __func__, __LINE__);
    int id = mcpwm_cfg_id;
    u32 ch = (u32)mcpwm_info[id]->cfg.ch;
    MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);
    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);

    u16 ch_cmph = 0;
    u16 ch_cmpl = 0;
    u16 tmr_con = timer_reg->tmr_con;
    u16 tmr_pr = timer_reg->tmr_pr;
    asm("csync");

    spin_lock(&mcpwm_lock);
    if (duty >= tmr_pr) {
        duty = tmr_pr;
    }

    ch_cmpl = duty;
    ch_cmph = ch_cmpl;

    ch_reg->ch_cmph = ch_cmph;
    ch_reg->ch_cmpl = ch_cmpl;
    timer_reg->tmr_con = tmr_con;
    spin_unlock(&mcpwm_lock);
}

void mcpwm_set_duty(int mcpwm_cfg_id, u16 duty)
{
    ASSERT(mcpwm_info[mcpwm_cfg_id] != NULL, "func:%s(), line:%d\n", __func__, __LINE__);
    int id = mcpwm_cfg_id;
    u32 ch = (u32)mcpwm_info[id]->cfg.ch;
    MCPWM_TIMERx_REG *timer_reg = mcpwm_get_timerx_reg(ch);
    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);

    u16 ch_cmph = 0;
    u16 ch_cmpl = 0;
    u16 tmr_con = timer_reg->tmr_con;
    u16 tmr_pr = timer_reg->tmr_pr;
    asm("csync");

    ch_cmpl = tmr_pr * duty / 10000;
    /* printf("---%d---%d---%d\n", ch_cmpl, tmr_pr, duty); */
    ch_cmph = ch_cmpl;

    spin_lock(&mcpwm_lock);
    if (duty >= 10000) {
        timer_reg->tmr_cnt = 0;
    } else if (duty == 0) {
        timer_reg->tmr_cnt = ch_cmpl;
    }
    ch_reg->ch_cmph = ch_cmph;
    ch_reg->ch_cmpl = ch_cmpl;
    timer_reg->tmr_con = tmr_con;
    spin_unlock(&mcpwm_lock);
}

void mcpwm_fpnd_clr(u32 ch)
{
    MCPWM_CHx_REG *ch_reg = mcpwm_get_chx_reg(ch);

    u32 mcpwm_con = JL_MCPWM->MCPWM_CON0;
    u16 ch_con1 = ch_reg->ch_con1;
    asm("csync");
    /* mcpwm_con |= BIT(MCPWM_CON_CLK_EN); */
    mcpwm_con |= BIT(ch + MCPWM_CON_TMR_EN);
    mcpwm_con |= BIT(ch + MCPWM_CON_PWM_EN);
    ch_con1 |= BIT(MCPWM_CH_FCLR);
    spin_lock(&mcpwm_lock);
    JL_MCPWM->MCPWM_CON0 = mcpwm_con;
    ch_reg->ch_con1 = ch_con1;
    spin_unlock(&mcpwm_lock);
}


