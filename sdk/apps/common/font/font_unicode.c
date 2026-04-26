#include "font_textout.h"
#include "fs/sdfile.h"

#define LOG_TAG_CONST   OFF
#define LOG_TAG         "[UNICODE]"
#include "debug.h"

struct font_header {
    u8 magic[4];
    u32 version;
    u32 timestamp;
    u8 fontsize;
    u8 reserved[19];
};

typedef struct {
    u8 width;
    u8 height;
    s8 left;
    s8 top;
    u32 baseaddr;
} __attribute__((packed, aligned(1))) UnicInfo;

#define Font_NumPages 1024
#define Font_PageSize (0x10000 / Font_NumPages)

typedef struct {
    u16 charIndex;
    u8 codesMask[Font_PageSize / 8];
} Font_PageEntry_t;


bool InitFont_Unicode(struct font_info *info)
{
    struct font_header header;
    memset(&header, 0, sizeof(header));
    info->pixel.file.fd = sdfile_open((char *)info->pixel.file.name, "r");
    if (!info->pixel.file.fd) {
        log_error("unicode file not open!");
        return FALSE;
    }

    sdfile_seek(info->pixel.file.fd, 0, SEEK_SET);
    sdfile_read(info->pixel.file.fd, &header, sizeof(struct font_header));
    log_info("magic : %s\n", header.magic);
    log_info("version : 0x%x\n", header.version);
    log_info("timestamp : %d\n", header.timestamp);
    log_info("fontsize : %d\n", header.fontsize);

    if (memcmp(header.magic, "unic", 4)) {
        log_error("unicode magic not paired!");
        fclose(info->pixel.file.fd);
        info->pixel.file.fd = NULL;
        return FALSE;
    }
    info->pixel.size = header.fontsize;
    info->pixel.nbytes = info->pixel.size * ((info->pixel.size + 7) / 8);
    log_info("unicode file open succ!\n");
    return TRUE;
}


/* 获取字符索引 */
static int Font_GetCharIndex(struct font_info *info, u16 unicode)
{
    Font_PageEntry_t  pageEntry;

    u16  m = unicode / Font_PageSize;
    u16  n = unicode % Font_PageSize;
    u16  i, j;

    sdfile_seek(info->pixel.file.fd, sizeof(struct font_header) + m * sizeof(Font_PageEntry_t), SEEK_SET);
    sdfile_read(info->pixel.file.fd, &pageEntry, sizeof(Font_PageEntry_t));

    if (!(pageEntry.codesMask[n / 8] & (1 << (n % 8)))) {
        return -1;
    }

    for (i = j = 0; i < n; i++) {
        if (pageEntry.codesMask[i / 8] & (1 << (i % 8))) {
            j++;
        }
    }
    i = pageEntry.charIndex + j;
    return i;
}

/* 获取字符位图数据 */
int Font_GetCharBits(struct font_info *info, u16 unicode)
{
    int charIndex;
    u16 nbytes;

    if ((charIndex = Font_GetCharIndex(info, unicode)) < 0) {
        log_error("unicode : 0x%x, charIndex = %d, char index not exist!\n", unicode, charIndex);
        return 0;
    }
    UnicInfo codeinfo;
    /* log_info("charIndex : %d (0x%04x)\n", charIndex, charIndex); */
    sdfile_seek(info->pixel.file.fd, sizeof(struct font_header) + Font_NumPages * sizeof(Font_PageEntry_t) + charIndex * sizeof(UnicInfo), SEEK_SET);
    sdfile_read(info->pixel.file.fd, &codeinfo, sizeof(UnicInfo));
    /* log_info("code info : %d x %d, 0x%x\n", codeinfo.Width, codeinfo.Height, codeinfo.BaseAddr); */

    nbytes = codeinfo.width * ((info->pixel.size + 7) / 8);
    if (info->pixel.pixelbuf != NULL) {
        if (nbytes > info->pixel.nbytes) {
            log_error("error:pixelbuf overlay!\n");
            log_error("info.width = %d, info->ascpixel.size = %d, nbytes = %d\n", codeinfo.width, info->pixel.size, nbytes);
            free(info->pixel.pixelbuf);
            info->pixel.pixelbuf = (u8 *)malloc(nbytes);
            if (info->pixel.pixelbuf) {
                info->pixel.nbytes = nbytes;
            } else {
                log_error("pixelbuf malloc failed!");
                info->pixel.nbytes = 0;
                return 0;
            }
        }
        sdfile_seek(info->pixel.file.fd, codeinfo.baseaddr, SEEK_SET);
        sdfile_read(info->pixel.file.fd, info->pixel.pixelbuf, nbytes);
    }

    return codeinfo.width;
}


u16 TextOutW_Unicode(struct font_info *info, u8 *str, u16 len)
{
    u16 text, i;
    u16 width = 0;
    u16 xpos = 0;
    u16 ypos = 0;
    u16 pixel_size = info->pixel.size;
    info->string_width = 0;
    for (i = 0; i < len; i += 2) {
        text = (u16)str[i + info->bigendian] | ((u16)str[i + (1 - info->bigendian)] << 8);
        if (text == 0) {
            break;
        }
        width = Font_GetCharBits(info, text);
        /* log_info("text: 0x%x, width: %d\n", text, width); */
        if (text == '\n') {
            xpos = 0;
            ypos += pixel_size;
            if (ypos + pixel_size > info->text_height) {
                ypos -= pixel_size;
                break;
            }
            continue;
        }
        if (width == 0) {
            text = '_';
            width = Font_GetCharBits(info, text);
            if (width == 0) {
                continue;
            }
        }
        xpos += width;
        info->string_width += width;
        if (xpos > info->text_width) {
            if (info->flags & FONT_SHOW_MULTI_LINE) {
                xpos = 0;
                ypos += pixel_size;
                if (ypos + pixel_size > info->text_height) {
                    ypos -= pixel_size;
                    break;
                }
                i -= 2;
                continue;
            } else {
                break;
            }
        }
        if (info->flags & FONT_SHOW_PIXEL) {
            font_copy_pixbuf(info, width, pixel_size, xpos - width, ypos);
        }
    }
    info->string_height = ypos + info->pixel.size;
    return i;
}


