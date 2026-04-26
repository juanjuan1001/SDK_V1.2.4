#include "includes.h"
#include "ui_core.h"
#include "ui_ll.h"
#include "res_file.h"
#include "ui_draw.h"
#include "ui_api.h"

//#define LOG_TAG_CONST       UI
#define LOG_TAG             "[UI_DRAW]"
#include "debug.h"

/* 矩形区域是否和当前块重合，用于判断当前块是否需要显示该控件的内容 */
#define RECT_RIGHT(r)	((r)->x + (r)->width)
#define RECT_BOTTOM(r)	((r)->y + (r)->height)
/**
 * Whether there are common parts for both areas
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 * @return false: the two area has NO common parts
 */
static inline bool ui_check_img_in_bulk(const Rect_t *a1_p, const Rect_t *a2_p)
{
    /*If x1 or y1 greater than x2 or y2 then the areas union is empty*/
    bool union_ok = true;
    if ((MAX(a1_p->x, a2_p->x) > MIN(RECT_RIGHT(a1_p), RECT_RIGHT(a2_p))) || \
        (MAX(a1_p->y, a2_p->y) > MIN(RECT_BOTTOM(a1_p), RECT_BOTTOM(a2_p)))) {
        union_ok = false;
    }
    return union_ok;
}

/**
 * Get the common parts of two areas
 * @param res_p pointer to an area, the result will be stored here
 * @param a1_p pointer to the first area
 * @param a2_p pointer to the second area
 * @return false: the two area has NO common parts, res_p is invalid
 */
static inline bool ui_area_intersect(Rect_t *res_p, const Rect_t *a1_p, const Rect_t *a2_p)
{
    int right = 0;
    int bottom = 0;
    /*Get the smaller area from 'a1_p' and 'a2_p'*/
    res_p->x = MAX(a1_p->x, a2_p->x);
    res_p->y = MAX(a1_p->y, a2_p->y);
    right = MIN(RECT_RIGHT(a1_p), RECT_RIGHT(a2_p));
    bottom = MIN(RECT_BOTTOM(a1_p), RECT_BOTTOM(a2_p));

    /*If x1 or y1 greater than x2 or y2 then the areas union is empty*/
    bool union_ok = true;
    if ((res_p->x > right) || (res_p->y > bottom)) {
        union_ok = false;
    }
    res_p->width = right - res_p->x;
    res_p->height = bottom - res_p->y;
    return union_ok;
}

/* 设置背景颜色 */
static void ui_fill_bg_color(ui_color_t color)
{
    u8 *dispBuf = ui_get_disp_drv()->buf->buf_act;
    u32 bufSize = ui_get_disp_drv()->buf->buf_size;
#if (UI_COLOR_DEPTH == 16 && UI_COLOR_16_SWAP == 1)
    for (int i = 0; i < bufSize; i += 2) {
        dispBuf[i] = color.full & 0xFF;
        dispBuf[i + 1] = color.full >> 8;
    }
#else
    for (int i = 0; i < bufSize; i += 2) {
        dispBuf[i] = color.full >> 8;
        dispBuf[i + 1] = color.full & 0xFF;
    }
#endif
}

static void ui_data_to_display_buffer(disp_drv_t *disp_drv, ui_img_t *img)
{

}
/**
 * Blend the background and foreground images based on transparency
 * @param bg_color background color
 * @param fg_color foreground color
 * @param fg_opa transparency of foreground
 * @return: result of blending
 */
static inline ui_color_t ui_rgb_mix_alpha(ui_color_t bg_color, ui_color_t fg_color, u8 fg_opa)
{
    ui_color_t mix_color;
    if (fg_opa >= UI_OPA_MAX) {
        mix_color.full = fg_color.full;
    } else if (fg_opa <= UI_OPA_MIN) {
        mix_color.full = bg_color.full;
    } else {
#if (UI_COLOR_16_SWAP == 0)
        mix_color.ch.red = ((fg_color.ch.red * fg_opa) + (255 - fg_opa) * bg_color.ch.red) >> 8;
        mix_color.ch.blue = ((fg_color.ch.blue * fg_opa) + (255 - fg_opa) * bg_color.ch.blue) >> 8;
        mix_color.ch.green = ((fg_color.ch.green * fg_opa) + (255 - fg_opa) * bg_color.ch.green) >> 8;
#else
        mix_color.ch.red = ((fg_color.ch.red * fg_opa) + (255 - fg_opa) * bg_color.ch.red) >> 8;
        mix_color.ch.blue = ((fg_color.ch.blue * fg_opa) + (255 - fg_opa) * bg_color.ch.blue) >> 8;
        mix_color.ch.green_h = ((fg_color.ch.green_h * fg_opa) + (255 - fg_opa) * bg_color.ch.green_h) >> 8;
        mix_color.ch.green_l = ((fg_color.ch.green_l * fg_opa) + (255 - fg_opa) * bg_color.ch.green_l) >> 8;
#endif
    }
    return mix_color;
}

