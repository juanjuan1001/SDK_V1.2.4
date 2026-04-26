#ifndef __FONT_UNICODE__
#define __FONT_UNICODE__

#include "typedef.h"
#include "fs/fs.h"

#define FONT_SHOW_PIXEL   		0x01
#define FONT_SHOW_MULTI_LINE 	0x02 /* 显示多行文本(默认显示一行) */
#define FONT_DEFAULT			(FONT_SHOW_PIXEL)
#define LCD_FONT_DRAW_TOP    80    /* 文字在屏上显示的起始y坐标 */
struct font_file {
    char *name;
    FILE *fd;
};

struct font {
    struct font_file file;
    u8 size;
    u8 *pixelbuf;
    u16 nbytes;
};

struct font_info {
    struct font pixel;			//像素
    u8  bigendian;				//大端模式(unicode编码)
    u16 flags;
    u16 text_width;				//文本宽度
    u16 text_height;			//文本高度
    u16 string_width;			//字符串宽度
    u16 string_height;			//字符串高度
};

bool InitFont_Unicode(struct font_info *info);
struct font_info *font_unicode_open(struct font_info *info);
u16 TextOutW_Unicode(struct font_info *info, u8 *str, u16 len);
void font_copy_pixbuf(struct font_info *info, u16 width, u16 height, u16 x, u16 y);

void font_unicode_uninit(struct font_info *info);
struct font_info *font_unicode_init();
int font_unicode_display(struct font_info *info, u8 *pixbuf, u16 strlen);

#endif
