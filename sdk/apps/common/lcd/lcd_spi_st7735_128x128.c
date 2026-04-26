#include "app_config.h"
#include "spi.h"
#include "lcd_drive.h"
#include "asm/clock.h"
#include "asm/pap.h"
#include "gpio.h"

#if TCFG_LCD_SPI_ST7735V_128X128_ENABLE

#define LOG_TAG_CONST   LCD
#define LOG_TAG         "[LCD_PAP]"
#include "debug.h"

#define delay_ms    mdelay

static u8 lcd_xs_pos_offset = 2;
static u8 lcd_xe_pos_offset = 2;
static u8 lcd_ys_pos_offset = 1;
static u8 lcd_ye_pos_offset = 1;

static InitCode LcdInit_code[] = {
    {0x11, 0},				// sleep out
    {REGFLAG_DELAY, 120},
    {0xb1, 3, {0x02, 0x35, 0x36}},
    {0xb2, 3, {0x02, 0x35, 0x36}},
    {0xb3, 6, {0x02, 0x35, 0x36, 0x02, 0x35, 0x36}},
    {0xb4, 1, {0x03}},
    {0xc0, 3, {0xa2, 0x02, 0x84}},
    {0xc1, 1, {0xc5}},
    {0xc2, 2, {0x0d, 0x00}},
    {0xc3, 2, {0x8d, 0x2a}},
    {0xc4, 2, {0x8d, 0xee}},
    {0xc5, 1, {0x0a}},
    {0x36, 1, {0x08}},
    {0xd0, 2, {0xa4, 0xa1}},
    {0xe0, 16, {0x12, 0x1c, 0x10, 0x18, 0x33, 0x2c, 0x25, 0x28, 0x28, 0x27, 0x2f, 0x3c, 0x00, 0x03, 0x03, 0x10}},
    {0xe1, 16, {0x12, 0x1c, 0x10, 0x18, 0x2d, 0x28, 0x23, 0x28, 0x28, 0x26, 0x2f, 0x3b, 0x00, 0x03, 0x03, 0x10}},
    {0x3a, 1, {0x05}},
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
    xs += lcd_xs_pos_offset;
    xe += lcd_xe_pos_offset;
    ys += lcd_ys_pos_offset;
    ye += lcd_ye_pos_offset; //lcd screen offset
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

void lcd_st7735_set_direction(enum LCD_DIRECTION dir)
{
    switch (dir) {
    case ROTATE_0_CLOCKWISE:
        TFT_Write_Cmd(0x36);
        TFT_Write_Data(0x08);
        lcd_xs_pos_offset = 2;
        lcd_xe_pos_offset = 2;
        lcd_ys_pos_offset = 1;
        lcd_ye_pos_offset = 1;
        break;
    case ROTATE_180_CLOCKWISE:
        TFT_Write_Cmd(0x36);
        TFT_Write_Data(0xc8);
        lcd_xs_pos_offset = 2;
        lcd_xe_pos_offset = 2;
        lcd_ys_pos_offset = 3;
        lcd_ye_pos_offset = 3;
        break;
    case ROTATE_90_CLOCKWISE:
        TFT_Write_Cmd(0x36);
        TFT_Write_Data(0x78);
        lcd_xs_pos_offset = 1;
        lcd_xe_pos_offset = 1;
        lcd_ys_pos_offset = 2;
        lcd_ye_pos_offset = 2;
        break;
    case ROTATE_270_CLOCKWISE:
        TFT_Write_Cmd(0x36);
        TFT_Write_Data(0xa8);
        lcd_xs_pos_offset = 3;
        lcd_xe_pos_offset = 3;
        lcd_ys_pos_offset = 2;
        lcd_ye_pos_offset = 2;
        break;
    default:
        break;
    }
}

#define LCD_WIDTH 128
#define LCD_HIGHT 128
#define LINE_BUFF_SIZE  (30 * LCD_WIDTH * 2)

static char line_buffer[LINE_BUFF_SIZE] __attribute__((aligned(4)));


REGISTER_LCD_DRIVE() = {
    .name = "st7735v",
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

