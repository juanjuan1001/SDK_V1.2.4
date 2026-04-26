#include "ui_demo.h"
#include "ui_core.h"
#include "ui_api.h"

//#define LOG_TAG_CONST       UI
#define LOG_TAG             "[UI]"
#include "debug.h"

/* 以下简易ui框架为测试demo */
typedef struct {
    ui_page_t *page_pic0;
} page_manage_t;
page_manage_t page_manage;

void ui_circle(void *priv)
{
    static u8 num = 0;

    ui_obj_img_set_src(page_manage.page_pic0, NUM, num);

    ui_refr(ui_get_disp_drv());

    if (++num == 10) {
        num = 0;
    }
}

void ui_platform_test(void)
{
    ui_platform_init();

    ui_img_add_by_id(&page_manage.page_pic0, NUM);

    ui_set_disp_page(page_manage.page_pic0);

    usr_timer_add(NULL, ui_circle, 200, 0);
}
