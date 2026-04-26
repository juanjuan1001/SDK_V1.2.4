#ifndef UI_CORE_H
#define UI_CORE_H

#include "includes.h"
#include "all_image_info.h"
#include "all_image_id.h"
#include "ui_color.h"

/* 控件状态 */
typedef enum {
    UI_OBJ_STATE_HIDDEN = 0,    //BIT(0)
    UI_OBJ_STATE_PRESS,

    UI_OBJ_STATE_MAX = 7,       //BIT(7)
} ui_obj_state_t;

/* rle结构体定义 */
typedef struct {
    u32 rgb_result;        //分块解压进度暂存
    u32 alpha_result;      //分块解压进度暂存
} ui_jlp_t;

/* 显存buffer */
typedef struct {
    void *buf1;
    void *buf2;
    void *buf_act;
    u32 buf_size;
} ui_disp_draw_buf_t;

// 定义链表节点结构
typedef struct Node_img {
    u8 img_type;            //0: JL格式ui资源  1: 未经压缩打包图片
    u8 state;               //标识是否需要隐藏该控件，1：隐藏，0：不隐藏
    u8 act_flag;            //标识是否需要更新
    void *user_data;        //raw data地址，暂未支持

    /* jl框架相关 */
    pImageInfo_t pic;       //控件原始信息
    ui_jlp_t jlp;           //分块解压缩相关
    uint16_t num;           //标识该图片控件需要显示的图片列表里的哪张图片

    Rect_t rect;            //坐标信息，用于移动控件
    struct Node_img *next;
} ui_img_t;

typedef u32 obj_id_t;  		//控件id变量类型重定义
typedef ui_img_t ui_page_t; //页面管理指针变量重定义

/* 显存结构体定义 */
typedef struct disp_drv_t {
    u8 bulk_index;                  //标识显存块序号
    u8 bulk_num;                    //标识显存块总数
    u8 refr_line;                   //刷新行数
    u16 disp_width;                 //lcd屏幕水平分辨率
    u16 disp_height;                //lcd屏幕垂直分辨率
    u32 buf_offset;                 //显存buffer偏移量
    ui_disp_draw_buf_t *buf;        //显存buffer地址
    ui_color_t bg_color;            //背景颜色
    ui_img_t *disp_page;            //需要显示的page，指向img控件头节点
    volatile u8 flushing;		    //1: 是否有更新disp buf
    int (*SetDrawArea)(int xs, int xe, int ys, int ye);  //显示区域接口
    int (*Draw)(char *buf, u32 len, u8 wait);    		 //推屏接口
} disp_drv_t;

#ifndef ARRAY_LEN
#define ARRAY_LEN(array)	(sizeof(array) / sizeof(array[0]))
#endif

#ifndef abs
#define abs(x)  ((x)>0?(x):-(x) )
#endif

void ui_disp_driver_init(void);

int ui_obj_img_add(ui_img_t **head, obj_id_t id);

void ui_obj_img_del(ui_img_t **head, obj_id_t id);

disp_drv_t *ui_get_disp_drv(void);

#endif
