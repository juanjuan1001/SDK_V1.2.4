#ifndef APP_CONFIG_H
#define APP_CONFIG_H

//*********************************************************************************//
//                                 配置开始                                        //
//*********************************************************************************//
#define ENABLE_THIS_MOUDLE					1
#define DISABLE_THIS_MOUDLE					0

#define ENABLE								1
#define DISABLE								0

#define NO_CONFIG_PORT						(-1)

/* 系统打印总开关 */
#define CONFIG_DEBUG_ENABLE

//*********************************************************************************//
//                                 UART配置                                        //
//*********************************************************************************//
#define TCFG_DB_UART_ENABLE					ENABLE_THIS_MOUDLE     //打印串口使能
#define TCFG_DB_UART_RX_PORT			    NO_CONFIG_PORT          //串口接收脚配置（用于打印可以选择NO_CONFIG_PORT）
#define TCFG_DB_UART_TX_PORT  			    IO_PORTA_04             //串口发送脚配置
#define TCFG_DB_UART_BAUDRATE  			    1000000                 //串口波特率配置

//*********************************************************************************//
//                                 VM 配置                                         //
//*********************************************************************************//
#define TCFG_VM_SIZE                        16                      //n kbyte
#define TCFG_VM_INIT_FAST                   0                       //VM快速初始化

//*********************************************************************************//
//                                 USB 配置                                        //
//*********************************************************************************//
#define TCFG_PC_ENABLE						ENABLE_THIS_MOUDLE//PC模块使能
#define TCFG_UDISK_ENABLE				    DISABLE_THIS_MOUDLE//U盘模块使能
#define TCFG_OTG_USB_DEV_EN                 BIT(0)//USB0 = BIT(0)  USB1 = BIT(1)

#undef USB_DEVICE_CLASS_CONFIG
#define USB_DEVICE_CLASS_CONFIG             (SPEAKER_CLASS|HID_CLASS|CDC_CLASS)
#undef USB_MALLOC_ENABLE
#define USB_MALLOC_ENABLE                   ENABLE_THIS_MOUDLE

//*********************************************************************************//
//                                 时钟配置                                        //
//*********************************************************************************//
#define TCFG_CLOCK_PLL_HZ	                (480 * 1000000L)        //系统PLL时钟设置 240M or 480M
#define TCFG_CLOCK_SYS_HZ                   (96  * 1000000L)        //默认系统时钟设置(CPU运行时钟)
#define TCFG_CLOCK_LSB_HZ                   (48  * 1000000L)        //默认LSB时钟设置(一般外设时钟)

//*********************************************************************************//
//                                 key 配置                                        //
//*********************************************************************************//
#define KEY_IO_NUM_MAX						3
#define KEY_AD_NUM_MAX						10
#define KEY_IR_NUM_MAX						21
#define KEY_TOUCH_NUM_MAX					3
//*********************************************************************************//
//                                 iokey 配置                                      //
//*********************************************************************************//
#define TCFG_IOKEY_ENABLE					DISABLE_THIS_MOUDLE //是否使能IO按键
#define TCFG_IOKEY_0_CONNECT_WAY            ONE_PORT_TO_LOW     //按键一端接低电平一端接IO
#define TCFG_IOKEY_0_ONE_PORT               IO_PORTA_03         //IO按键端口
#define TCFG_IOKEY_1_CONNECT_WAY            ONE_PORT_TO_LOW     //按键一端接低电平一端接IO
#define TCFG_IOKEY_1_ONE_PORT               IO_PORTA_06         //IO按键端口
#define TCFG_IOKEY_2_CONNECT_WAY            ONE_PORT_TO_LOW     //按键一端接低电平一端接IO
#define TCFG_IOKEY_2_ONE_PORT               IO_PORTA_07         //IO按键端口

//*********************************************************************************//
//                                 adkey 配置                                      //
//*********************************************************************************//
#define TCFG_ADKEY_ENABLE                   ENABLE_THIS_MOUDLE  //是否使能AD按键
#define TCFG_ADKEY_PORT                     IO_PORTA_00         //AD按键端口(需要注意选择的IO口是否支持AD功能)
#define TCFG_ADKEY_AD_CHANNEL               AD_CH_PA0
#define TCFG_ADKEY_EXTERN_UP_ENABLE         ENABLE_THIS_MOUDLE  //是否使用外部上拉
#if TCFG_ADKEY_EXTERN_UP_ENABLE
#define R_UP                                220                 //22K，外部上拉阻值在此自行设置
#else
#define R_UP                                100                 //10K，内部上拉默认10K
#endif

