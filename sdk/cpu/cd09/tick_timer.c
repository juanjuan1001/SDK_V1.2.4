#include "system/includes.h"

#define TICK_CON    (j32CPU(core_num())->TTMR_CON)
#define TICK_PRD    (j32CPU(core_num())->TTMR_PRD)
#define TICK_CNT    (j32CPU(core_num())->TTMR_CNT)

#define configSYSTICK_CLOCK_HZ  clk_get("sys")
#define configTICK_RATE_HZ      (500)

volatile unsigned long jiffies = 0;

___interrupt
static void tick_timer_isr(void)
{
    TICK_CON |= BIT(6);
    jiffies += 2;
    sys_timer_schedule();
}

void tick_timer_init(void)
{
    request_irq(IRQ_TICK_TMR_IDX, 1, tick_timer_isr, 0);
    TICK_CNT = 0;
    TICK_PRD = configSYSTICK_CLOCK_HZ  / configTICK_RATE_HZ - 1UL;
    TICK_CON = BIT(6) | BIT(0);
}

static void clock_critical_enter(void)
{
    TICK_CON = 0;
}

static void clock_critical_exit(void)
{
    TICK_CNT = 0;
    TICK_PRD = configSYSTICK_CLOCK_HZ  / configTICK_RATE_HZ - 1UL;
    TICK_CON = BIT(6) | BIT(0);
}
HSB_CRITICAL_HANDLE_REG(tick_timer, clock_critical_enter, clock_critical_exit)

