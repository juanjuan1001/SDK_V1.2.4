#ifndef _UI_API_H
#define _UI_API_H

//ui_platform_init初始化
void ui_platform_init(void);
//刷新屏幕
void ui_refr(disp_drv_t *disp);
//获取显存驱动指针
disp_drv_t *ui_get_disp_drv(void);
//获取当前刷新状态，1：刷新中   0：未处于刷新状态
u8 ui_get_refr_state(void);
/**
 * 获取当前图片控件中图片的数量
 * @param id：图片控件id
 */
u16 ui_obj_img_get_number(ui_page_t *page, obj_id_t id);
/**
 * 设置图片控件的内容
 * @param page：界面的首地址
 * @param id：图片控件的id号
 * @param num：图片控件中需要显示的图片序号
 */
void ui_obj_img_set_src(ui_page_t *page, obj_id_t id, u16 num);
/**
 * 设置图片控件的坐标
 * @param page：界面的首地址
 * @param id：图片控件的id号
 * @param pos：图片控件的坐标，超过屏幕大小的部分会被裁剪
 */
void ui_obj_img_set_pos(ui_page_t *page, obj_id_t id, Rect_t pos);
/**
 * 将图片控件添加到界面中
 * @param page：界面的首地址
 * @param id：图片控件的id号
 */
void ui_img_add_by_id(ui_page_t **page, obj_id_t id);
/**
 * 将图片控件从界面中删除
 * @param page：界面的首地址
 * @param id：图片控件的id号
 */
void ui_img_del_by_id(ui_page_t **page, obj_id_t id);

/**
 * 设置当前需要显示的界面
 * @param page：界面的首地址
 */
void ui_set_disp_page(ui_page_t *page);

/**
 * 给控件添加某个状态，如按下、隐藏
 * @param page：界面的首地址
 * @param id：图片控件的id号
 * @param state：需要添加的状态，可添加状态参考ui_obj_state_t
 */
void ui_img_add_state(ui_page_t *page, obj_id_t id, ui_obj_state_t state);
/**
 * 删除控件的某个状态
 * @param page：界面的首地址
 * @param id：图片控件的id号
 * @param state：需要删除的状态，可添加状态参考ui_obj_state_t
 */
void ui_img_del_state(ui_page_t *page, obj_id_t id, ui_obj_state_t state);

/**
 * 判断控件是否存在某个状态
 * @param page：界面的首地址
 * @param id：图片控件的id号
 * @param state：需要查询的状态，可添加状态参考ui_obj_state_t
 * @return 1：控件存在该状态，0：控件不存在该状态
 */
bool ui_img_state_get(ui_page_t *page, obj_id_t id, ui_obj_state_t state);

/**
 * 隐藏当前页面下的所有控件
 * @param page：界面的首地址
 */
void ui_add_page_hidden_state(ui_page_t *page);

/**
 * 清除当前页面下的所有控件的隐藏标志
 * @param page：界面的首地址
 */
void ui_del_page_hidden_state(ui_page_t *page);

#endif
