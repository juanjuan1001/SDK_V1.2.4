#include "includes.h"
#include "lcd_drive.h"
#include "all_image_id.h"
#include "ui_ll.h"
#include "res_file.h"
#include "jlp_decoder.h"
#include "ui_core.h"
#include "ui_draw.h"

//#define LOG_TAG_CONST       UI
#define LOG_TAG             "[UI_CORE]"
#include "debug.h"

/* 配置显存buffer为单buffer还是双buffer，单buffer需要等spi推完数据再渲染下一块数据 */
#define UI_FRAME_SINGLE_BUFFER      0
#define UI_FRAME_DOUBLE_BUFFER      1
#define UI_FRAME_BUFFER_CONFIG      UI_FRAME_SINGLE_BUFFER

#define DISP_HOR_RES 				80  // 水平分辨率
#define DISP_VER_RES				160 // 垂直分辨率
#define DISP_VLOCK_H				20  // 推屏行数

static disp_drv_t *disp_driver = NULL;  // 显存

void ui_disp_driver_init(void)
{
    if (disp_driver == NULL) {
        disp_driver = malloc(sizeof(disp_drv_t));
        if (disp_driver == NULL) {
            log_error("disp_driver malloc fail");
            return;
        }
    }

    static ui_disp_draw_buf_t draw_buf;
    static u8 buf1[DISP_HOR_RES * DISP_VLOCK_H * 2];
#if (UI_FRAME_BUFFER_CONFIG == UI_FRAME_SINGLE_BUFFER)
    /* 单buffer */
    static u8 *buf2 = NULL;
#elif (UI_FRAME_BUFFER_CONFIG == UI_FRAME_DOUBLE_BUFFER)
    /* 双buffer */
    static u8 buf2[DISP_HOR_RES * DISP_VLOCK_H * 2];
#endif
    memset(&draw_buf, 0, sizeof(ui_disp_draw_buf_t));
    draw_buf.buf1 = buf1;
    draw_buf.buf2 = buf2;
    draw_buf.buf_act = draw_buf.buf1;
    draw_buf.buf_size = DISP_HOR_RES * DISP_VLOCK_H * 2;

    disp_driver->buf = &draw_buf;
    disp_driver->disp_width = lcd_get_screen_width();
    disp_driver->disp_height = lcd_get_screen_height();
    disp_driver->refr_line = DISP_VLOCK_H;
    disp_driver->bulk_num = (disp_driver->disp_height % disp_driver->refr_line == 0) ? \
                            (disp_driver->disp_height / disp_driver->refr_line) : \
                            (disp_driver->disp_height / disp_driver->refr_line + 1);

    disp_driver->bulk_index = 0;
    disp_driver->buf_offset = 0;
    disp_driver->bg_color = DEFAULT_BG_COL;
    disp_driver->flushing = 0;
    disp_driver->disp_page = NULL;  //表示还没有需要显示的page
    disp_driver->SetDrawArea = lcd_set_draw_area;
    disp_driver->Draw = lcd_draw;

    log_info("disp driver init finish");
}

int ui_obj_img_add(ui_img_t **head, obj_id_t id)
{
    pImageInfo_t pic = get_image_info_by_id(id);
    if (pic == NULL) {
        log_error("pic is NULL");
        return -1;
    }
    int res = append_node(head, 0, pic);
    if (res != 0) {
        log_error("obj img add fail");
        return res;
    }
    return res;
}

void ui_obj_img_del(ui_img_t **head, obj_id_t id)
{
    delete_node(head, id);
}

disp_drv_t *ui_get_disp_drv(void)
{
    return disp_driver;
}
