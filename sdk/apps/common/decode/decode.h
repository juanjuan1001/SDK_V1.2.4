#ifndef _UMP3_DECODE_H_
#define _UMP3_DECODE_H_

#include "includes.h"
#include "sdfile.h"

#define TONE_NUM_0      		SDFILE_RES_ROOT_PATH"tone/0.ump3"
#define TONE_NUM_1      		SDFILE_RES_ROOT_PATH"tone/1.ump3"
#define TONE_NUM_2				SDFILE_RES_ROOT_PATH"tone/2.ump3"
#define TONE_NUM_3				SDFILE_RES_ROOT_PATH"tone/3.ump3"
#define TONE_NUM_4				SDFILE_RES_ROOT_PATH"tone/4.ump3"
#define TONE_NUM_5				SDFILE_RES_ROOT_PATH"tone/5.ump3"
#define TONE_NUM_6				SDFILE_RES_ROOT_PATH"tone/6.ump3"
#define TONE_NUM_7				SDFILE_RES_ROOT_PATH"tone/7.ump3"
#define TONE_NUM_8				SDFILE_RES_ROOT_PATH"tone/8.ump3"
#define TONE_NUM_9				SDFILE_RES_ROOT_PATH"tone/9.ump3"
#define TONE_ALARM				SDFILE_RES_ROOT_PATH"tone/alarm.ump3"
#define TONE_AVERAGE       		SDFILE_RES_ROOT_PATH"tone/average.ump3"
#define TONE_BAI    			SDFILE_RES_ROOT_PATH"tone/bai.ump3"
#define TONE_BASEWORLD			SDFILE_RES_ROOT_PATH"tone/baseworld.ump3"
#define TONE_BEGIN				SDFILE_RES_ROOT_PATH"tone/begin.ump3"
#define TONE_CONTINUEHIGH		SDFILE_RES_ROOT_PATH"tone/continhigh.ump3"
#define TONE_DIAN				SDFILE_RES_ROOT_PATH"tone/dian.ump3"
#define TONE_END				SDFILE_RES_ROOT_PATH"tone/end.ump3"
#define TONE_ERROR				SDFILE_RES_ROOT_PATH"tone/error.ump3"
#define TONE_HIGH				SDFILE_RES_ROOT_PATH"tone/high.ump3"
#define TONE_KP					SDFILE_RES_ROOT_PATH"tone/kp.ump3"
#define TONE_LOW				SDFILE_RES_ROOT_PATH"tone/low.ump3"
#define TONE_LOWPOWER 			SDFILE_RES_ROOT_PATH"tone/lowpower.ump3"
#define TONE_MAIBO 				SDFILE_RES_ROOT_PATH"tone/maibo.ump3"
#define TONE_MMHG 				SDFILE_RES_ROOT_PATH"tone/mmhg.ump3"
#define TONE_NORMAL 			SDFILE_RES_ROOT_PATH"tone/normal.ump3"
#define TONE_NORMALHIGH			SDFILE_RES_ROOT_PATH"tone/normalhigh.ump3"
#define TONE_PLEASESEAT			SDFILE_RES_ROOT_PATH"tone/pleaseseat.ump3"
#define TONE_REGISTER			SDFILE_RES_ROOT_PATH"tone/register.ump3"
#define TONE_SHI 				SDFILE_RES_ROOT_PATH"tone/shi.ump3"
#define TONE_SHOUSUOYA 			SDFILE_RES_ROOT_PATH"tone/shousuoya.ump3"
#define TONE_SHUZHANGYA 		SDFILE_RES_ROOT_PATH"tone/shuzhangya.ump3"
#define TONE_THANKS 			SDFILE_RES_ROOT_PATH"tone/thanks.ump3"
#define TONE_ZHUYI 			    SDFILE_RES_ROOT_PATH"tone/zhuyi.ump3"

enum INDEX_TONE {
    INDEX_TONE_NUM_0,
    INDEX_TONE_NUM_1,
    INDEX_TONE_NUM_2,
    INDEX_TONE_NUM_3,
    INDEX_TONE_NUM_4,
    INDEX_TONE_NUM_5,
    INDEX_TONE_NUM_6,
    INDEX_TONE_NUM_7,
    INDEX_TONE_NUM_8,
    INDEX_TONE_NUM_9,
    INDEX_TONE_ALARM,
    INDEX_TONE_AVERAGE,
    INDEX_TONE_BAI,
    INDEX_TONE_BASEWORLD,
    INDEX_TONE_BEGIN,
    INDEX_TONE_CONTINUEHIGH,
    INDEX_TONE_DIAN,
    INDEX_TONE_END,
    INDEX_TONE_ERROR,
    INDEX_TONE_HIGH,
    INDEX_TONE_KP,
    INDEX_TONE_LOW,
    INDEX_TONE_LOWPOWER,
    INDEX_TONE_MAIBO,
    INDEX_TONE_MMHG,
    INDEX_TONE_NORMAL,
    INDEX_TONE_NORMALHIGH,
    INDEX_TONE_PLEASESEAT,
    INDEX_TONE_REGISTER,
    INDEX_TONE_SHI,
    INDEX_TONE_SHOUSUOYA,
    INDEX_TONE_SHUZHANGYA,
    INDEX_TONE_THANKS,
    INDEX_TONE_ZHUYI,
};

enum {
    MAD_STOP = 0,
    MAD_PLAY = 1,
    MAD_PAUSE = 2,
    MAD_END = 3,
};

enum {
    DEC_END = 0,
    DEC_ING = 1,
};

enum {
    DEC_SUCC = 0,
    DEC_FAIL = 1,
};

/*
 * @brief 初始化解码器
 */
void decode_init(void);

/*
 * @brief 解码器停止工作，释放资源
 */
void decode_uninit(void);

/*
 * @brief 通过文件索引，播放音频文件
 * @param index: 音频文件索引
 * @return DEC_FAIL: 解码失败 DEC_SUCC: 解码成功
 */
u8 a_player_tonebyindex(enum INDEX_TONE index);

/*
 * @brief 结束音频播放，关闭播放器
 * @param wait: 1: 等待当前音频播放结束 0：不等待
 */
void a_player_stop(u8 wait);

/*
 * @brief 暂停/启动 音频播放函数
 * @return 返回当前播放状态
 */
u8 a_player_pause(void);

#endif
