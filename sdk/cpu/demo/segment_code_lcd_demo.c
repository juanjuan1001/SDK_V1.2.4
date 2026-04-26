#include "gpio.h"
#include "asm/clock.h"
#include "asm/wdt.h"
#include "asm/segment_code_lcd_drv.h"

//=================================================================================//
//                              FOR TEST CODE                                      //
//=================================================================================//
#define  NUM_SYS_HUN_A    (LCD_POINT(0, 9))
#define  NUM_SYS_HUN_B    (LCD_POINT(0, 8))
#define  NUM_SYS_HUN_C    (LCD_POINT(2, 8))
#define  NUM_SYS_HUN_D    (LCD_POINT(2, 9))
#define  NUM_SYS_HUN_E    (LCD_POINT(1, 9))
#define  NUM_SYS_HUN_F    (LCD_POINT(INVALID_COM, 0))
#define  NUM_SYS_HUN_G    (LCD_POINT(1, 8))

#define  NUM_SYS_TEN_A    (LCD_POINT(0, 6))
#define  NUM_SYS_TEN_B    (LCD_POINT(1, 5))
#define  NUM_SYS_TEN_C    (LCD_POINT(2, 5))
#define  NUM_SYS_TEN_D    (LCD_POINT(2, 6))
#define  NUM_SYS_TEN_E    (LCD_POINT(2, 7))
#define  NUM_SYS_TEN_F    (LCD_POINT(1, 7))
#define  NUM_SYS_TEN_G    (LCD_POINT(1, 6))

#define  NUM_SYS_ONE_A    (LCD_POINT(0, 3))
#define  NUM_SYS_ONE_B    (LCD_POINT(1, 2))
#define  NUM_SYS_ONE_C    (LCD_POINT(2, 2))
#define  NUM_SYS_ONE_D    (LCD_POINT(2, 3))
#define  NUM_SYS_ONE_E    (LCD_POINT(2, 4))
#define  NUM_SYS_ONE_F    (LCD_POINT(1, 4))
#define  NUM_SYS_ONE_G    (LCD_POINT(1, 3))

#define  NUM_DIA_HUN_A    (LCD_POINT(3, 9))
#define  NUM_DIA_HUN_B    (LCD_POINT(3, 8))
#define  NUM_DIA_HUN_C    (LCD_POINT(5, 8))
#define  NUM_DIA_HUN_D    (LCD_POINT(5, 9))
#define  NUM_DIA_HUN_E    (LCD_POINT(4, 9))
#define  NUM_DIA_HUN_F    (LCD_POINT(INVALID_COM, 0))
#define  NUM_DIA_HUN_G    (LCD_POINT(4, 8))

#define  NUM_DIA_TEN_A    (LCD_POINT(3, 6))
#define  NUM_DIA_TEN_B    (LCD_POINT(3, 5))
#define  NUM_DIA_TEN_C    (LCD_POINT(4, 5))
#define  NUM_DIA_TEN_D    (LCD_POINT(5, 6))
#define  NUM_DIA_TEN_E    (LCD_POINT(4, 7))
#define  NUM_DIA_TEN_F    (LCD_POINT(3, 7))
#define  NUM_DIA_TEN_G    (LCD_POINT(4, 6))

#define  NUM_DIA_ONE_A    (LCD_POINT(3, 3))
#define  NUM_DIA_ONE_B    (LCD_POINT(3, 2))
#define  NUM_DIA_ONE_C    (LCD_POINT(4, 2))
#define  NUM_DIA_ONE_D    (LCD_POINT(5, 3))
#define  NUM_DIA_ONE_E    (LCD_POINT(4, 4))
#define  NUM_DIA_ONE_F    (LCD_POINT(3, 4))
#define  NUM_DIA_ONE_G    (LCD_POINT(4, 3))

