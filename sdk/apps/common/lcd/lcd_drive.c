#include "app_config.h"
#include "gpio.h"
#include "lcd_drive.h"
#include "asm/clock.h"
#include "asm/wdt.h"
#include "asm/pap.h"

#define LOG_TAG_CONST   LCD
#define LOG_TAG         "[LCD_DRV]"
#include "debug.h"

static int spi_pnd = false;
static volatile u8 is_lcd_busy = 0;
static u8  lcd_sleep_in     = 0;
static struct lcd_spi_platform_data *spi_dat = NULL;
#define __this  ((struct spi_lcd_init *)&lcd_drive)

#if (LCD_PUSH_MODE == SPI)
static spi_hardware_info spi1_p_data = {
    .port = {
        IO_PORTC_01, //clk any io
        IO_PORTC_00, //do any io
        -1, //di any io
        -1,
        -1,
        -1,
    },
    .role = SPI_ROLE_MASTER,
    .mode = SPI_MODE_BIDIR_1BIT,//SPI_MODE_UNIDIR_2BIT,//SPI_MODE_BIDIR_1BIT,
    .bit_mode = SPI_FIRST_BIT_MSB,
    .cpol = 0,//clk level in idle state:0:low,  1:high
    .cpha = 0,//sampling edge:0:first,  1:second
    .ie_en = 0, //ie enbale:0:disable,  1:enable
    .spi_isr_callback = NULL,  //spi isr callback
    .clk  = 2000000L,
};

const static struct lcd_spi_platform_data lcd_spi_data = {
    .pin_reset	= IO_PORTC_11,
    .pin_rs	= IO_PORTC_02,
    .pin_cs	= IO_PORTD_02,
    .pin_bl     = IO_PORTA_08,
    .spi_cfg	= HW_SPI1,
    .spi_pdata  = &spi1_p_data,
};
#elif (LCD_PUSH_MODE == PAP)
const static struct lcd_spi_platform_data lcd_spi_data = {
    .pin_reset	= IO_PORTC_02,//触摸屏 和 屏公用复位，先初始化触摸
    .pin_rs	= IO_PORTC_04,
    .pin_cs	= IO_PORTC_03,
    .pin_bl     = IO_PORTC_00,
};
#else
#error "error! lcd_push_mode not found!, see lcd_pap_st7789_240x240.c"
#endif

const static struct ui_devices_cfg ui_cfg_data = {
    .type = TFT_LCD,
    .private_data = (void *) &lcd_spi_data,
};

static void spi_dma_wait_finish()
{
    if (spi_pnd) {
        while (!spi_get_pending(spi_dat->spi_cfg)) {
        }
        spi_clear_pending(spi_dat->spi_cfg);
        spi_pnd = false;
    }
}

static int __spi_dma_send(spi_dev spi, void *buf, u32 len, u8 wait)
{
    int err = 0;

    if (!wait || spi_pnd) {
        spi_dma_wait_finish();
    }
    spi_dma_transmit_for_isr(spi_dat->spi_cfg, buf, len, 0);
    spi_pnd = true;
    asm("csync");

    if (wait) {
        spi_dma_wait_finish();
    }

    return err;
}

void spi_dma_send_map(char *map, u32 size)
{
    int err = 0;

    if (spi_dat) {
        err = __spi_dma_send(spi_dat->spi_cfg, map, size, 0);
    }

    if (err < 0) {
        log_error("spi dma send map timeout\n");
    }

}

#if LCD_SPI_INTERRUPT_ENABLE
/* SPI中断函数 */
// 注：dma模式在发送数据时内部已经清理中断pnd
__attribute__((interrupt("")))
static void spi_isr()
{
    if (spi_get_pending(spi_dat->spi_cfg)) {
        /* printf("spi_isr\n"); */
        /* spi_clear_pending(spi_dat->spi_cfg); */
        if (is_lcd_busy) {
            is_lcd_busy = 0;
        }
        spi_set_ie(spi_dat->spi_cfg, 0);
    }
}
#endif

