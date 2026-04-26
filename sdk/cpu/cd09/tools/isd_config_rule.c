#include "app_config.h"

#define _CAT2(a,b) a ## _ ## b
#define CAT2(a,b) _CAT2(a,b)

#define _CAT3(a,b,c) a ## _ ## b ##  _  ## c
#define CAT3(a,b,c) _CAT3(a,b,c)

#define _CAT4(a,b,c,d) a ## _ ## b ##  _  ## c ## _ ## d
#define CAT4(a,b,c,d) _CAT4(a,b,c,d)

#ifndef CONFIG_DOWNLOAD_MODEL
#define CONFIG_DOWNLOAD_MODEL                   USB                   //下载模式选择，可选配置USB\SERIAL
#endif

#ifndef CONFIG_DEVICE_NAME
#define CONFIG_DEVICE_NAME                      JlVirtualJtagSerial   //串口通讯的设备名(配置串口通讯时使用)
#endif

#ifndef CONFIG_SERIAL_BAUD_RATE
#define CONFIG_SERIAL_BAUD_RATE                 1000000               //串口通讯的波特率(配置串口通讯时使用)
#endif

#ifndef CONFIG_SERIAL_CMD_OPT
#define CONFIG_SERIAL_CMD_OPT                   2                     //串口通讯公共配置参数(配置串口通讯时使用)
#endif

#ifndef CONFIG_SERIAL_CMD_RATE
#define CONFIG_SERIAL_CMD_RATE                  100                   //串口通讯时公共配置参数(配置串口通讯时使用)[n*10000]
#endif

#ifndef CONFIG_SERIAL_CMD_RES
#define CONFIG_SERIAL_CMD_RES                   0                     //串口通讯时公共配置参数(配置串口通讯时使用)
#endif

#ifndef CONFIG_SERIAL_INIT_BAUD_RATE
#define CONFIG_SERIAL_INIT_BAUD_RATE            9600                  //串口通信初始化时通讯的波特率(配置串口通讯时使用)
#endif

#ifndef CONFIG_LOADER_BAUD_RATE
#define CONFIG_LOADER_BAUD_RATE                 1000000               //写入loader文件时通讯的波特率(配置串口通讯时使用)
#endif

#ifndef CONFIG_LOADER_ASK_BAUD_RATE
#define CONFIG_LOADER_ASK_BAUD_RATE             1000000
#endif

#ifndef CONFIG_SERIAL_SEND_KEY
#define CONFIG_SERIAL_SEND_KEY                  YES                   //SERIAL_SEND_KEY：串口交互时数据是否需要进行加密(配置串口通讯时使用，有效值：YES)
#endif

#ifndef CONFIG_BREFORE_LOADER_WAIT_TIME
#define CONFIG_BREFORE_LOADER_WAIT_TIME         150                   //写入loader前延时时间(配置串口通讯时使用)
#endif

#ifndef CONFIG_ENTRY_ADDRESS
#define CONFIG_ENTRY_ADDRESS                    0x4000100             //程序入口地址，一般不需要修改(跟张恺讨论过把RESERVED_OPT=0合并到一个配置项)
#endif

#ifndef CONFIG_SPI_DATA_WIDTH
#define CONFIG_SPI_DATA_WIDTH                   2                     //data_width[0 1 2 3 4] 3的时候uboot自动识别2或者4线
#endif

#ifndef CONFIG_SPI_CLK_DIV
#define CONFIG_SPI_CLK_DIV                      3                     //clk [0-255]
#endif

//mode:
//	  0 RD_OUTPUT,		 1 cmd 		 1 addr
//    1 RD_I/O,   		 1 cmd 		 x addr
//	  2 RD_I/O_CONTINUE] no_send_cmd x add
#ifndef CONFIG_SPI_MODE
#define CONFIG_SPI_MODE                         0
#endif

