#include "sin_generator/sin_generator.h"
#include "system/timer.h"
#include "asm/hadc.h"

u16 temp_timer;

void sin_generatore_test_timer_callbalk(void *priv)
{
    static u8 sin_f = 0;
    if (sin_f > SIN_GENERATOR_150K) {
        sin_f = 0;
    }
    sin_generator_start(sin_f, HADC_PA_GAIN_SEL_M0db);
    sin_f++;
}

//不同频率正弦波输出测试demo
void sin_generatore_test_demo_start(void)
{
    printf("\n***** SIN GENERATOR module test begin *****\n");
    if (!temp_timer) {
        sin_generatore_test_timer_callbalk(NULL);
        temp_timer = sys_timer_add(NULL, sin_generatore_test_timer_callbalk, 3000);
    }
}

void sin_generatore_test_demo_stop(void)
{
    sys_timer_del(temp_timer);
    temp_timer = 0;
    sin_generator_close();
    printf("\n***** SIN GENERATOR module test end *****\n");
}