static void spi_init(spi_hardware_info *spi_info)
{
    int err;
    // spi gpio init
    err = spi_open(spi_dat->spi_cfg, spi_info);
    if (err < 0) {
        log_info("open spi falid\n");
    }
#if LCD_SPI_INTERRUPT_ENABLE
    // 配置中断优先级，中断函数
    /* spi_set_ie(spi_cfg, 1); */
    request_irq(IRQ_SPI_IDX, 3, spi_isr, 0);
#endif
}

// io口操作
void lcd_reset_l()
{
    gpio_set_mode(IO_PORT_SPILT((u32)spi_dat->pin_reset), PORT_OUTPUT_LOW);
}

void lcd_reset_h()
{
    gpio_set_mode(IO_PORT_SPILT((u32)spi_dat->pin_reset), PORT_OUTPUT_HIGH);
}

void lcd_cs_l()
{
    gpio_set_mode(IO_PORT_SPILT((u32)spi_dat->pin_cs), PORT_OUTPUT_LOW);
}

void lcd_cs_h()
{
    gpio_set_mode(IO_PORT_SPILT((u32)spi_dat->pin_cs), PORT_OUTPUT_HIGH);
}

void lcd_rs_l()
{
#if TCFG_LCD_9BIT_SPI_ENABLE
    /* 推屏使用的SPIx */
    JL_SPI1->CON1 &= (~BIT(4));
#else
    gpio_set_mode(IO_PORT_SPILT((u32)spi_dat->pin_rs), PORT_OUTPUT_LOW);
#endif
}

void lcd_rs_h()
{
#if TCFG_LCD_9BIT_SPI_ENABLE
    /* 推屏使用的SPIx */
    JL_SPI1->CON1 |= BIT(4);
#else
    gpio_set_mode(IO_PORT_SPILT((u32)spi_dat->pin_rs), PORT_OUTPUT_HIGH);
#endif
}

void lcd_bl_l()
{
    gpio_set_mode(IO_PORT_SPILT((u32)spi_dat->pin_bl), PORT_OUTPUT_LOW);
}

void lcd_bl_h()
{
    gpio_set_mode(IO_PORT_SPILT((u32)spi_dat->pin_bl), PORT_OUTPUT_HIGH);
}


int lcd_spi_send_byte(u8 byte)
{
    int ret;

    ret = spi_send_byte(spi_dat->spi_cfg, byte);
    spi_pnd = false;

    return 0;
}

int lcd_set_draw_area(int xs, int xe, int ys, int ye)
{
    if ((is_lcd_busy == 0x11) || lcd_sleep_in) {
        return 0;
    }
    is_lcd_busy = 1;
#if (LCD_PUSH_MODE == SPI)
    spi_set_ie(spi_dat->spi_cfg, 0);
    spi_dma_wait_finish();
#endif
    __this->SetDrawArea(xs, xe, ys, ye);
    return 0;
}

int lcd_draw(char *buf, u32 len, u8 wait)
{
    if ((is_lcd_busy == 0x11) || lcd_sleep_in) {
        return 0;
    }

    if (wait) {
        __this->WriteMap(buf, len);
#if LCD_SPI_INTERRUPT_ENABLE
        spi_set_ie(spi_dat->spi_cfg, 1);
#else
        is_lcd_busy = 0;
#endif
    } else {
        __this->WriteMap(buf, len);
        is_lcd_busy = 0;
    }
#if (LCD_PUSH_MODE == SPI)
    spi_dma_wait_finish();
#endif
    return 0;
}

int lcd_clear_screen(u16 color)
{
    int i;
    int buffer_lines;
    int remain;
    int draw_line;
    int y;

    if (__this->color_format == LCD_COLOR_RGB565) {
        buffer_lines = __this->bufsize / __this->lcd_width / 2;
        log_debug("buffer_lines:%d, lcd_width:%d, color:0x%x\n", buffer_lines, __this->lcd_width, color);
        for (i = 0; i < buffer_lines * __this->lcd_width; i++) {
            __this->dispbuf[2 * i] = color >> 8;
            __this->dispbuf[2 * i + 1] = color;
        }

        y = 0;
        remain = __this->lcd_height;
        while (remain) {
            draw_line = buffer_lines > remain ? remain : buffer_lines;
            lcd_set_draw_area(0, __this->lcd_width - 1, y, y + draw_line - 1);
            lcd_draw(__this->dispbuf, draw_line * __this->lcd_width * 2, 0);
#if (LCD_PUSH_MODE == SPI)
            spi_dma_wait_finish();
#endif
            remain -= draw_line;
            y += draw_line;
        }
    } else if (__this->color_format == LCD_COLOR_MONO) {
        __this->SetDrawArea(0, -1, 0, -1);
        memset(__this->dispbuf, 0x00, __this->lcd_width * __this->lcd_height / 8);
        __this->WriteMap(__this->dispbuf, __this->lcd_width * __this->lcd_height / 8);
    } else {
        ASSERT(0, "the color_format %d not support yet!", __this->color_format);
    }

    return 0;
}

