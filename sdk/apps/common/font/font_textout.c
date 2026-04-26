#include "font_textout.h"
#include "lcd_drive.h"

#define __this  ((struct spi_lcd_init *)&lcd_drive)

static struct font_info font_info_table[] = {
    {
        .flags = FONT_SHOW_PIXEL | FONT_SHOW_MULTI_LINE,
        .pixel.file.name = (char *)"mnt/sdfile/res/F_UNIC.PIX",
        .bigendian = false,
    },
};

static struct font_info *font_unicode_open(struct font_info *info)
{
    u8 ret = 0;
    if (!info) {
        info = font_info_table;
    }

    ret = InitFont_Unicode(info);

    if (info->pixel.nbytes) {
        info->pixel.pixelbuf = malloc(info->pixel.nbytes);
        if (!info->pixel.pixelbuf) {
            ret = FALSE;
        }
    }

    if (ret == TRUE) {
        return info;
    }
    font_unicode_uninit(info);
    return NULL;
}

void font_unicode_uninit(struct font_info *info)
{
    if (info->pixel.pixelbuf) {
        free(info->pixel.pixelbuf);
        info->pixel.pixelbuf = NULL;
    }
    if (info->pixel.file.fd) {
        sdfile_close(info->pixel.file.fd);
        info->pixel.file.fd = NULL;
    }
}

/* 填充推屏buffer */
void font_copy_pixbuf(struct font_info *info, u16 width, u16 height, u16 x, u16 y)
{
    int i, j, h;
    u16 osd_color;
    u32 size;

    u16 color = 0xff80;  //文字颜色，可改，RGB565
    osd_color = color & 0xffff ;
    //下面如果使用指针操作 要注意大小端
    osd_color = ((osd_color << 8) & 0xff00) | ((osd_color >> 8) & 0xff); //
    //如果使用打点操作，注释这个
    for (j = 0; j < (height + 7) / 8; j++) { /* 纵向8像素为1字节 */
        for (i = 0; i < width; i++) {
            if (((i + x) >= x)
                && ((i + x) <= (x + info->text_width - 1))) { /* x在绘制区域，要绘制 */
                u8 pixel = info->pixel.pixelbuf[j * width + i];
                int hh = height - (j * 8);
                if (hh > 8) {
                    hh = 8;
                }
                for (h = 0; h < hh; h++) {
                    if (((y + j * 8 + h) >= y)
                        && ((y + j * 8 + h) <= (y + LCD_FONT_DRAW_TOP + info->text_height - 1))) { /* y在绘制区域，要绘制 */
                        u16 clr = pixel & BIT(h) ? 1 : 0;
                        if (clr) {
                            u16 *pdisp = (u16 *)__this->dispbuf;
                            int offset = (y + j * 8 + h) * __this->lcd_width + (x + i);
                            if ((offset * 2 + 1) < __this->bufsize) {
                                pdisp[offset] = osd_color;
                            }
                        }
                    }
                } /* endof for h */
            }
        }/* endof for i */
    }/* endof for j */

}

