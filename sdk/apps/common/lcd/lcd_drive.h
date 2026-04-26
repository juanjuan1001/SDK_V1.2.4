#ifndef __SPI_LCD_DRIVER_H
#define __SPI_LCD_DRIVER_H

#include "spi.h"

#define SPI		0
#define PAP		1
#define LCD_PUSH_MODE 	SPI //驱动方式
//屏幕驱动
#define TCFG_LCD_SPI_ST7735V_128X128_ENABLE         0
#define TCFG_LCD_SPI_ST7735V_80X160_ENABLE          1
#define TCFG_LCD_SPI_ST7789V_ENABLE                 0
#define TCFG_LCD_PAP_SPI_ST7789V_240X240_ENABLE     0
#define TCFG_LCD_PAP_SPI_ST7789V_240X320_ENABLE     0

#if (LCD_PUSH_MODE == SPI)
#define TCFG_TFT_LCD_DEV_SPI_HW_NUM	    1
#define LCD_SPI_INTERRUPT_ENABLE        1
#else
#define TCFG_TFT_LCD_DEV_SPI_HW_NUM	    0
#define LCD_SPI_INTERRUPT_ENABLE        0
#endif
//9 bit spi 屏需要打开该配置, 例如:lcd_spi_st7789_BOE1.54_update_240x240
#define TCFG_LCD_9BIT_SPI_ENABLE        0

/* 选择SPIx模块作为推屏通道，0、1、2 */
/* 但SPI0通常作为外部falsh使用，一般不用SPI0 */
#define SPI_MODULE_CHOOSE   TCFG_TFT_LCD_DEV_SPI_HW_NUM

#define REGFLAG_DELAY		0xFF

#ifndef LCD_SPI_INTERRUPT_ENABLE
#define LCD_SPI_INTERRUPT_ENABLE    1
#endif


#if LCD_SPI_INTERRUPT_ENABLE
#if SPI_MODULE_CHOOSE == 0
#define IRQ_SPI_IDX     IRQ_SPI0_IDX
#elif SPI_MODULE_CHOOSE == 1
#define IRQ_SPI_IDX     IRQ_SPI1_IDX
#elif SPI_MODULE_CHOOSE == 2
#define IRQ_SPI_IDX     IRQ_SPI2_IDX
#else
#error  "error! SPI_MODULE_CHOOSE defien error!"
#endif
#endif


enum LCD_IF {
    LCD_SPI,
    LCD_EMI,
};
enum LCD_COLOR {
    LCD_COLOR_RGB565,
    LCD_COLOR_MONO,
};

enum LCD_DIRECTION {
    ROTATE_0_CLOCKWISE,
    ROTATE_90_CLOCKWISE,
    ROTATE_180_CLOCKWISE,
    ROTATE_270_CLOCKWISE,
};

/* 定义初始化数据结构体 */
typedef struct {
    u8 cmd;		// 地址
    u8 cnt;		// 数据个数
    u8 dat[64];	// 数据
} InitCode;

struct spi_lcd_init {
    char *name;			// 名称
    u8 spi_pending;
    u8 soft_spi;
    u16 lcd_width;
    u16 lcd_height;
    u8  color_format;
    u8  interface;
    u8  column_addr_align;
    u8  row_addr_align;
    u8  backlight_status;
    char *dispbuf;
    u32 bufsize;
    InitCode *initcode;	// 初始化代码
    u16 initcode_cnt;	// 初始化代码条数
    void (*Init)(void);
    void (*WriteComm)(u8 cmd);	// 写命令
    void (*WriteData)(u8 dat);	// 写数据
    void (*WriteMap)(char *map, u32 size);	// 写整个buf
    void (*SetDrawArea)(int, int, int, int);
    void (*reset)(void);
    void (*BackLightCtrl)(u8);
    void (*EnterSleep)();
    void (*ExitSleep)();
};


#define REGISTER_LCD_DRIVE() \
	struct spi_lcd_init lcd_drive

extern struct spi_lcd_init lcd_drive;

typedef const int spi_dev;

struct lcd_spi_platform_data {
    u32  pin_reset;
    u32  pin_cs;
    u32  pin_rs;
    u32  pin_bl;
    enum spi_index spi_cfg;
    spi_hardware_info *spi_pdata;
};

enum ui_devices_type {
    LED_7,
    LCD_SEG3X9,
    TFT_LCD,//彩屏
    DOT_LCD,//点阵屏
};

struct ui_devices_cfg {
    enum ui_devices_type type;
    void *private_data;
};

void spi_dma_send_map(char *map, u32 size);
int lcd_spi_send_byte(u8 byte);
void lcd_reset_l();
void lcd_reset_h();
void lcd_cs_l();
void lcd_cs_h();
void lcd_rs_l();
void lcd_rs_h();
void lcd_bl_l();
void lcd_bl_h();

void push_screen_test(void);
void font_unicode_test(void);
int lcd_set_draw_area(int xs, int xe, int ys, int ye);
int lcd_draw(char *buf, u32 len, u8 wait);

void lcd_driver_init(void);
void lcd_driver_deinit(void);
int lcd_backlight_ctrl(u8 on);
int lcd_clear_screen(u16 color);
u16 lcd_get_screen_width();
u16 lcd_get_screen_height();
void lcd_set_screen_width(u16 width);
void lcd_set_screen_height(u16 height);
void lcd_fill_rect(u16 xs, u16 xe, u16 ys, u16 ye, u16 color);
void lcd_st7735_set_direction(enum LCD_DIRECTION dir);
void lcd_drawpoint(u16 x, u16 y, u16 color);
void lcd_draw_line(u16 xs, u16 ys, u16 xe, u16 ye, u16 color);

#endif