//*********************************************************************************//
//                                 irkey 配置                                      //
//*********************************************************************************//
#define TCFG_IRKEY_ENABLE                   DISABLE_THIS_MOUDLE //是否使能ir按键
#define TCFG_IRKEY_PORT                     IO_PORTA_02         //IR按键端口

//*********************************************************************************//
//                             tocuh key 配置                                      //
//*********************************************************************************//
#define TCFG_TOUCH_KEY_ENABLE 				DISABLE_THIS_MOUDLE	//是否使能触摸按键
#define TCFG_TOUCH_KEY0_PRESS_DELTA	   	    100//变化阈值，当触摸产生的变化量达到该阈值，则判断被按下，每个按键可能不一样，可先在驱动里加到打印，再反估阈值
#define TCFG_TOUCH_KEY0_PORT 				IO_PORTA_08 //触摸按键key0 IO配置
#define TCFG_TOUCH_KEY0_VALUE 				0x12 		//触摸按键key0 按键值
//key1配置
#define TCFG_TOUCH_KEY1_PRESS_DELTA	   	    100//变化阈值，当触摸产生的变化量达到该阈值，则判断被按下，每个按键可能不一样，可先在驱动里加到打印，再反估阈值
#define TCFG_TOUCH_KEY1_PORT 				IO_PORTA_09 //触摸按键key1 IO配置
#define TCFG_TOUCH_KEY1_VALUE 				0x34        //触摸按键key1 按键值


//*********************************************************************************//
//                                 LCD SPI1 配置                                   //
//*********************************************************************************//
#define TCFG_HW_SPI1_PORT_CLK               IO_PORTC_04
#define TCFG_HW_SPI1_PORT_DO                IO_PORTC_05
#define TCFG_HW_SPI1_PORT_DI                NO_CONFIG_PORT
#define TCFG_HW_SPI1_PORT_D2                NO_CONFIG_PORT
#define TCFG_HW_SPI1_PORT_D3                NO_CONFIG_PORT
#define TCFG_HW_SPI1_BAUD	    	        24000000L
#define TCFG_HW_SPI1_MODE		            SPI_MODE_BIDIR_1BIT
#define TCFG_HW_SPI1_ROLE		            SPI_ROLE_MASTER

//*********************************************************************************//
//                                 FLASH SPI2 配置                                 //
//*********************************************************************************//
#define TCFG_NORFLASH_DEV_ENABLE            DISABLE_THIS_MOUDLE
#define TCFG_HW_SPI2_PORT_CLK               IO_PORTA_10
#define TCFG_HW_SPI2_PORT_DO                IO_PORTA_09
#define TCFG_HW_SPI2_PORT_DI                IO_PORTA_11
#define TCFG_HW_SPI2_PORT_CS				IO_PORTA_12
#define TCFG_HW_SPI2_BAUD	    	        1000000L
#define TCFG_HW_SPI2_MODE		            SPI_MODE_BIDIR_1BIT
#define TCFG_HW_SPI2_ROLE		            SPI_ROLE_MASTER

//*********************************************************************************//
//                                 SD卡 配置                                       //
//*********************************************************************************//
#define TCFG_SD0_ENABLE                     0
#define TCFG_SD1_ENABLE                     0

//*********************************************************************************//
//                                 内置FLASH设备配置                               //
//*********************************************************************************//
//描述:配合USB的MSD可将内部flash的部分区域枚举成为U盘
#define TCFG_MSFC_DEV_ENABLE                DISABLE_THIS_MOUDLE
#define TCFG_MSFC_DEV_ADDR                  384         //枚举成msd的地址(Kbyte)
#define TCFG_MSFC_DEV_SIZE                  124         //枚举成msd的大小(Kbyte)

//*********************************************************************************//
//                             LOWPOWER 配置                                       //
//*********************************************************************************//
#define TCFG_LOWPOWER_LOWPOWER_SEL          SLEEP_EN
#define TCFG_VDDIOM_LEVEL                   VDDIOM_VOL_33V//VDDIO配置

