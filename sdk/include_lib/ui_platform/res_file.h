#ifndef __RES_FILE_H__
#define __RES_FILE_H__

#include "img_include.h"

enum {
    ERR_UNKNOWN_COLOR_FORMAT = 0,
    ERR_IMAGE_INFO = -1,
    ERR_IMAGE_NUM = -2,
    ERR_IMAGE_INDEX = -3,
};
// 通过颜色格式获取像素字节数
int get_pixel_byte_by_format(int color_format);

// 获取图片的颜色格式，rgb565 rgb888, argb8565, argb8888
// 注意：图片ID只能使用 all_image_id.h 的宏
int get_image_format_by_id(u32 id);

// 获取图片所在页面信息，page_id
// 注意：图片ID只能使用 all_image_id.h 的宏
int get_image_page_by_id(u32 id);

// 根据图片 ID 获取图片信息
// 注意：图片ID只能使用 all_image_id.h 的宏
pImageInfo_t get_image_info_by_id(u32 id);

// 通过数据源获取图片信息
// 注意：数据源ID只能使用 img_include.h 的 DATA_SOURCE_XXX 宏
pImageInfo_t get_image_info_by_source(u32 source_id);

// 获取图片位置坐标
#define get_image_x_by_info(pic)		(pic->rect.x)
#define get_image_y_by_info(pic)		(pic->rect.y)
#define get_image_width_by_info(pic)	(pic->rect.width)
#define get_image_height_by_info(pic)	(pic->rect.height)

// 获取图片控件大小
int get_image_size_by_info(pImageInfo_t pic);

// 获取图片控件区域
pRect_t get_image_rect_by_info(pImageInfo_t pic);

// 通过图片信息获取图片数量
// 注意：图片数量为1~N，图片索引为 0~(N-1)
int get_image_number_by_info(pImageInfo_t pic);

// 通过图片信息，获取图片使用列表索引值
int get_image_used_value_by_info(pImageInfo_t pic, int index);

// 通过使用的图片索引，获取图片索引信息
ImageSearch_t get_image_search_info_by_used(int offset);

// 通过图片信息，获取图片rgb数据所在地址
u32 get_image_rgb_addr_by_info(pImageInfo_t pic, int index);

// 通过图片信息，获取图片alpha数据所在地址
// 注意：如果 alpha 地址指向0x00，说明这张图片不存在 alpha 数据
u32 get_image_alpha_addr_by_info(pImageInfo_t pic, int index);

// 通过图片信息，获取图片rgb数据压缩后的大小
// 注意：这是压缩后的数据大小，不是压缩前的！这个大小跟压缩相关，无法直接算出
u32 get_image_rgb_size_by_info(pImageInfo_t pic, int index);

// 通过图片信息，获取图片alpha数据压缩后的大小
// 注意：这是压缩后的数据大小，不是压缩前的！这个大小跟压缩相关，无法直接算出
u32 get_image_alpha_size_by_info(pImageInfo_t pic, int index);

//分块解压
u32 jlp_deocde_block(u32 jlp_sign, u8 *in_data, int in_len, u8 *out_data, int out_len, int bit);

void printf_image_info_by_id(u32 id);

void image_info_register(const ImageInfo_t *image_info, u16 arr_len);

void image_used_list_register(const u16 *image_used_list, u16 arr_len);

void image_search_info_register(const u32 *image_search_info, u16 arr_len);

void all_image_data_register(const u8 *all_image_data, u32 arr_len);

void ui_res_file_printf_enable(u8 en);

#endif