//port:
//	  0  优先选A端口
//	  1  优先选B端口
#ifndef CONFIG_SPI_PORT
#define CONFIG_SPI_PORT                         0
#endif

//uboot and ota.bin串口tx
#ifndef CONFIG_UBOOT_DEBUG_PIN
#define CONFIG_UBOOT_DEBUG_PIN                  PA04
#endif

//uboot and ota.bin串口波特率
#ifndef CONFIG_UBOOT_DEBUG_BAUD_RATE
#define CONFIG_UBOOT_DEBUG_BAUD_RATE            1000000
#endif

#ifndef CONFIG_MCLR_EN
#define CONFIG_MCLR_EN                          YES
#endif

#ifndef CONFIG_PINR_RESET_EN
#define CONFIG_PINR_RESET_EN                    YES
#endif

[EXTRA_CFG_PARAM]
#if TCFG_DUAL_BANK_ENABLE
BR22_TWS_DB = YES;
FLASH_SIZE = CONFIG_FLASH_SIZE;
BR22_TWS_VERSION = 0;
#if CONFIG_DB_UPDATE_DATA_GENERATE_EN
DB_UPDATE_DATA = YES;
#endif
#else
NEW_FLASH_FS = YES;
#endif
CHIP_NAME = AC82N;
ENTRY = CONFIG_ENTRY_ADDRESS;
PID = AC82N;
VID = 0.01;
RESERVED_OPT = 0;
OTP_CFG_SIZE = 512;
CHECK_OTA_BIN = NO;
DOWNLOAD_MODEL = CONFIG_DOWNLOAD_MODEL;
SERIAL_DEVICE_NAME = CONFIG_DEVICE_NAME;
SERIAL_BARD_RATE = CONFIG_SERIAL_BAUD_RATE;
SERIAL_CMD_OPT = CONFIG_SERIAL_CMD_OPT;
SERIAL_CMD_RATE = CONFIG_SERIAL_CMD_RATE;
SERIAL_CMD_RES = CONFIG_SERIAL_CMD_RES;
SERIAL_INIT_BAUD_RATE = CONFIG_SERIAL_INIT_BAUD_RATE;
LOADER_BAUD_RATE = CONFIG_LOADER_BAUD_RATE;
LOADER_ASK_BAUD_RATE = CONFIG_LOADER_ASK_BAUD_RATE;
BEFORE_LOADER_WAIT_TIME = CONFIG_BREFORE_LOADER_WAIT_TIME;
SERIAL_SEND_KEY = CONFIG_SERIAL_SEND_KEY;
SPECIAL OPT = 0;
FORCE_4K_ALIGN = YES;
NEED_RESERVED_AREA = NO;

[BURNER_OPTIONS]
LONG_RESET_EN = CONFIG_PINR_RESET_EN;
SHORT_RESET_EN = CONFIG_MCLR_EN;

[BURNER_PASSTHROUGH_CFG]
FLASH_WRITE_PROTECT = YES;

// #####匹配的芯片版本,请勿随意改动######
[CHIP_VERSION]
SUPPORTED_LIST = A, B;

// #####UBOOT配置项，请勿随意调整顺序####
[SYS_CFG_PARAM]
SPI = CAT4(CONFIG_SPI_DATA_WIDTH, CONFIG_SPI_CLK_DIV, CONFIG_SPI_MODE, CONFIG_SPI_PORT);
LRC = -1;
#ifdef CONFIG_UBOOT_DEBUG_PIN
UTTX = CONFIG_UBOOT_DEBUG_PIN; //uboot串口tx
UTBD = CONFIG_UBOOT_DEBUG_BAUD_RATE; //uboot串口波特率
#endif


#ifndef CONFIG_VM_ADDR
#define CONFIG_VM_ADDR		0
#endif

#if TCFG_VM_SIZE
#define __VM_SIZE(size)     size##K
#define _VM_SIZE(size)      __VM_SIZE(size)
#define CONFIG_VM_LEAST_SIZE	_VM_SIZE(TCFG_VM_SIZE)
#endif

