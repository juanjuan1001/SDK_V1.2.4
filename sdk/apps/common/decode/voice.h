#ifndef _VOICE_H_
#define _VOICE_H_

#include "includes.h"

/*
 * @brief 播放音频文件列表
 * @return 0:所有文件都播放结束 1:还有文件未播放
 */
char app_play_file_loop();

/*
 * @brief 输入收缩压/舒张压/心率参数，串联音频文件，并启动播放
 * @param pluse: 心率 sbp: 收缩压 dbp: 舒张压
 */
void make_voice_list_and_play(int pulse, int sbp, int dbp);

/*
 * @brief 播放初始化音频
 */
void app_play_set_and_begin(void);

/*
 * @brief 启动播放，开始播放音频文件列表
 */
void app_play_files(void);

#endif