/*---------Power Wakeup IO------------------*/
#define POWER_WAKEUP_IO                     IO_PORTA_02
#define POWER_WAKEUP_EDGE                   RISING_EDGE

#define TCFG_SFC_FAST_BOOT_ENABLE			DISABLE

//*********************************************************************************//
//                             长按复位 配置                                       //
//*********************************************************************************//
#define PINR_RESET_IO                       IO_PORTA_00
#define PINR_RESET_LVL                      0 // 0下降沿 1上升沿
#define PINR_PRESS_TIME						8 // 0/1/2/4/8/16 秒
#define PINR_RESET_RELASE					1 // 0释放后复位 1立即复位
#define PINR_PROTECT_EN						0 // 写保护使能

//*********************************************************************************//
//                             供电电源 配置                                       //
//*********************************************************************************//
//干电池标压1.5V，出厂电压为1.6V多一些，关断电压为0.9V(保守一点用1.0V)
#define BATTERY_TYPE_2_DRY_DELL				(0) //2节干电池(3.2V)，放电管理
#define BATTERY_TYPE_3_DRY_DELL				(1) //3节干电池(4.8V)，放电管理
#define BATTERY_TYPE_4_DRY_DELL				(2) //4节干电池(6.4V)，放电管理
#define BATTERY_TYPE_LITHIUM_DELL			(3) //锂电池(4.2V)，充放电管理
#define TCFG_BATTERY_TYPE_SEL         		BATTERY_TYPE_3_DRY_DELL
#define TCFG_BAT_DET_IO                     NO_CONFIG_PORT //检测电池的IO
//若TCFG_BAT_DET_IO为NO_CONFIG_PORT，则TCFG_BAT_AD_CHANNEL默认配置为AD_ANA_PMU_CH_VPWR_DIV_4
//若TCFG_BAT_DET_IO为实际IO（例如IO_PORT_PA0），则TCFG_BAT_AD_CHANNEL需配置为对应AD通道（如AD_CH_PA0）
#define TCFG_BAT_AD_CHANNEL                 AD_ANA_PMU_CH_VPWR_DIV_4 //电池检测的AD通道
#if (TCFG_BATTERY_TYPE_SEL == BATTERY_TYPE_LITHIUM_DELL)
#define TCFG_CHARGE_DET_IO					IO_PORTA_06 //USB插入检测io
#define TCFG_CHARGE_FULL_IO					IO_PORTA_09 //充满检测io
#endif

//*********************************************************************************//
//                         AT_CHAR蓝牙串口配置                                     //
//*********************************************************************************//
#define TCFG_AT_CHAR_UART_TX_PORT			IO_PORTC_04
#define TCFG_AT_CHAR_UART_RX_PORT			IO_PORTC_03
#define TCFG_AT_CHAR_UART_BAUDRATE  		115200

//*********************************************************************************//
//                                升级相关配置                                     //
//*********************************************************************************//
#define TCFG_DUAL_BANK_ENABLE				DISABLE_THIS_MOUDLE //双备份升级使能
#define TCFG_DB_UPDATE_PASSIVE_DEMO_EN		DISABLE_THIS_MOUDLE //升级参考demo - 测试时用
#if TCFG_DUAL_BANK_ENABLE
#define CONFIG_DB_UPDATE_DATA_GENERATE_EN	ENABLE              //默认生成db_update_data.bin
#define CONFIG_FLASH_SIZE					4M                  //根据实际使用的flash大小配置 256K 512K 1M 2M ...
#if TCFG_DB_UPDATE_PASSIVE_DEMO_EN
#define CONFIG_UPDATA_INFO_ADD				0x39b000            //升级demo中升级文件(db_update_data.bin)存放的位置
#define CONFIG_UPDATA_INFO_LEN				400K                //升级文件占用空间
#define CONFIG_UPDATA_INFO_OPT				1
#endif
#endif

//*********************************************************************************//
//                         RTC 模式配置                                            //
//*********************************************************************************//
#define TCFG_APP_RTC_CLK_SEL             CLK_SEL_LRC
#define TCFG_APP_RTC_EN                  0

//*********************************************************************************//
//                                 配置结束                                        //
//*********************************************************************************//
#include "usb_std_class_def.h"
#include "usb_common_def.h"

#endif

