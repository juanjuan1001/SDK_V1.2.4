#ifndef __PRIVATE_ALL_IMAGE_INFO_H__
#define __PRIVATE_ALL_IMAGE_INFO_H__


typedef struct rect {
    int x;
    int y;
    int width;
    int height;
} Rect_t, *pRect_t;


typedef struct image_info {
    u32 id;
    Rect_t rect;
    u16 number;
    u16 source;
    u16 play_mode;
    u16 interval;
    u32 offset;
} ImageInfo_t, *pImageInfo_t;


typedef struct image_search {
    u32 pixel: 32;
    u32 alpha: 32;
    u32 width: 16;
    u32 height: 16;
    u32 format: 8;
    u32 size: 24;
} ImageSearch_t, *pImageSearch_t;




#endif
