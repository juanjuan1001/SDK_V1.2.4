#ifndef __PRIVATE_jlp_DECODER_H__
#define __PRIVATE_jlp_DECODER_H__

#include "img_typedef.h"

/*
rle_decode_image 整张图片解码
data 输入待解压数据
data_len 带解压数据长度
buf 输出buf
buf_len 输出buf 大小
bit 像素位数 alpha 1, 565 2, 888 3
*/
void jlp_decode_image(u8 *data, int data_len, u8 *buf, int buf_len, int bit);

/*
rle_deocde_block 分块解码
rle_sign rle参数标志
in_data 待解码数据
in_len 待解码数据长度
out_data 输出缓存buf
out_len 输出缓存buf长度
bit 像素位数 alpha 1, 565 2, 888 3
*/
u32 jlp_deocde_block(u32 jlp_sign, u8 *in_data, int in_len, u8 *out_data, int out_len, int bit);

#endif
