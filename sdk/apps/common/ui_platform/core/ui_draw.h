#ifndef _UI_DRAW_H
#define _UI_DRAW_H

/* 测试屏幕刷新时间(PA07)、推屏时间(PA06)使能、解压缩时间(PA07) */
#define TIME_DET_DEBUG 			0
#define TIME_REFR_TEST_PORT     IO_PORTA_07
#define TIME_PUSH_TEST_PORT     IO_PORTA_06
#define TIME_DECODER_TEST_PORT  IO_PORTA_07
#if TIME_DET_DEBUG
#define TIME_DET_START(a)  {gpio_hw_set_direction(IO_PORT_SPILT(a), 0); gpio_hw_write(a, 1);}
#define TIME_DET_END(a)    {gpio_hw_write(a, 0);}
#else
#define TIME_DET_START(a)
#define TIME_DET_END(a)
#endif

void ui_draw(void);

#endif
