#include "app_config.h"
#include "spi.h"
#include "lcd_drive.h"
#include "asm/clock.h"
#include "asm/pap.h"
#include "gpio.h"

#if TCFG_LCD_PAP_SPI_ST7789V_240X240_ENABLE

#define LOG_TAG_CONST   LCD
#define LOG_TAG         "[LCD_PAP]"
#include "debug.h"

#define delay_ms    mdelay

static InitCode LcdInit_code[] = {
    {0X36, 1, {0X00}},
    {0X3A, 1, {0X05}},
    {0XB2, 5, {0X0C, 0X0C, 0X00, 0X33, 0X33}},
    {0XB7, 1, {0X35}},
    {0xBB, 1, {0x37}},
    {0xC0, 1, {0x2C}},
    {0xC2, 1, {0x01}},
    {0xC3, 1, {0x12}},
    {0xC4, 1, {0x20}},
    {0xC6, 1, {0x0F}},
    {0xd0, 2, {0xa4, 0xa1}},
    {0xE0, 14, {0xD0, 0x04, 0x0d, 0x11, 0x13, 0x2b, 0x3F, 0x54, 0x4c, 0x18, 0x0d, 0x0b, 0x1f, 0x23}},
    {0xE1, 14, {0xD0, 0x04, 0x0c, 0x11, 0x13, 0x2c, 0x3F, 0x44, 0x51, 0x2f, 0x1f, 0x1f, 0x20, 0x23}},
    {0X21, 0},
    {0x11, 0},				// sleep out
    /* {REGFLAG_DELAY, 120}, */
    {0X29, 0},
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
    /* printf("xs: %x, xe: %x, ys: %x, ye: %x", xs, xe, ys, ye); */
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

    gpio_set_mode(IO_PORT_SPILT(IO_PORTC_01), PORT_OUTPUT_LOW);
#if (LCD_PUSH_MODE == PAP)
    gpio_write(IO_PORTC_01, 0); //IM: 0(mcu), 1(SPI)
#elif (LCD_PUSH_MODE == SPI)
    gpio_write(IO_PORTC_01, 1); //IM: 0(mcu), 1(SPI)
#else
#error "error! lcd_push_mode not found!, see lcd_pap_st7789_240x240.c"
#endif
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
    .EnterSleep = NULL,
    .ExitSleep = NULL,
};

#endif

