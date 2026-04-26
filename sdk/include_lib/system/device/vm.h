#ifndef _VM_H_
#define _VM_H_

#include "generic/typedef.h"
#include "generic/list.h"
#include "device/ioctl_cmds.h"
#include "ioctl.h"

#define IOCTL_SET_VM_INFO               _IOW('V', 1, 1)
#define IOCTL_GET_VM_INFO               _IOW('V', 2, 1)

typedef enum _FLASH_ERASER {
    CHIP_ERASER = 0,
    BLOCK_ERASER,
    SECTOR_ERASER,
    PAGE_ERASER,
} FLASH_ERASER;

// VM define and api
typedef u16 vm_hdl;

struct vm_table {
    u16  index;
    u16 value_byte;
    int value;      //cache value which value_byte <= 4
};

typedef enum _vm_err {
    VM_ERR_NONE = 0,
    VM_INDEX_ERR = -0x100, //-256
    VM_INDEX_EXIST,     //0xFF -255
    VM_DATA_LEN_ERR,    //0xFE -254
    VM_READ_NO_INDEX,   //0xFD -253
    VM_READ_DATA_ERR,   //0xFC -252
    VM_WRITE_OVERFLOW,  //0xFB -251
    VM_NOT_INIT,        //-250
    VM_INIT_ALREADY,    //-249
    VM_DEFRAG_ERR,      //-248
    VM_ERR_INIT,        //-247
    VM_ERR_PROTECT      //-246
} VM_ERR;

int syscfg_vm_init(u8 fast);

// vm api
/**@brief  VM区域擦除函数
  @param  void
  @note   VM_ERR VM_ERR vm错误码
 */
VM_ERR vm_eraser(void);

/**
 * @brief VM初始化函数，所有VM操作之前都必须先初始化
 *
 * @param  [in] dev_hdl NULL
 * @param  [in] vm_addr VM起始地址
 * @param  [in] vm_len VM总大小空间
 * @param  [in] vm_mode 设置最小操作单元，目前支持按sector和按page写入
 * @param  [in] fast 快速初始化
 *
 * @return VM_ERR vm错误码
 */
VM_ERR vm_init(void *dev_hdl, u32 vm_addr, u32 vm_len, u8 vm_mode, u8 fast);

/*----------------------------------------------------------------------------*/
/**@brief  VM_WARNING_LINE检测函数
  @param  void：默认检测单前使用区域
  @return FALSE：未到达警告线，TRUE：以到达警告
  @note   用于整理VM前检测
 */
/*----------------------------------------------------------------------------*/
bool vm_warning_line_check(void);

/*----------------------------------------------------------------------------*/
/**@brief  VM检测是否到达百分比警告线，到达则整理VM区域，不到达则清除一个flash最小操作单元
  @param  level：0：查询VM是否需要整理，并进行一次擦除vm未使用区域的操作 1：整理全VM区域
  @return NON
  @note   当调用vm_write写入的累计内容到达百分比警告线时会触发vm整理区域机制，会长时间操作flash，因此出现系统卡顿现象。虽然vm写操作会擦除一次最小flash操作单元，但不一定能保证在vm已经使用区域大小到达警告线的时候可以完全擦除vm的未使用区域，所以外部app应该在实现流程不忙碌的时候调用vm_check_all(0)参数查询VM是否需要整理，并进行一次擦除操作。
 */
void vm_check_all(u8 level);    //level : default 0

/*----------------------------------------------------------------------------*/
/**@brief  VM模块重置，擦除掉vm占用的flash的内容，耗时较长。
  @return VM_ERR vm错误码
  @note   用于整理VM前检测
 */
/*----------------------------------------------------------------------------*/
VM_ERR vm_reset(void);

/**
 * @brief VM打开
 *
 * @param  [in] index 打开的序号
 *
 * @return 读写句柄
 */
s32 vm_open(u16 index);

/**
 * @brief VM读接口
 *
 * @param  [in] hdl 打开的句柄
 * @param  [in] data_buf 缓存
 * @param  [in] len 读取的长度
 *
 * @return VM_ERR vm错误码
 */
s32 vm_read(vm_hdl hdl, u8 *data_buf, u16 len);
/**
 * @brief VM写接口
 *
 * @param  [in] hdl 打开的句柄
 * @param  [in] data_buf 缓存
 * @param  [in] len 写入的长度
 *
 * @return VM_ERR vm错误码
 */
s32 vm_write(vm_hdl hdl, u8 *data_buf, u16 len);

bool sfc_erase_zone(u32 addr, u32 len);
bool sfc_erase(FLASH_ERASER cmd, u32 addr);
u32 sfc_write(const u8 *buf, u32 addr, u32 len);
u32 sfc_read(u8 *buf, u32 addr, u32 len);

//虚拟otp区域访问接口
struct otp_cfg_item {
    u32 crc16_low: 8;
    u32 id: 12;
    u32 len: 12;
    u8 data[0];
} __attribute__((packed));

int votp_read(u32 id, void *buff, u32 len);
int votp_get_ptr(u32 id, const void **p, u32 len);
int votp_write(u32 id, const void *buff, u32 len);

#endif  //_VM_H_

