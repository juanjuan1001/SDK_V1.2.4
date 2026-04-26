#ifndef __SYS_DEBUG_H__
#define __SYS_DEBUG_H__

void debug_init();
void exception_analyze();

/********************************** DUBUG SFR *****************************************/
#define DBG_REV             0x00
#define DBG_AUDIO           0x03
#define DBG_APA             0x04
#define DBG_ISP             0x05
#define DBG_SBC             0x09
#define DBG_HADC            0x0b
#define DBG_SD0             0x0c
#define DBG_SPI0            0x0e
#define DBG_SPI1            0x0f
#define DBG_SPI2            0x10
#define DBG_UART0           0x11
#define DBG_UART1           0x12
#define DBG_UART2           0x13
#define DBG_LEDC            0x14
#define DBG_GPADC           0x15
#define DBG_PAP             0x16
#define DBG_CTM             0x17
#define DBG_ANC             0x18
#define DBG_IIC        		0x1A
#define DBG_TDM          	0x1C

#define DBG_Q32S_IF         0x20
#define DBG_Q32S_RW         0x21

#define DBG_AXI_00          0x80
#define DBG_AXI_1F          0x9f
#define DBG_USB             0xa0
#define DBG_FM              0xa1
#define DBG_BT              0xa2
#define DBG_EQ              0xa4
#define DBG_SRC             0xa5
#define DBG_AES             0xa9

#define DBG_CPU0_WR         0xf0
#define DBG_CPU0_RD         0xf1
#define DBG_CPU0_IF         0xf2
#define DBG_CPU1_WR         0xf3
#define DBG_CPU1_RD         0xf4
#define DBG_CPU1_IF         0xf5
#define DBG_CPU2_WR         0xf6
#define DBG_CPU2_RD         0xf7
#define DBG_CPU2_IF         0xf8
#define DBG_CPU3_WR         0xf9
#define DBG_CPU3_RD         0xfa
#define DBG_CPU3_IF         0xfb

#define DBG_SDTAP           0xff
#define MSG_NULL            0xff


/* ---------------------------------------------------------------------------- */
/**
 * @brief Memory权限保护设置
 *
 * @param idx: 保护框索引, 范围: 0 ~ 3, 目前系统默认使用0和3, 用户可用1和2
 * @param begin: Memory开始地址
 * @param end: Memory结束地址
 * @param inv: 0: 保护框内, 1: 保护框外
 * @param format: "Cxwr0rw1rw2rw3rw", CPU:外设0:外设1:外设2:外设3,
 * @param ...: 外设ID号索引, 如: DBG_EQ, 见debug.h
 */
/* ---------------------------------------------------------------------------- */
void mpu_set(int idx, u32 begin, u32 end, u32 inv, const char *format, ...);


/* ---------------------------------------------------------------------------- */
/**
 * @brief 取消指定框的mpu保护
 *
 * @param idx: 保护框索引号
 */
/* ---------------------------------------------------------------------------- */
void mpu_disable_by_index(u8 idx);


/* ---------------------------------------------------------------------------- */
/**
 * @brief :取消所有保护框mpu保护
 */
/* ---------------------------------------------------------------------------- */
void mpu_diasble(void);


#endif



