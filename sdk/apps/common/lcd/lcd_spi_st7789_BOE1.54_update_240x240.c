#include "app_config.h"
#include "spi.h"
#include "lcd_drive.h"
#include "asm/clock.h"

#if TCFG_LCD_SPI_ST7789V_ENABLE

#define LOG_TAG_CONST   LCD
#define LOG_TAG         "[LCD_SPI]"
#include "debug.h"

#define delay_ms    mdelay

static InitCode LcdInit_code[] = {
    {0x01, 0},				// soft reset
    {REGFLAG_DELAY, 120}, 	// delay 120ms
    {0x11, 0},				// sleep out
    {REGFLAG_DELAY, 120},
    /* {0x35, 1, {0x00}}, //te */
    {0x36, 1, {0x00}},
    {0x3A, 1, {0x05}}, //lcd color format
    {0xB2, 5, {0x0c, 0x0c, 0x00, 0x33, 0x33}},
    {0xBB, 1, {0x1A}},
    {0xC0, 1, {0x2C}},
    {0xC2, 1, {0x01}},
    {0xC3, 1, {0x0F}},
    {0xC4, 1, {0x20}},
    {0xC6, 1, {0x0F}},
    {0xD0, 2, {0xA4, 0xA1}},
    {0xE0, 14, {0xD0, 0x13, 0x1A, 0x0A, 0x0A, 0x26, 0x3F, 0x54, 0x54, 0x18, 0x14, 0x14, 0x30, 0x33}},
    {0xE1, 14, {0xD0, 0x13, 0x1A, 0x0A, 0x0A, 0x26, 0x3F, 0x54, 0x54, 0x1A, 0x16, 0x16, 0x32, 0x35}},
    {0xE7, 1, {0x00}}, //spi data number
    {0X21, 0},
    {0X2A, 4, {0x00, 0x00, 0x00, 0xEF}},
    {0X2B, 4, {0x00, 0x00, 0x00, 0xEF}},
    {0X29, 0},
    {REGFLAG_DELAY, 20},
    {0X2C, 0},
    {REGFLAG_DELAY, 20},
};

void TFT_Write_Cmd(u8 data)
{
    lcd_cs_l();
    lcd_rs_l();
    lcd_spi_send_byte(data);
    lcd_cs_h();
}

void TFT_Write_Data(u8 data)
{
    lcd_cs_l();
    lcd_rs_h();
    lcd_spi_send_byte(data);
    lcd_cs_h();
}

void TFT_Write_Map(char *map, u32 size)
{
    spi_dma_send_map(map, size);
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
    delay_ms(10);
    lcd_reset_l();
    delay_ms(10);
    lcd_reset_h();
}


#define LCD_WIDTH 240
#define LCD_HIGHT 240
#define LINE_BUFF_SIZE  (30 * LCD_WIDTH * 2)

static char line_buffer[LINE_BUFF_SIZE] __attribute__((aligned(4)));


REGISTER_LCD_DRIVE() = {
    .name = "st7789v",
    .lcd_width = LCD_WIDTH,
    .lcd_height = LCD_HIGHT,
    .color_format = LCD_COLOR_RGB565,
    .interface = LCD_SPI,
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


