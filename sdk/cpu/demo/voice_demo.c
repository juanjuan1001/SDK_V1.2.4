#include "decode/decode.h"
#include "decode/voice.h"

//音频文件播放测试demo
void voice_test_demo(void)
{
    printf("\n***** VOICE module test begin *****\n");

    // 在该接口内添加音频到播放列表后开始播放
    app_play_set_and_begin();

    // 每播放完一条音频都会发出MSG_EVENT_DECODE_END消息事件
    // 在app_msg_event_handler中对该事件进行处理，检查是否已播放完所有音频
    // 未播放完则继续播放下一条，直至结束
}