static u16 __utf8_to_utf16(struct font_info *info, u8 *utf8_buf, u16 utf8_len, u16 *utf16_buf)
{
    u16 *putf16 = utf16_buf;
    u8 *putf8 = utf8_buf;
    u16 utf16;
    u16 utf16_len = 0;

    while (utf8_len) {
        if ((*putf8 >= 0xF0) && (*putf8 <= 0xF7)) {
            if (utf8_len < 4) {
                /* printf("4bytes utf8 code incorrect,discard it!\n"); */
                break;
            }

            if (putf16) {
                utf16 = 0xFFFF;
                if (info->bigendian) {
                    ((u8 *)putf16)[0] = utf16 >> 8;
                    ((u8 *)putf16)[1] = utf16 & 0xff;
                } else {
                    ((u8 *)putf16)[0] = utf16 & 0xff;
                    ((u8 *)putf16)[1] = utf16 >> 8;
                }
            }

            putf8 += 4;
            utf8_len -= 4;
        } else if ((*putf8 >= 0xE0) && (*putf8 <= 0xEF)) {
            if (utf8_len < 3) {
                /* printf("3bytes utf8 code incorrect,discard it!\n"); */
                break;
            }
            if (putf16) {
                utf16 = ((u16)(*putf8++ & 0x0F) << 12);
                utf16 |= ((u16)(*putf8++ & 0x3F) << 6);
                utf16 |= (*putf8++ & 0x3F);
                if (info->bigendian) {
                    ((u8 *)putf16)[0] = utf16 >> 8;
                    ((u8 *)putf16)[1] = utf16 & 0xff;
                } else {
                    ((u8 *)putf16)[0] = utf16 & 0xff;
                    ((u8 *)putf16)[1] = utf16 >> 8;
                }
            } else {
                putf8 += 3;
            }
            utf8_len -= 3;
        } else if ((*putf8 >= 0xC0) && (*putf8 <= 0xDF)) {
            if (utf8_len < 2) {
                /* printf("2bytes utf8 code incorrect,discard it!\n"); */
                break;
            }
            if (putf16) {
                utf16 = ((u16)(*putf8++ & 0x1F) << 6);
                utf16 |= (*putf8++ & 0x3F);
                if (info->bigendian) {
                    ((u8 *)putf16)[0] = utf16 >> 8;
                    ((u8 *)putf16)[1] = utf16 & 0xff;
                } else {
                    ((u8 *)putf16)[0] = utf16 & 0xff;
                    ((u8 *)putf16)[1] = utf16 >> 8;
                }
            } else {
                putf8 += 2;
            }
            utf8_len -= 2;
        } else if ((*putf8 >= 0) && (*putf8 <= 0x7F)) {
            if (utf8_len < 1) {
                /* printf("1byte utf8 code incorrect,discard it!\n"); */
                break;
            }
            if (putf16) {
                utf16 = *putf8++;
                if (info->bigendian) {
                    ((u8 *)putf16)[0] = utf16 >> 8;
                    ((u8 *)putf16)[1] = utf16 & 0xff;
                } else {
                    ((u8 *)putf16)[0] = utf16 & 0xff;
                    ((u8 *)putf16)[1] = utf16 >> 8;
                }
            } else {
                putf8 ++;
            }
            utf8_len --;
        } else {
            //ASSERT(0);
            printf("utf8 err!\n");
            break;
        }

        if (putf16) {
            putf16++;
        }
        utf16_len += 2;
    }

    return utf16_len;
}


struct font_info *font_unicode_init()
{
    struct font_info *info = NULL;
    info = font_unicode_open(NULL);
    if (!info) {
        return NULL;
    }
    info->text_width = __this->lcd_width; //显示区域宽度
    info->text_height = 60; //显示区域高度
    return info;
}
int font_unicode_display(struct font_info *info, u8 *pixbuf, u16 strlen)
{
    if ((info == NULL) || (pixbuf == NULL)) {
        return -1;
    }
    u8 *unicbuf = (u8 *)malloc(strlen * 2);
    if (unicbuf == NULL) {
        return -1;
    }
    /* 原始编码为unicode则不用转，这里用的utf-8 */
    u16 utf16_len = __utf8_to_utf16(info, (u8 *)pixbuf, strlen, (u16 *)unicbuf);
    TextOutW_Unicode(info, (u8 *)unicbuf, utf16_len);
    u16 offset_y = LCD_FONT_DRAW_TOP; //屏幕文字显示y偏移
    lcd_set_draw_area(0, __this->lcd_width - 1, offset_y, __this->bufsize / __this->lcd_width / 2 - 1);
    lcd_draw(__this->dispbuf, __this->bufsize, 0);
    free(unicbuf);

    return 0;
}