static void lcd_init_code(const InitCode *code, u8 cnt)
{
    u8 i, j;

    for (i = 0; i < cnt; i++) {
        if (code[i].cmd == REGFLAG_DELAY) {
            mdelay(code[i].cnt);
            wdt_clear();
        } else {
            __this->WriteComm(code[i].cmd);
            log_debug("%02x ", code[i].cmd);
            for (j = 0; j < code[i].cnt; j++) {
                __this->WriteData(code[i].dat[j]);
                log_debug("%02x ", code[i].dat[j]);
            }
        }
    }
}


static void lcd_9bit_spi_mode_init()
{
#if TCFG_LCD_9BIT_SPI_ENABLE
    JL_SPI1->CON1 |= BIT(3);
    JL_SPI1->CON |= BIT(5);
#endif
    return;
}

/* LCD初始化: IO初始化以及发送屏幕初始化代码 */
static void lcd_dev_init(const void *p)
{
    log_info("lcd_dev_init...\n");
    struct ui_devices_cfg *cfg = (struct ui_devices_cfg *)p;
    int err = 0;
    spi_dat = (struct lcd_spi_platform_data *)cfg->private_data;
    ASSERT(spi_dat, "Error! spi io not config");

    gpio_set_mode(IO_PORT_SPILT((u32)spi_dat->pin_cs), PORT_OUTPUT_HIGH);
    gpio_set_mode(IO_PORT_SPILT((u32)spi_dat->pin_rs), PORT_OUTPUT_HIGH);

#if (LCD_PUSH_MODE == SPI)
    log_info("SPI MODE!\n");
    spi_init(spi_dat->spi_pdata);
    lcd_9bit_spi_mode_init();
#elif (LCD_PUSH_MODE == PAP)
    log_info("PAP MODE!\n");
    hw_pap_init(0);
#else
#error "error! lcd_push_mode not found!, see lcd_pap_st7789_240x240.c"
#endif
    if (__this->reset) {
        __this->reset();
    }
    log_info("lcd_width:%d, lcd_height:%d, color_format:%d\n", __this->lcd_width, __this->lcd_height, __this->color_format);
    lcd_init_code(__this->initcode, __this->initcode_cnt);  // 初始化屏幕
}

/* 屏幕背光控制 */
int lcd_backlight_ctrl(u8 on)
{
    static u8 first_power_on = true;

    if (first_power_on) {
        log_info("first_power_on\n");
        lcd_clear_screen(0x0000);
        mdelay(30);
        first_power_on = false;
    }

    if (__this->BackLightCtrl) {
        if (on) {
            log_info("start\n");
            if (__this->ExitSleep) {
                __this->ExitSleep();
                log_info("exit sleep!\n");
            }
            __this->BackLightCtrl(on);
            log_info("finished!\n");
        } else {
            __this->BackLightCtrl(false);
            if (__this->EnterSleep) {
                __this->EnterSleep();
            }
        }
        lcd_sleep_in = !on;
    }

    return 0;
}

/* 屏幕初始化 */
void lcd_driver_init(void)
{
    lcd_dev_init(&ui_cfg_data);
}

/* 关闭屏幕 */
void lcd_driver_deinit(void)
{
    if (__this->BackLightCtrl) {
        __this->BackLightCtrl(false);
    }
    if (__this->EnterSleep) {
        __this->EnterSleep();
    }
}