static const u8 NUM_SYS_HUN[7] = {NUM_SYS_HUN_A, NUM_SYS_HUN_B, NUM_SYS_HUN_C, NUM_SYS_HUN_D, NUM_SYS_HUN_E, NUM_SYS_HUN_F, NUM_SYS_HUN_G};
static const u8 NUM_SYS_TEN[7] = {NUM_SYS_TEN_A, NUM_SYS_TEN_B, NUM_SYS_TEN_C, NUM_SYS_TEN_D, NUM_SYS_TEN_E, NUM_SYS_TEN_F, NUM_SYS_TEN_G};
static const u8 NUM_SYS_ONE[7] = {NUM_SYS_ONE_A, NUM_SYS_ONE_B, NUM_SYS_ONE_C, NUM_SYS_ONE_D, NUM_SYS_ONE_E, NUM_SYS_ONE_F, NUM_SYS_ONE_G};

static const u8 NUM_DIA_HUN[7] = {NUM_DIA_HUN_A, NUM_DIA_HUN_B, NUM_DIA_HUN_C, NUM_DIA_HUN_D, NUM_DIA_HUN_E, NUM_DIA_HUN_F, NUM_DIA_HUN_G};
static const u8 NUM_DIA_TEN[7] = {NUM_DIA_TEN_A, NUM_DIA_TEN_B, NUM_DIA_TEN_C, NUM_DIA_TEN_D, NUM_DIA_TEN_E, NUM_DIA_TEN_F, NUM_DIA_TEN_G};
static const u8 NUM_DIA_ONE[7] = {NUM_DIA_ONE_A, NUM_DIA_ONE_B, NUM_DIA_ONE_C, NUM_DIA_ONE_D, NUM_DIA_ONE_E, NUM_DIA_ONE_F, NUM_DIA_ONE_G};

//复用测试
static void segment_code_lcd_isr_cb(void)
{
    u8 i;
    gpio_set_mode(PORTC, PORT_PIN_7, PORT_OUTPUT_HIGH);
    for (i = 0; i < 10; i++) {
        gpio_write(IO_PORTC_07, 1);
        udelay(5);
        gpio_write(IO_PORTC_07, 0);
        udelay(5);
    }
    gpio_set_mode(PORTC, PORT_PIN_7, PORT_HIGHZ);
    segment_code_lcd_start();
}

const struct segment_code_lcd_platform_data lcd_test_data = {
    .clk = LCD_CLK_SEL_WCLK,
    .chgpump_vol = LCDCP_PUMPVOL_SEL_3P8V,
    .ldob_vol = LCDCP_LDOB_SEL_3P3V,
    .vlcd = LCD_VOL_SEL_1P000XLCDCP,
    .bias = LCD_BIAS_1DIV3,
    .chgmode = LCD_CHGMODE_STRONG,
    .chgduty = 0,
    .dot_com = LCD_COM0,
    .dot_seg = LCD_SEG18,
    .lcd_com_en = 0x3F,     //6com
    .lcd_seg_en = 0x7FFFFC, //21seg
    .ctu_en = 0, //0:断续推屏(en ie) 1:连续推屏
    .isr_cb = segment_code_lcd_isr_cb,
};

void segment_code_lcd_test(void)
{
    segment_code_lcd_init(&lcd_test_data);

    segment_code_lcd_disp_all();
    segment_code_lcd_disp_update();
    mdelay(500);

    segment_code_lcd_disp_clear();

    segment_code_lcd_fill_number(NUM_SYS_HUN, 1);
    segment_code_lcd_fill_number(NUM_SYS_TEN, 2);
    segment_code_lcd_fill_number(NUM_SYS_ONE, 3);

    segment_code_lcd_fill_number(NUM_DIA_HUN, 1);
    segment_code_lcd_fill_number(NUM_DIA_TEN, 5);
    segment_code_lcd_fill_number(NUM_DIA_ONE, 7);

    segment_code_lcd_disp_update();

    while (1) {
        wdt_clear();
    }
}


