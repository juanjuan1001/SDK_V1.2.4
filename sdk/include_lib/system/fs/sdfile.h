#ifndef _SDFILE_H_
#define _SDFILE_H_

#include "typedef.h"

//=======================================================//
//                    默认宏配置定义                     //
//=======================================================//
/********* sdfile 文件头 **********/
#define SDFILE_NAME_LEN 			16

typedef struct sdfile_file_head {
    u16 head_crc;
    u16 data_crc;
    u32 addr;
    u32 len;
    u8 attr;
    u8 res;
    u16 index;
    char name[SDFILE_NAME_LEN];
} SDFILE_FILE_HEAD;

struct sdfile_dir {
    u16 file_num;
    struct sdfile_file_head head;
};

enum sdfile_err_table {
    SDFILE_DIR_NOT_EXIST = -0xFF,
    SDFILE_FILE_NOT_EXIST,
    SDFILE_MALLOC_ERR,
    SDFILE_VM_NOT_FIND,
    SDFILE_DATA_CRC_ERR,
    SDFILE_WRITE_AREA_NEED_ERASE_ERR,
    SDFILE_SUSS = 0,
    SDFILE_END,
};

typedef struct sdfile_file_t {
    u32 fptr;
    struct sdfile_file_head head;
} SDFILE, FILE;

struct vfs_attr {
    u8 attr;		//属性
    u32 fsize;		//文件大小
    u32 sclust;		//地址
};

int sdfile_init(void);
SDFILE *sdfile_open(const char *path, const char *mode);
int sdfile_read(SDFILE *fp, void *buf, u32 len);
int sdfile_write(SDFILE *fp, void *buf, u32 len);
int sdfile_seek(SDFILE *fp, u32 offset, u32 fromwhere);
int sdfile_close(SDFILE *fp);
int sdfile_pos(SDFILE *fp);
int sdfile_len(SDFILE *fp);
int sdfile_get_name(SDFILE *fp, u8 *name, int len);
int sdfile_get_attrs(SDFILE *fp, struct vfs_attr *attr);

static inline SDFILE *fopen(const char *path, const char *mode)
{
    return sdfile_open(path, mode);
}
static inline int fread(SDFILE *fp, void *buf, u32 len)
{
    return sdfile_read(fp, buf, len);
}
static inline int fwrite(SDFILE *fp, void *buf, u32 len)
{
    return sdfile_write(fp, buf, len);
}
static inline int fseek(SDFILE *fp, u32 offset, u32 fromwhere)
{
    return sdfile_seek(fp, offset, fromwhere);
}
static inline int fclose(SDFILE *fp)
{
    return sdfile_close(fp);
}
static inline int fpos(SDFILE *fp)
{
    return sdfile_pos(fp);
}
static inline int flen(SDFILE *fp)
{
    return sdfile_len(fp);
}
static inline int fget_name(SDFILE *fp, u8 *name, int len)
{
    return sdfile_get_name(fp, name, len);
}
static inline int fget_attrs(SDFILE *fp, struct vfs_attr *attr)
{
    return sdfile_get_attrs(fp, attr);
}

int sdfile_delete_data(SDFILE *fp);
#define fdel_data   sdfile_delete_data

#ifndef SDFILE_MOUNT_PATH
#define SDFILE_MOUNT_PATH     	"mnt/sdfile"
#endif /* #ifndef SDFILE_MOUNT_PATH */

#ifndef SDFILE_APP_ROOT_PATH
#define SDFILE_APP_ROOT_PATH       	SDFILE_MOUNT_PATH"/app/"  //app分区
#endif /* #ifndef SDFILE_APP_ROOT_PATH */

#ifndef SDFILE_RES_ROOT_PATH
#define SDFILE_RES_ROOT_PATH       	SDFILE_MOUNT_PATH"/res/"  //资源文件分区
#endif /* #ifndef SDFILE_RES_ROOT_PATH */

//获取flash物理大小, 单位: Byte
u32 sdfile_get_disk_capacity(void);
//flash addr  -> cpu addr
u32 sdfile_flash_addr2cpu_addr(u32 offset);
//cpu addr  -> flash addr
u32 sdfile_cpu_addr2flash_addr(u32 offset);

void sdfile_head_addr_get(char *name, u32 *addr, u32 *len);

void sdfile_get_votp_cfg_addr_len(u32 *votp_cfg_addr, u32 *votp_cfg_len);
#endif /* _SDFILE_H_ */