#ifndef CONFIG_VM_LEAST_SIZE
#define CONFIG_VM_LEAST_SIZE	8K
#endif

#ifndef CONFIG_VM_OPT
#define CONFIG_VM_OPT       1
#endif

#ifndef CONFIG_BTIF_ADDR
#define CONFIG_BTIF_ADDR	AUTO
#endif

#ifndef CONFIG_BTIF_LEN
#define CONFIG_BTIF_LEN		0x1000
#endif

#ifndef CONFIG_BTIF_OPT
#define CONFIG_BTIF_OPT		1
#endif

#ifndef CONFIG_PRCT_ADDR
#define CONFIG_PRCT_ADDR	0
#endif

#ifndef CONFIG_PRCT_LEN
#define CONFIG_PRCT_LEN		CODE_LEN
#endif

#ifndef CONFIG_PRCT_OPT
#define CONFIG_PRCT_OPT		2
#endif

#ifndef CONFIG_BURNER_INFO_SIZE
#define CONFIG_BURNER_INFO_SIZE		32
#endif

#ifndef TCFG_MSFC_DEV_ENABLE
#define TCFG_MSFC_DEV_ENABLE        0
#endif

// ########flash空间使用配置区域###############################################
// #PDCTNAME:    产品名，对应此代码，用于标识产品，升级时可以选择匹配产品名
// #BOOT_FIRST:  1=代码更新后，提示APP是第一次启动；0=代码更新后，不提示
// #UPVR_CTL：   0：不允许高版本升级低版本   1：允许高版本升级低版本
// #XXXX_ADR:    区域起始地址	AUTO：由工具自动分配起始地址
// #XXXX_LEN:    区域长度		CODE_LEN：代码长度
// #XXXX_OPT:    区域操作属性
// #
// #
// #
// #操作符说明  OPT:
// #	0:  下载代码时擦除指定区域
// #	1:  下载代码时不操作指定区域
// #	2:  下载代码时给指定区域加上保护
// ############################################################################
[RESERVED_CONFIG]

//for protect area cfg
PRCT_ADR = CONFIG_PRCT_ADDR;
PRCT_LEN = CONFIG_PRCT_LEN;
PRCT_OPT = CONFIG_PRCT_OPT;

//for volatile memory area cfg
VM_ADR = CONFIG_VM_ADDR;
VM_LEN = CONFIG_VM_LEAST_SIZE;
VM_OPT = CONFIG_VM_OPT;

#if (TCFG_DUAL_BANK_ENABLE && TCFG_DB_UPDATE_PASSIVE_DEMO_EN)
#if CONFIG_DB_UPDATE_DATA_GENERATE_EN
UPDATA_FILE = db_update_data.bin;
#endif
UPDATA_ADR = CONFIG_UPDATA_INFO_ADD;
UPDATA_LEN = CONFIG_UPDATA_INFO_LEN;
UPDATA_OPT = CONFIG_UPDATA_INFO_OPT;
#endif

#if TCFG_MSFC_DEV_ENABLE && TCFG_USB_SLAVE_MSD_ENABLE
//for insidef flash dev area cfg
#define __MSFC_SIZE(size)       size##K
#define _MSFC_SIZE(size)        __MSFC_SIZE(size)
#define CONFIG_MSFC_ADDR        _MSFC_SIZE(TCFG_MSFC_DEV_ADDR)
#define CONFIG_MSFC_SIZE        _MSFC_SIZE(TCFG_MSFC_DEV_SIZE)
MSFC_ADR = CONFIG_MSFC_ADDR;
MSFC_LEN = CONFIG_MSFC_SIZE;
MSFC_OPT = 1;
#endif

[BURNER_CONFIG]
SIZE = CONFIG_BURNER_INFO_SIZE;

