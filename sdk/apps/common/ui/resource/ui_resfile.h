#ifndef RESFILE_H
#define RESFILE_H

#include "typedef.h"
#include "fs/fs.h"

struct image_file {
    u8 format;
    u8 compress;
    u16 id;
    u16 data_crc;
    u16 width;
    u16 height;
    u16 version;
    u32 offset;
    u32 len;
};

typedef struct {
    u8  magic[4];       //'R', 'U', '2', 1
    u16 version;
    u16 bPanelType;
    u16 totalPage;
    u16 reserved;
    u32 resver;
} RES_HEAD_T; //12字节

typedef struct {
    u32 pageNum;
    u32 pageAddr;
} RES_PAGE_T;

/*资源类型索引表目录项的数据结构*/
typedef struct {
    u32 dwOffset;       //资源内容索引表的偏移
    u16 wCount; 	    //资源类型总数
    u8  bItemType;      //'P'--PIC Table,'S'--String Table,'X' -- XML File
    u8  langsum;
    u32 language;
} RES_ENTRY_T; //8字节

typedef struct {
    u16 head_crc;
    u16 data_crc;
    u16 res_type;
    u16 typeId;         //压缩方式(3bits)|类型(3bits)|图片ID(10bits)
    u16 wWidth;         //图片宽度(以像素为单位)
    u16 wHeight;        //图片高度(以像素为单位)
    u32 dwLength;       //图片数据大小,最大4G,4 bytes
    u32 dwOffset;       //数据区在文件内偏移,4 bytes
} RES_BMP_T; //20字节

extern FILE *res_file;
int open_resfile(const char *name);
int Rle_Decode(u8 *inbuf, int inSize, u8 *outbuf, int onuBufSize, int offset, int len, int pixel_size);

#endif

