#include "ui_image.h"
#include "ui_resfile.h"
#include "lcd_drive.h"
#include "fs/sdfile.h"
#include "asm/crc16.h"

static const char *image_path = (char *)"mnt/sdfile/res/JL.res";

static int open_image_by_id(struct image_file *f, int id, int page_num)
{
    RES_HEAD_T head ALIGNED(4);
    RES_ENTRY_T entry ALIGNED(4);
    RES_BMP_T res_pic ALIGNED(4);
    RES_PAGE_T page ALIGNED(4);
    FILE *file;
    u16 head_crc;
    int offset;
    int retry = 3;

    file = res_file;
    if (!file) {
        return -EINVAL;
    }

    do {
        sdfile_seek(file, 0, SEEK_SET);
        sdfile_read(file, &head, sizeof(RES_HEAD_T));
        f->version = head.magic[2] << 8 | head.magic[3];

        offset = sizeof(RES_HEAD_T) + sizeof(RES_PAGE_T) * page_num;
        sdfile_seek(file, offset, SEEK_SET);
        sdfile_read(file, &page, sizeof(RES_PAGE_T));

        offset = page.pageAddr + sizeof(RES_ENTRY_T);
        sdfile_seek(file, offset, SEEK_SET);
        sdfile_read(file, &entry, sizeof(RES_ENTRY_T));

        if (id > entry.wCount) {
            return -EINVAL;
        }
        offset = entry.dwOffset + (id - 1) * sizeof(RES_BMP_T);
        sdfile_seek(file, offset, SEEK_SET);
        sdfile_read(file, &res_pic, sizeof(RES_BMP_T));

        head_crc = CRC16(&((u8 *)&res_pic)[2], sizeof(RES_BMP_T) - 2);
        if (head_crc == res_pic.head_crc) {
            f->width    = res_pic.wWidth;
            f->height   = res_pic.wHeight;
            f->format   = (res_pic.typeId >> 10) & 0x07;
            f->compress = (res_pic.typeId >> 13) & 0x07;
            f->id       = res_pic.typeId & 0x3ff;
            f->offset   = res_pic.dwOffset;
            f->len      = res_pic.dwLength;
            f->data_crc = res_pic.data_crc;

            return 0;
        }
    } while (retry-- > 0);

    return -EINVAL;
}

static int read_image_data(struct image_file *f, u8 *data, int len, int offset)
{
    FILE *file = res_file;
    sdfile_seek(file, f->offset + offset, SEEK_SET);
    if (sdfile_read(file, data, len) != len) {
        return -EFAULT;
    }

    return len;
}

static u16 get_mixed_pixel(u16 backcolor, u16 forecolor, u8 alpha)
{
    u16 mixed_color;
    u8 r0, g0, b0;
    u8 r1, g1, b1;
    u8 r2, g2, b2;

    if (alpha == 255) {
        return (forecolor >> 8) | (forecolor & 0xff) << 8;
    } else if (alpha == 0) {
        return (backcolor >> 8) | (backcolor & 0xff) << 8;
    }

    r0 = ((backcolor >> 11) & 0x1f) << 3;
    g0 = ((backcolor >> 5) & 0x3f) << 2;
    b0 = ((backcolor >> 0) & 0x1f) << 3;

    r1 = ((forecolor >> 11) & 0x1f) << 3;
    g1 = ((forecolor >> 5) & 0x3f) << 2;
    b1 = ((forecolor >> 0) & 0x1f) << 3;

    r2 = (alpha * r1 + (255 - alpha) * r0) / 255;
    g2 = (alpha * g1 + (255 - alpha) * g0) / 255;
    b2 = (alpha * b1 + (255 - alpha) * b0) / 255;

    mixed_color = ((r2 >> 3) << 11) | ((g2 >> 2) << 5) | (b2 >> 3);

    return (mixed_color >> 8) | (mixed_color & 0xff) << 8;
}

