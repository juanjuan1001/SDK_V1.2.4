#ifndef __SFC_NORFLASH_API_H__
#define __SFC_NORFLASH_API_H__

#include "typedef.h"
#include "device/device.h"

struct flash_wp_arg {   //写保护配置信息
    u8 numOfwp_array;//写保护参数的个数
    u8 wp_sr1_mask; //sr1要保留或修改的bit
    u8 wp_sr2_mask; //sr2要保留或修改的bit
    struct {
        u8 wp_sr1; //写保护sr1取值
        u8 wp_sr2;//写保护sr2取值
        u16 wp_addr;//写保护结束地址,单位K
    } wp_array[32]; //写保护的组数，修改可变长
} __attribute__((packed));

struct flash_otp_arg {//flash otp信息
    u8 otp_lock_sr1_mask;//sr1要保留或修改的bit
    u8 otp_lock_sr2_mask;//sr2要保留或修改的bit
    u8 otp_lock_sr1;//otp sr1的取值
    u8 otp_lock_sr2;//otp sr2的取值
    u16 otp_NumberOfpage;//otp的page数量
    u16 otp_page_size;//otp的page大小
    u32 otp_offset[5];//otp page的偏移地址组数
} __attribute__((packed));

struct flash_io_drv {
    u8 cs_drv: 2;
    u8 clk_drv: 2;
    u8 do_drv: 2;
    u8 di_drv: 2;
    u8 d2_drv: 2;
    u8 d3_drv: 2;
    u8 input_delay: 4;
} __attribute__((packed));

struct	flash_config_info { //flash配置信息的结构体
    u8 write_en_use_50h;//写使能配置0:06H		1:50H
    u8 wr_sr_cmd[2];//寄存器的写命令，当两个命令是一样的，就使用连续写模式
    struct	flash_wp_arg wp;// flash 写保护参数
    struct	flash_otp_arg otp; //flash otp 参数
    struct  flash_io_drv drv; // flash drv 参数
} __attribute__((packed));

const void *flash_addr2cpu_addr(u32 offset);
u32 cpu_addr2flash_addr(const void *offset);

int norflash_init(const void *node, void *arg);
int norflash_open(const char *name, void **device, void *arg);
int norflash_read(void *device, void *buf, u32 len, u32 offset);
int norflash_write(void *device, const void *buf, u32 len, u32 offset);
int norflash_erase(void *device, u32 cmd, u32 addr);
int norflash_ioctl(void *device, u32 cmd, u32 arg);

int msfc_norflash_init(const void *node, void *arg);
int msfc_norflash_ioctl(void *device, u32 cmd, u32 arg);
int msfc_norflash_read(void *device, void *buf, u32 len, u32 offset);
int msfc_norflash_erase(u32 cmd, u32 addr);
int msfc_norflash_write(void *device, const void *buf, u32 len, u32 offset);
void msfc_norflash_entry_sleep(void *device);
void msfc_norflash_exit_sleep(void *device);
void msfc_norflash_enter_4byte_addr();
u8 *msfc_get_norflash_uuid(void);
void sfc_drop_cache(void *ptr, u32 len);
void otp_api_verify();
void doe(u16 k, void *pBuf, u32 lenIn, u32 addr);


const void *flash_addr2cpu_addr(u32 offset);
u32 cpu_addr2flash_addr(const void *offset);
int norflash_origin_read(void *device, u8 *buf, u32 offset, u32 len);
void norflash_entry_sleep(struct device *device);
void norflash_exit_sleep(struct device *device);
const u8 *get_norflash_uuid(void);//从缓存读取uuid
u32 norflash_get_uuid(void *device, u8 *uuid);//从硬件读取uuid，需要开关中断
int votp_read(u32 id, void *buff, u32 len);
int votp_get(u32 id, const void **p, u32 len);
int votp_write(u32 id, const void *buff, u32 len);
u32 syscfg_read_otp(u32 id, u8 *buf, u32 len);
u32 syscfg_write_otp(u32 id, const u8 *buf, u16 len);
const struct flash_config_info *norflash_get_info();
int norflash_write_protect_config(struct device *device, u32 addr, const struct flash_config_info *p);
int norflash_set_write_protect(u32 enable, u32 wp_addr);

u32 syscfg_read_otp(u32 id, u8 *buf, u32 len);
u32 syscfg_write_otp(u32 id, const u8 *buf, u16 len);


//device 设备接口
struct msfc_dev_platform_data {
    u32 start_addr;         //分区起始地址
    u32 size;               //分区大小
};

#endif

