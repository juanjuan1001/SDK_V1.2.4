#ifndef  __SYSCFG_ID_H__
#define  __SYSCFG_ID_H__

#include "generic/typedef.h"
#include "generic/list.h"
#include "device/ioctl_cmds.h"
#include "ioctl.h"

//=================================================================================//
//                        系统配置项(VM, INSIDEOTP,EXOTP, cfg_bin)读写接口                    //
//接口说明:                                			   						       //
// 	1.输入参数                                			     					   //
// 		1)item_id: 配置项ID号, 由本文件统一分配;                                   //
// 		2)buf: 用于存储read/write数据内容;                                    	   //
// 		3)len: buf的长度(byte), buf长度必须大于等于read/write数据长度;             //
// 	2.返回参数:                                 			     				   //
// 		1)执行正确: 返回值等于实际上所读到的数据长度(大于0);                       //
//      2)执行错误: 返回值小于等于0, 小于0表示相关错误码;                          //
// 	3.读写接口使用注意事项:             										   //
// 		1)不能在中断里调用写(write)接口;                                   		   //
// 		2)调用本读写接口时应该习惯性判断返回值来检查read/write动作是否执行正确;    //
//=================================================================================//

#define   CFG_ID_MASK           (0x00ffffff)
#define   CFG_GROUP_MASK        (0xff000000)

#define   CFG_GROUP_VOTP        (0x01<< 24)     //FLASH 虚拟OTP，通过INI文件分配大小,flash 配置了写保护后，才会变成不可擦写
#define   CFG_GROUP_OTP         (0x02<< 24)     //FLASH 提供的物理OTP区域，不同芯片容量不同
#define   CFG_GROUP_VM         	(0x03<< 24)     //FLASH 提供的VM区域

//存储在虚拟OTP区域,只能写一次
enum votp_id {
    CFG_ID_VOTP_FLASH_INFO = CFG_GROUP_VOTP,
    CFG_ID_VOTP_CALIBRATION,   //校准信息
    CFG_ID_VOTP_CALIBRATION_1, //备用校准信息1
    CFG_ID_VOTP_CALIBRATION_2, //备用校准信息2
    //user votp id
};

//存储在VM区域,可以重复读写
enum vm_id {
    VM_RTC_SYS_TIME = CFG_GROUP_VM,
    VM_PMU_VOLTAGE,
    VM_AVDDR_REF,
    VM_XOSC_HCS,

    //user vm id
    VM_OXIMETER_CFG_INFO,
    VM_FOC_CFG_LVL,
};

/* --------------------------------------------------------------------------*/
/**
 * @brief 读取对应配置项的内容
 *
 * @param  [in] item_id 配置项ID号
 * @param  [out] buf 用于存储read数据内容
 * @param  [in] len buf的长度(byte), buf长度必须大于等于read数据长度
 *
 * @return 1)执行正确: 返回值等于实际上所读到的数据长度(大于0);
 *         2)执行错误: 返回值小于等于0, 小于0表示相关错误码;
 */
/* --------------------------------------------------------------------------*/
int syscfg_read(u32 item_id, void *buf, u16 len);

int syscfg_get_ptr(u32 item_id, const void **p, u16 len);
/* --------------------------------------------------------------------------*/
/**
 * @brief 写入对应配置项的内容
 *
 * @param  [in] item_id 配置项ID号
 * @param  [in] buf 用于存储write数据内容
 * @param  [in] len buf的长度(byte), buf长度必须大于等于write数据长度
 *
 * @return 1)执行正确: 返回值等于实际上所读到的数据长度(大于0);
 *         2)执行错误: 返回值小于等于0, 小于0表示相关错误码;
 */
/* --------------------------------------------------------------------------*/
int syscfg_write(u32 item_id, const void *buf, u16 len);



#endif  /*SYSCFG_ID_H*/