static void ui_jl_data_to_display_buffer(disp_drv_t *disp_drv, ui_img_t *img, u8 index, Rect_t pic_rect, Rect_t fill_rect)
{
    disp_drv_t *disp = disp_drv;
    ui_img_t *currentImg = img;
    u32 line = disp->refr_line;
    u8 bulk_index = index;
    u32 img_offset = 0;

    /* 读图片资源 */
    /* 适配第二版UI资源打包工具 */
    u32 rgb_addr = get_image_rgb_addr_by_info(currentImg->pic, currentImg->num);    //获取图片的RGB数据地址
    u32 rgb_size = get_image_rgb_size_by_info(currentImg->pic, currentImg->num);    //获取待解压数据大小
    int color_format = get_image_format_by_id(currentImg->pic->id);
    int pix_byte = get_pixel_byte_by_format(color_format);
    // log_info("bulk index: %d", bulk_index);
    // log_info("currentImg->num: %d", currentImg->num);
    // log_info("rgb_addr: 0x%x", rgb_addr);
    // log_info("rgb_size: %d", rgb_size);
    // log_info("fill rect----x: %d, y: %d, w: %d, h: %d", fill_rect.x, fill_rect.y, fill_rect.width, fill_rect.height);

    img_offset = (currentImg->rect.x < 0) ? (-currentImg->rect.x) : 0;
    /* 根据控件跟disp bulk的位置关系，设置显存buffer初始偏移量 */
    if ((int)pic_rect.y >= (int)(bulk_index * line)) {  //控件(x, y)在显存块内
        if (currentImg->rect.x < 0) {
            disp->buf_offset = ((pic_rect.y - (line * bulk_index)) * disp->disp_width) * pix_byte;
            img_offset = -currentImg->rect.x;
        } else {
            disp->buf_offset = ((pic_rect.y - (line * bulk_index)) * disp->disp_width + pic_rect.x) * pix_byte;
            img_offset = 0;
        }
    } else {                                //控件(x, y)在显存块上方
        if (currentImg->rect.x < 0) {
            disp->buf_offset = 0;
            img_offset = -currentImg->rect.x;
        } else {
            disp->buf_offset = pic_rect.x * pix_byte;
            img_offset = 0;
        }
    }
    /* 获取透明度数据地址 */
    u32 alpha_addr = get_image_alpha_addr_by_info(currentImg->pic, currentImg->num);
    if (alpha_addr != (u32)NULL) {
        u32 alpha_size = get_image_alpha_size_by_info(currentImg->pic, currentImg->num);
        u8 *_alpha_buf = malloc(currentImg->pic->rect.width);
        u8 *_rgb_buf = malloc(currentImg->pic->rect.width * pix_byte);
        if (_alpha_buf == NULL || _rgb_buf == NULL) {
            log_error("alpha buf malloc fail");
            if (_alpha_buf) {
                free(_alpha_buf);
                _alpha_buf = NULL;
            }
            if (_rgb_buf) {
                free(_rgb_buf);
                _rgb_buf = NULL;
            }
            return;
        }
        ui_color_t fg_color;
        ui_color_t bg_color;
        ui_color_t res_color;
        ui_opa_t fg_opa;
        ui_opa_t res_opa;
        TIME_DET_START(TIME_DECODER_TEST_PORT);
        if (currentImg->rect.y < (int)0) {      //解压缩y<0的部分
            for (int i = 0; i < -currentImg->rect.y; i++) {
                currentImg->jlp.alpha_result = jlp_deocde_block(currentImg->jlp.alpha_result, (u8 *)alpha_addr, \
                                               alpha_size, _alpha_buf, currentImg->pic->rect.width, 1);
                currentImg->jlp.rgb_result = jlp_deocde_block(currentImg->jlp.rgb_result, (u8 *)rgb_addr, rgb_size, _rgb_buf, \
                                             currentImg->pic->rect.width * pix_byte, pix_byte);
            }
        }
        for (int i = 0; i < fill_rect.height; i++) {
            currentImg->jlp.alpha_result = jlp_deocde_block(currentImg->jlp.alpha_result, (u8 *)alpha_addr, \
                                           alpha_size, _alpha_buf, currentImg->pic->rect.width, 1);
            currentImg->jlp.rgb_result = jlp_deocde_block(currentImg->jlp.rgb_result, (u8 *)rgb_addr, rgb_size, \
                                         _rgb_buf, currentImg->pic->rect.width * pix_byte, pix_byte);
            u8 *out_buf = disp->buf->buf_act + disp->buf_offset + (disp->disp_width * i * pix_byte);

            /* 裁剪并计算透明度 */
            u8 *rgb_buf = _rgb_buf + img_offset * pix_byte;   //偏移rgb_buf
            u8 *alpha_buf = _alpha_buf + img_offset * 1;        //偏移alpha_buf
            for (int j = 0; j < fill_rect.width; j++) {
                fg_color.full = (rgb_buf[j * pix_byte] << 8) | rgb_buf[j * pix_byte + 1];
                fg_opa = alpha_buf[j];
                bg_color.full = (out_buf[j * pix_byte] << 8) | out_buf[j * pix_byte + 1];
                res_color = ui_rgb_mix_alpha(bg_color, fg_color, fg_opa);

                out_buf[pix_byte * j] = res_color.full >> 8;
                out_buf[pix_byte * j + 1] = res_color.full;
            }
        }
        TIME_DET_END(TIME_DECODER_TEST_PORT);
        if (_alpha_buf) {
            free(_alpha_buf);
            _alpha_buf = NULL;
        }
        if (_rgb_buf) {
            free(_rgb_buf);
            _rgb_buf = NULL;
        }
    } else {
        /* 按行解压缩，并填充到显存buffer */
        TIME_DET_START(TIME_DECODER_TEST_PORT);
        uint8_t *out_buf = NULL;  //out_buf设为有效buffer
        uint8_t *temp_buf = malloc(currentImg->pic->rect.width * pix_byte);
        if (!temp_buf) {
            log_error("%s %d malloc fail", __func__, __LINE__);
            return;
        }
        if (currentImg->rect.y < (int)0) {   //解压缩y<0的部分
            for (int i = 0; i < -currentImg->rect.y; i++) {
                currentImg->jlp.rgb_result = jlp_deocde_block(currentImg->jlp.rgb_result, (u8 *)rgb_addr, rgb_size, temp_buf, \
                                             currentImg->pic->rect.width * pix_byte, pix_byte);
            }
        }
        //解压缩数据，并裁剪数据，存放到显存buffer
        for (int i = 0; i < fill_rect.height; i++) {
            currentImg->jlp.rgb_result = jlp_deocde_block(currentImg->jlp.rgb_result, (u8 *)rgb_addr, rgb_size, temp_buf, \
                                         currentImg->pic->rect.width * pix_byte, pix_byte);
            out_buf = disp->buf->buf_act + disp->buf_offset + (disp->disp_width * i * pix_byte); //out_buf跳转到下一行
            //裁剪数据，并保存到显存buffer
            if (currentImg->rect.x < 0) {
                if (-currentImg->rect.x < currentImg->pic->rect.width) {
                    memcpy(out_buf, temp_buf - (currentImg->rect.x * pix_byte), \
                           (currentImg->pic->rect.width + currentImg->rect.x)*pix_byte);        //只粘贴大于0的部分
                }
            } else if ((currentImg->rect.x + currentImg->pic->rect.width) > disp->disp_width) {
                if (currentImg->rect.x < disp->disp_width) {
                    memcpy(out_buf, temp_buf, (disp->disp_width - currentImg->rect.x)*pix_byte);     //全部粘贴
                }
            } else {
                memcpy(out_buf, temp_buf, currentImg->pic->rect.width * pix_byte);                   //全部粘贴
            }
        }
        if (temp_buf) {
            free(temp_buf);
            temp_buf = NULL;
        }
        TIME_DET_END(TIME_DECODER_TEST_PORT);
    }
}

