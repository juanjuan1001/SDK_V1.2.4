/*
* @file  		voice.c
* @brief        音频播放接口
* @details		TONE PLAYER API
* @author		JL
* @date     	2023-06-09
* @version  	V1.0
* @copyright    Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
 */

#include "decode.h"
#include "voice.h"

#define PRESSURE_UNIT_MMHG  1
#define PRESSURE_UNIT_KPA   0
#define PRESSURE_UNIR_MODE  PRESSURE_UNIT_MMHG//血压单位

static u8 playfilelist[30]; //播放列表
static u8 allplayfile;		//播放列表文件总数
static u8 currentfile;		//当前播放文件索引

/*
 * @brief 根据输入数值，串联音频文件
 * @param str:起始音频文件地址 num：数值
 * @return 返回串联长度
 */
static char num_to_voice(char *str, int num)
{
    char *bck = str;
    if (num >= 1000) {
        return 0;
    }
    if (num >= 100) {
        *str++ = num / 100 + INDEX_TONE_NUM_0;
        *str++ = INDEX_TONE_BAI;
        num = num % 100;
        if ((num / 10 == 0) && (num % 10 != 0)) {
            *str++ = INDEX_TONE_NUM_0;
        }
    }
    if (num >= 10) {
        *str++ = num / 10 + INDEX_TONE_NUM_0;
        *str++ = INDEX_TONE_SHI;
        num = num % 10;
    }
    if (num > 0) {
        *str++ = num + INDEX_TONE_NUM_0;
    }
    return str - bck;
}

/*
 * @brief 播放串联的音频文件列表
 * @return 0:所有文件都播放结束 1:还有文件未播放
 */
char app_play_file_loop(void)
{
    currentfile++;
    if (currentfile < allplayfile) {
        a_player_tonebyindex(playfilelist[currentfile]);
        return 1;
    }
    return 0;
}

/*
 * @brief 启动播放，开始播放音频文件列表
 */
void app_play_files(void)
{
    currentfile = 0;
    a_player_tonebyindex(playfilelist[currentfile]);
}

/*
 * @brief 播放初始化音频
 */
void app_play_set_and_begin(void)
{
    u8 index = 0;
    playfilelist[index++] = INDEX_TONE_ZHUYI;
    playfilelist[index++] = INDEX_TONE_REGISTER;
    playfilelist[index++] = INDEX_TONE_BEGIN;
    allplayfile = index;
    app_play_files();
}

/*
 * @brief 输入收缩压/舒张压/心率参数，串联音频文件，并启动播放
 * @param pluse: 心率 sbp: 收缩压 dbp: 舒张压
 */
void make_voice_list_and_play(int pulse, int sbp, int dbp)
{
    u8 index = 0 ;
    if (PRESSURE_UNIR_MODE) {
        playfilelist[index++] = INDEX_TONE_SHOUSUOYA;
        index += num_to_voice((char *)&playfilelist[index], sbp);
        playfilelist[index++] = INDEX_TONE_MMHG;
        playfilelist[index++] = INDEX_TONE_SHUZHANGYA;
        index += num_to_voice((char *)&playfilelist[index], dbp);
        playfilelist[index++] = INDEX_TONE_MMHG;
        playfilelist[index++] = INDEX_TONE_MAIBO;
        index += num_to_voice((char *)&playfilelist[index], pulse);
        allplayfile = index ;
    } else {
        playfilelist[index++] = INDEX_TONE_SHOUSUOYA;
        index += num_to_voice((char *)&playfilelist[index], sbp * 100 / 75);
        playfilelist[index++] = INDEX_TONE_KP;
        playfilelist[index++] = INDEX_TONE_SHUZHANGYA;
        index += num_to_voice((char *)&playfilelist[index], dbp * 100 / 75);
        playfilelist[index++] = INDEX_TONE_KP;
        playfilelist[index++] = INDEX_TONE_MAIBO;
        index += num_to_voice((char *)&playfilelist[index], pulse);
        allplayfile = index;
    }
    app_play_files();//开始播放
}
