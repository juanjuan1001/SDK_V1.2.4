#include "app_config.h"
#include "spi.h"
#include "lcd_drive.h"
#include "asm/clock.h"
#include "asm/pap.h"
#include "gpio.h"

#if TCFG_LCD_PAP_SPI_ST7789V_240X320_ENABLE

#define LOG_TAG_CONST   LCD
#define LOG_TAG         "[LCD_PAP]"
#include "debug.h"

#define delay_ms    mdelay


static InitCode LcdInit_code[] = {
    {0x11, 0},				// sleep out
    {REGFLAG_DELAY, 120},
    {0x36, 1, {0x00}},
    {0x3a, 1, {0x05}},
    {0xb2, 5, {0x0c, 0x0c, 0x00, 0x33, 0x33}},
    {0xb7, 1, {0x35}},
    {0xbb, 1, {0x20}},
    {0xc0, 1, {0x2c}},
    {0xc2, 1, {0x01}},
    {0xc3, 1, {0x0b}},
    {0xc4, 1, {0x20}},
    {0xc6, 1, {0x0f}},
    {0xd0, 2, {0xa4, 0xa1}},
    {0xd6, 1, {0xa1}},
    {0xe0, 14, {0xd0, 0x03, 0x09, 0x0e, 0x11, 0x3d, 0x47, 0x55, 0x53, 0x1a, 0x16, 0x14, 0x1f, 0x22}},
    {0xe1, 14, {0xd0, 0x02, 0x08, 0x0d, 0x12, 0x2c, 0x43, 0x55, 0x53, 0x1e, 0x1b, 0x19, 0x20, 0x22}},
    /* {REGFLAG_DELAY, 120}, */
    {0x29, 0},
    /* {REGFLAG_DELAY, 120}, */
};

void TFT_Write_Cmd(u8 data)
{
    lcd_cs_l();
    lcd_rs_l();
#if (LCD_PUSH_MODE == PAP)
    hw_pap_tx_byte(0, data);
#elif (LCD_PUSH_MODE == SPI)
    lcd_spi_send_byte(data);
#else
#error "error! lcd_push_mode not found!, see lcd_pap_st7789_240x240.c"
#endif
    lcd_cs_h();
}

void TFT_Write_Data(u8 data)
{
    lcd_cs_l();
    lcd_rs_h();
#if (LCD_PUSH_MODE == PAP)
    hw_pap_tx_byte(0, data);
#elif (LCD_PUSH_MODE == SPI)
    lcd_spi_send_byte(data);
#else
#error "error! lcd_push_mode not found!, see lcd_pap_st7789_240x240.c"
#endif
    lcd_cs_h();
}

void TFT_Write_Map(char *map, u32 size)
{
#if (LCD_PUSH_MODE == PAP)
    hw_pap_write_buf(0, map, size);
#elif (LCD_PUSH_MODE == SPI)
    spi_dma_send_map(map, size);
#else
#error "error! lcd_push_mode not found!, see lcd_pap_st7789_240x240.c"
#endif
}

void TFT_Set_Draw_Area(int xs, int xe, int ys, int ye)
{
    TFT_Write_Cmd(0x2A);
    TFT_Write_Data(xs >> 8);
    TFT_Write_Data(xs);
    TFT_Write_Data(xe >> 8);
    TFT_Write_Data(xe);

    TFT_Write_Cmd(0x2B);
    TFT_Write_Data(ys >> 8);
    TFT_Write_Data(ys);
    TFT_Write_Data(ye >> 8);
    TFT_Write_Data(ye);

    TFT_Write_Cmd(0x2C);

    lcd_cs_l();
    lcd_rs_h();
}

static void TFT_BackLightCtrl(u8 on)
{
    if (on) {
        lcd_bl_h();
    } else {
        lcd_bl_l();
    }
}

static void TFT_EnterSleep()
{
    TFT_Write_Cmd(0x28);
    TFT_Write_Cmd(0x10);
    delay_ms(120);
}

static void TFT_ExitSleep()
{
    TFT_Write_Cmd(0x11);
    delay_ms(120);
    TFT_Write_Cmd(0x29);
}

static void lcd_reset()
{
    lcd_reset_h();
    delay_ms(15);
    lcd_reset_l();
    delay_ms(15);
    lcd_reset_h();
    delay_ms(15);
}

void lcd_st7789_set_direction(enum LCD_DIRECTION dir)
{
    switch (dir) {
    case ROTATE_0_CLOCKWISE:
        TFT_Write_Cmd(0x36);
        TFT_Write_Data(0x08);
        break;
    case ROTATE_180_CLOCKWISE:
        TFT_Write_Cmd(0x36);
        TFT_Write_Data(0xc8);
        break;
    case ROTATE_90_CLOCKWISE:
        TFT_Write_Cmd(0x36);
        TFT_Write_Data(0x78);
        break;
    case ROTATE_270_CLOCKWISE:
        TFT_Write_Cmd(0x36);
        TFT_Write_Data(0xa8);
        break;
    default:
        break;
    }
}

#define LCD_WIDTH 240
#define LCD_HIGHT 320
#define LINE_BUFF_SIZE  (30 * LCD_WIDTH * 2)

static char line_buffer[LINE_BUFF_SIZE] __attribute__((aligned(4)));


REGISTER_LCD_DRIVE() = {
    .name = "st7789v",
    .lcd_width = LCD_WIDTH,
    .lcd_height = LCD_HIGHT,
    .color_format = LCD_COLOR_RGB565,
    .interface = LCD_EMI,
    .column_addr_align = 1,
    .row_addr_align = 1,
    .dispbuf = line_buffer,
    .bufsize = LINE_BUFF_SIZE,
    .initcode = LcdInit_code,
    .initcode_cnt = sizeof(LcdInit_code) / sizeof(LcdInit_code[0]),
    .WriteComm = TFT_Write_Cmd,
    .WriteData = TFT_Write_Data,
    .WriteMap = TFT_Write_Map,
    .SetDrawArea = TFT_Set_Draw_Area,
    .reset = lcd_reset,
    .BackLightCtrl = TFT_BackLightCtrl,
    .EnterSleep = TFT_EnterSleep,
    .ExitSleep = TFT_ExitSleep,
};

#endif