static void ui_raw_data_to_display_buffer(disp_drv_t *disp_drv, Rect_t pic_rect, Rect_t fill_rect)
{

}

static void ui_refr_process(disp_drv_t *disp_drv)
{
    if (disp_drv == NULL) {
        log_error("disp driver is null");
        return;
    }

    if (disp_drv->flushing == 1) {  //标识还没刷新完上一屏幕数据，防止重入
        log_info("flushing");
        return;
    }

    if (disp_drv->disp_page == NULL) {  //标识没有page需要显示
        log_info("no page need to display");
        return;
    }

    Rect_t bulk_rect;          //保存显存buffer对应屏幕的坐标
    Rect_t fill_rect = {0};    //保存矩形区域相交部分
    Rect_t pic_rect;           //保存控件位置

    disp_drv_t *disp = disp_drv;
    u8 bulk_num = disp->bulk_num;
    u8 bulk_index = disp->bulk_index;
    u32 line = disp->refr_line;

    disp_drv->flushing = 1;     //正在渲染

    while (bulk_index < bulk_num) {
        /* 获取显存buffer对应屏幕的坐标 */
        bulk_rect.x = 0;
        bulk_rect.y = bulk_index * line;
        bulk_rect.width = disp->disp_width;
        bulk_rect.height = line;

        ui_img_t *currentImg = disp_drv->disp_page;
        /* 设置背景颜色 */
        ui_fill_bg_color(disp->bg_color);

        /* 遍历链表，查看是否存在bulk_index块中需要更新的内容 */
        while (currentImg != NULL) {
            pic_rect = currentImg->rect;
            currentImg->act_flag = ui_check_img_in_bulk(&bulk_rect, &pic_rect) && \
                                   (!!(currentImg->state & BIT(UI_OBJ_STATE_HIDDEN)) != 1);
            currentImg = currentImg->next;
        }

        /* 遍历链表，将图片需要的控件内容更新到显存buffer */
        currentImg = disp_drv->disp_page;   //回到链表头节点
        while (currentImg != NULL) {
            /* 将需要更新的块填充到dispBuf中 */
            if (currentImg->act_flag) {
                pic_rect = currentImg->rect;    //获取控件当前位置

                /* 获取相交区域 */
                if (!ui_area_intersect(&fill_rect, &bulk_rect, &pic_rect)) {
                    log_error("no fill area");
                    break;
                }

                if (currentImg->img_type == 0) {
                    /* 将jl数据解析并填充 */
                    ui_jl_data_to_display_buffer(disp, currentImg, bulk_index, pic_rect, fill_rect);
                } else {
                    /* 用户自定义数据解析并填充 */
                    ui_raw_data_to_display_buffer(disp, pic_rect, fill_rect);
                }
            }
            /* node_info_dump(currentImg, bulk_index); */
            currentImg = currentImg->next;
        }

        TIME_DET_START(TIME_REFR_TEST_PORT);
        if ((bulk_index + 1)*line > disp->disp_height) {
            u16 refr_line = disp->disp_height - bulk_index * line;
            disp->SetDrawArea(0, disp->disp_width - 1, bulk_index * line, disp->disp_height - 1);
            disp->Draw(disp->buf->buf_act, disp->disp_width * refr_line * 2, 1);
        } else {
            disp->SetDrawArea(0, disp->disp_width - 1, bulk_index * line, (bulk_index + 1)*line - 1);
            disp->Draw(disp->buf->buf_act, disp->disp_width * line * 2, 1);
        }
        TIME_DET_END(TIME_REFR_TEST_PORT);

        bulk_index++;

        /* 判断是否有双buffer */
        if (disp->buf->buf1 && disp->buf->buf2) {
            if (disp->buf->buf_act == disp->buf->buf1) {
                disp->buf->buf_act = disp->buf->buf2;
            } else {
                disp->buf->buf_act = disp->buf->buf1;
            }
        }
    }

    /* 刷新完成一屏数据，清除缓存信息 */
    ui_img_t *currentImg_clear = disp_drv->disp_page;
    while (currentImg_clear != NULL) {
        currentImg_clear->act_flag = 0;             //清除图片控件的act_flag
        currentImg_clear->jlp.rgb_result = 0;       //清除压缩暂存
        currentImg_clear->jlp.alpha_result = 0;     //清除压缩暂存
        currentImg_clear = currentImg_clear->next;
    }

    disp_drv->flushing = 0; //渲染刷新完成
}

void ui_draw(void)
{
    ui_refr_process(ui_get_disp_drv());
}