int ui_show_image_by_id(u32 id, u16 x, u16 y)
{
    struct image_file img = {0};
    u16 page_num, pic_idx;
    if (!res_file) {
        if (open_resfile(image_path)) {
            printf("resfile open faild!\n");
            return -EINVAL;
        }
    }
    page_num = id >> 16;
    pic_idx = id & 0xffff;
    /* put_buf((u8 *)(&id), 4); */
    if (open_image_by_id(&img, pic_idx, page_num)) {
        printf("open image info faild!\n");
        return -EINVAL;
    }
    u8 *data = (u8 *)zalloc(img.len);
    if (data == NULL) {
        return -ENOMEM;
    }

    if (read_image_data(&img, data, img.len, 0) != img.len) {
        printf("read image data faild!\n");
        free(data);
        return -EINVAL;
    }
    u32 alpha_offset = ((u32 *)data)[0]; //获取alpha数据偏移
    u32 rle_rgb_addr = 0;
    u16 rle_rgb_size = 0;
    u16 *rgb_buf = NULL;
    u16 rgb_size = 0;
    u16 lcd_width = lcd_get_screen_width();
    u16 lcd_height = lcd_get_screen_height();
    if (alpha_offset) { //带alpha的图片
        u32 rle_alpha_addr = (u32)data + alpha_offset + sizeof(u32) * img.height;
        u16 rle_alpha_size = img.len - alpha_offset - sizeof(u32) * img.height;
        u16 alpha_size = img.width * img.height;
        u8 *alpha_buf = (u8 *)malloc(alpha_size);
        if (alpha_buf == NULL) {
            free(data);
            return -ENOMEM;
        }
        Rle_Decode((u8 *)rle_alpha_addr, rle_alpha_size, alpha_buf, alpha_size, 0, alpha_size, 1);
        rle_rgb_addr = (u32)data + sizeof(u32) + sizeof(u32) * img.height;
        rle_rgb_size = alpha_offset - sizeof(u32) - sizeof(u32) * img.height;
        rgb_size = img.width * img.height * sizeof(u16);
        rgb_buf = (u16 *)malloc(rgb_size);
        if (rgb_buf == NULL) {
            free(data);
            free(alpha_buf);
            return -ENOMEM;
        }
        Rle_Decode((u8 *)rle_rgb_addr, rle_rgb_size, (u8 *)rgb_buf, rgb_size, 0, rgb_size, 2);
        for (int h = 0; h < img.height; h++) {
            for (int w = 0; w < img.width; w++) {
                u16 forecolor = rgb_buf[h * img.width + w];
                u16 backcolor = 0x0; //默认背景纯黑
                u8 alpha = alpha_buf[h * img.width + w];
                forecolor = (forecolor >> 8) | (forecolor << 8);
                backcolor = (backcolor >> 8) | (backcolor << 8);
                rgb_buf[h * img.width + w] = get_mixed_pixel(backcolor, forecolor, alpha);
            }
        }
        if (x + img.width > lcd_width) {
            x = lcd_width - img.width;
        }
        if (y + img.height > lcd_height) {
            y = lcd_height - img.height;
        }
        lcd_set_draw_area(x, x + img.width - 1, y, y + img.height - 1);
        lcd_draw((char *)rgb_buf, rgb_size, 0);
        free(alpha_buf);
    } else { //不带alpha的图片
        rle_rgb_addr = (u32)data + sizeof(u32) + sizeof(u32) * img.height;
        rle_rgb_size = img.len - sizeof(u32) - sizeof(u32) * img.height;
        rgb_size = img.width * img.height * sizeof(u16);
        rgb_buf = (u16 *)zalloc(rgb_size);
        if (rgb_buf == NULL) {
            free(data);
            return -ENOMEM;
        }
        Rle_Decode((u8 *)rle_rgb_addr, rle_rgb_size, (u8 *)rgb_buf, rgb_size, 0, rgb_size, 2);
        if (x + img.width > lcd_width) {
            x = lcd_width - img.width;
        }
        if (y + img.height > lcd_height) {
            y = lcd_height - img.height;
        }
        lcd_set_draw_area(x, x + img.width - 1, y, y + img.height - 1);
        lcd_draw((char *)rgb_buf, rgb_size, 0);
    }
    free(data);
    free(rgb_buf);
    return 0;
}