/* 纯色推屏测试 format:RGB565 */
void push_screen_test(void)
{
#define	INTERVAL_TIME		1000	// 刷屏测试间隔时间

    u16 color_tab[] = {0xf800, 0x07e0, 0x001f};
    int color_num = sizeof(color_tab) / sizeof(color_tab[0]);
    static u8 i = 0;

    lcd_dev_init(&ui_cfg_data);
    lcd_backlight_ctrl(true);

    log_info("start push screen\n");

    while (1) {
        lcd_clear_screen(color_tab[i]);

        if (++i >= color_num) {
            i = 0;
        }
        mdelay(INTERVAL_TIME);
        wdt_clear();
    }
}


#include "font_textout.h"
void font_unicode_test(void)
{
    int i = 0;
    lcd_dev_init(&ui_cfg_data);
    lcd_backlight_ctrl(true);

    log_info("start show text!\n");

    struct font_info *font_unicode_init(); //字库初始化
    void font_unicode_uninit(struct font_info * info); //关闭字库资源
    int font_unicode_display(struct font_info * info, u8 * pixbuf, u16 strlen); //显示字库
    struct font_info *info = NULL;
    info = font_unicode_init();
    /* 测试文字 */
    char *str[5] = {
        "本工具主要目的是实现hex与字符之间的转换",
        "目前支持utf-8/unicode及gbk(兼容gb2312)编码",
        "“字符编码”为“自动”时，将自动识别hex内容并使用正确的编码处理及优化",
        "如果不能识别或是转hex那么将使用默认utf8编码处理",
        "输入汉字查询对应的编码，或者输入编码查询对应的汉字"
    };

    while (1) {
        lcd_clear_screen(0x0000);
        printf("text content: %s\n", str[i]);
        font_unicode_display(info, (u8 *)str[i], strlen(str[i]));
        if (++i >= 5) {
            i = 0;
        }
        mdelay(1000);
        wdt_clear();
    }
    font_unicode_uninit(info);
}

u16 lcd_get_screen_width()
{
    return __this->lcd_width;
}

u16 lcd_get_screen_height()
{
    return __this->lcd_height;
}

void lcd_set_screen_width(u16 width)
{
    __this->lcd_width = width;
}

void lcd_set_screen_height(u16 height)
{
    __this->lcd_height = height;
}

void lcd_fill_rect(u16 xs, u16 xe, u16 ys, u16 ye, u16 color)
{
    u16 i, j;
    lcd_set_draw_area(xs, xe - 1, ys, ye - 1);
    for (i = ys; i < ye; i++) {
        for (j = xs; j < xe; j++) {
            __this->WriteData(color >> 8);
            __this->WriteData(color);
        }
    }
}

void lcd_drawpoint(u16 x, u16 y, u16 color)
{
    lcd_set_draw_area(x, x, y, y);
    __this->WriteData(color >> 8);
    __this->WriteData(color);
}

void lcd_draw_line(u16 xs, u16 ys, u16 xe, u16 ye, u16 color)
{
    u32 e, f, i, j, g, flag;
    flag = 0;
    if (xe > xs) {
        e = xe - xs;
    } else {
        e = xs - xe;
        flag |= 0x01;
    }
    if (ye > ys) {
        f = ye - ys;
    } else {
        f = ys - ye;
        flag |= 0x02;
    }
    if (f > e) {
        g = f;
        f = e;
        e = g;
        flag |= 0x04;
    }
    j = 0;
    for (i = 0; i < e + 1; i++) {
        switch (flag & 0x07) {
        case 0:
            lcd_drawpoint(xs + i, ys + j, color);
            break;
        case 1:
            lcd_drawpoint(xs - i, ys + j, color);
            break;
        case 2:
            lcd_drawpoint(xs + i, ys - j, color);
            break;
        case 3:
            lcd_drawpoint(xs - i, ys - j, color);
            break;
        case 4:
            lcd_drawpoint(xs + j, ys + i, color);
            break;
        case 5:
            lcd_drawpoint(xs - j, ys + i, color);
            break;
        case 6:
            lcd_drawpoint(xs + j, ys - i, color);
            break;
        case 7:
            lcd_drawpoint(xs - j, ys - i, color);
            break;
        default:
            break;
        }
        if (e * (2 * j + 1) < 2 * f * (i + 1)) {
            j++;
        }
    }
}

