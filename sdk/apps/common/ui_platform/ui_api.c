#include "ui_core.h"
#include "ui_draw.h"
#include "ui_api.h"
#include "lcd_drive.h"
#include "ui_ll.h"
#include "res_file.h"
#include "all_image_data.h"

//#define LOG_TAG_CONST       UI
#define LOG_TAG             "[UI_API]"
#include "debug.h"

u16 elapse_time;

void ui_refr(disp_drv_t *disp)
{
    extern volatile unsigned long jiffies;

    TIME_DET_START(TIME_REFR_TEST_PORT);
    u32 start_time = jiffies;
    ui_draw();
    u32 end_time = jiffies;
    TIME_DET_END(TIME_REFR_TEST_PORT);

    //2ms是jiffies的时基
    elapse_time = abs(end_time - start_time) * 2;
}

u8 ui_get_refr_state(void)
{
    u8 state = ui_get_disp_drv()->flushing;
    if (state == 1) {
        log_info("flusing, it need %dms to refresh", elapse_time);
        return state;
    }
    return state;
}

u16 ui_obj_img_get_number(ui_page_t *page, obj_id_t id)
{
    ui_img_t *currentImg = find_node_by_id(page, id);
    if (currentImg == NULL) {
        log_error("find node fail");
        return -1;
    }
    return currentImg->pic->number;
}

void ui_obj_img_set_src(ui_page_t *page, obj_id_t id, u16 num)
{
    ui_img_t *currentImg = find_node_by_id(page, id);

    if (currentImg == NULL) {
        log_error("find node fail");
        return;
    }
    if (num < currentImg->pic->number) {
        currentImg->num = num;  //修改显示的内容
        // log_info("0x%x----%d image number %d", id, num, currentImg->pic->number);
    } else {
        log_error("0x%x----%d exceed image number %d", id, num, currentImg->pic->number);
    }
}

/* 设置控件位置 */
void ui_obj_img_set_pos(ui_page_t *page, obj_id_t id, Rect_t pos)
{
    ui_img_t *currentImg = find_node_by_id(page, id);
    if (currentImg == NULL) {
        log_error("find node fail");
        return;
    }
    currentImg->rect = pos; //设置显示的坐标

    /* 该做法需要把image_info的const标识去掉，否则没有修改权限 */
    // currentImg->pic->rect = pos;
}

/* 根据id添加图片控件到页面 */
void ui_img_add_by_id(ui_page_t **page, obj_id_t id)
{
    if (ui_get_disp_drv() == NULL) {
        log_error("please call ui_core_init function to initialize display driver");
        return;
    }
    /* 防止重复添加 */
    ui_img_t *currentImg = find_node_by_id(*page, id);
    if (currentImg != NULL) {
        log_info("this image has been added");
        return;
    }

    int res = ui_obj_img_add(page, id);
    if (res == 0) {
        log_info("add img succ, id is 0x%x", id);
    } else {
        log_error("add img fail, id is 0x%x", id);
    }
}

/* 根据id删除页面中的对应图片控件 */
void ui_img_del_by_id(ui_page_t **page, obj_id_t id)
{
    ui_obj_img_del(page, id);
}

/* 设置控件在页面中的状态，是否隐藏 */
void ui_img_add_state(ui_page_t *page, obj_id_t id, ui_obj_state_t state)
{
    ui_img_t *currentImg = find_node_by_id(page, id);
    if (currentImg == NULL) {
        log_error("find node fail");
        return;
    }
    currentImg->state |= BIT(state);
}

void ui_img_del_state(ui_page_t *page, obj_id_t id, ui_obj_state_t state)
{
    ui_img_t *currentImg = find_node_by_id(page, id);
    if (currentImg == NULL) {
        log_error("find node fail");
        return;
    }
    currentImg->state &= ~(BIT(state));
}

bool ui_img_state_get(ui_page_t *page, obj_id_t id, ui_obj_state_t state)
{
    ui_img_t *currentImg = find_node_by_id(page, id);
    if (currentImg == NULL) {
        log_error("find node fail");
        return 0;
    }
    return !!(currentImg->state & BIT(state));
}

void ui_add_page_hidden_state(ui_page_t *page)
{
    ui_page_t *node = page;
    while (node) {
        node->state |= BIT(UI_OBJ_STATE_HIDDEN);
        node = node->next;
    }
}

void ui_del_page_hidden_state(ui_page_t *page)
{
    ui_page_t *node = page;
    while (node) {
        node->state &= ~(BIT(UI_OBJ_STATE_HIDDEN));
        node = node->next;
    }
}

/* 设置当前显示的区域 */
void ui_set_disp_page(ui_page_t *page)
{
    ui_get_disp_drv()->disp_page = page;
    log_info("disp page 0x%x", (u32)ui_get_disp_drv()->disp_page);
}

void ui_res_file_register(void)
{
    image_info_register(image_info, ARRAY_LEN(image_info));
    image_used_list_register(image_used_list, ARRAY_LEN(image_used_list));
    image_search_info_register(image_search_info, ARRAY_LEN(image_search_info));
    all_image_data_register(all_image_data, ARRAY_LEN(all_image_data));
}

void ui_platform_init(void)
{
    log_info("%s", __func__);
    lcd_driver_init();
    lcd_backlight_ctrl(true);
    ui_res_file_printf_enable(1);   //使能res_file.c的log
    ui_res_file_register();
    ui_disp_driver_init();
}
