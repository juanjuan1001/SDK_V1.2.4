#ifndef _UI_COLOR_H
#define _UI_COLOR_H

#include "typedef.h"

//交换2字节的RGB565颜色。如果用串行设备且为8位会很有用，因为u16类型强转u8指针会先指向低8位
#define UI_COLOR_16_SWAP 0  //该宏置一还有bug，暂未修复
typedef union {
    struct {
#if UI_COLOR_16_SWAP == 0
        uint16_t blue : 5;
        uint16_t green : 6;
        uint16_t red : 5;
#else
        uint16_t green_h : 3;
        uint16_t red : 5;
        uint16_t blue : 5;
        uint16_t green_l : 3;
#endif
    } ch;
    uint16_t full;
} ui_color16_t;

#define _UI_CONCAT3(x, y, z) x ## y ## z
#define UI_CONCAT3(x, y, z) _UI_CONCAT3(x, y, z)
#define UI_COLOR_DEPTH 		16   //目前只支持16bit位深的格式
typedef UI_CONCAT3(ui_color, UI_COLOR_DEPTH, _t) ui_color_t;

static inline ui_color_t ui_color_hex(u32 c)
{
#if UI_COLOR_DEPTH == 16
    ui_color_t r;
#if UI_COLOR_16_SWAP == 0
    /* Convert a 4 bytes per pixel in format ARGB32 to R5G6B5 format
                    r = ((c & 0xF80000) >> 8)
                    g = ((c & 0xFC00) >> 5)
                    b = ((c & 0xFF) >> 3)
                    rgb565 = r | g | b
        That's 3 mask, 3 bitshifts and 2 or operations */
    r.full = (uint16_t)(((c & 0xF80000) >> 8) | ((c & 0xFC00) >> 5) | ((c & 0xFF) >> 3));
#else
    /* We want: rrrr rrrr GGGg gggg bbbb bbbb => gggb bbbb rrrr rGGG */
    r.full = (uint16_t)(((c & 0xF80000) >> 16) | ((c & 0xFC00) >> 13) | ((c & 0x1C00) << 3) | ((c & 0xF8) << 5));
#endif
    return r;
#elif UI_COLOR_DEPTH == 32
    ui_color_t r;
    r.full = c | 0xFF000000;
    return r;
#else /*UI_COLOR_DEPTH == 8*/
    /* 暂不支持 */
#endif
}


typedef uint8_t ui_opa_t;
#define UI_OPA_MIN 		2    /*Opacities below this will be transparent*/
#define UI_OPA_MAX 		253  /*Opacities above this will fully cover*/
/* 默认背景色 */
#define DEFAULT_BG_COL  ui_color_hex(0x000000)    //黑色
#endif
