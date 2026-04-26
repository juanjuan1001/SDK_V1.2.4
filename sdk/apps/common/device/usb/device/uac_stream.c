#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".usb.data.bss")
#pragma data_seg(".usb.data")
#pragma code_seg(".usb.text")
#pragma const_seg(".usb.text.const")
#pragma str_literal_override(".usb.text.const")
#endif

#include "app_config.h"
#include "system/includes.h"
#include "printf.h"
#include "usb/usb_config.h"
#include "usb/device/usb_stack.h"

#if TCFG_USB_SLAVE_AUDIO_ENABLE
#include "usb/device/uac_audio.h"
#include "uac_stream.h"

#define LOG_TAG_CONST       USB
#define LOG_TAG             "[UAC]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

void uac_speaker_stream_write(const u8 *obuf, u32 len)
{
    /* putchar('s'); */
    return;
}

void uac_speaker_stream_open(u32 samplerate, u32 ch)
{
    log_info("func:%s(), line:%d\n", __func__, __LINE__);
    return;
}

void uac_speaker_stream_close()
{
    log_info("func:%s(), line:%d\n", __func__, __LINE__);
    return;
}

int uac_get_spk_vol()
{
    return 99;
}

void uac_mute_volume(u32 type, u32 l_vol, u32 r_vol)
{
    log_info("func:%s(), line:%d\n", __func__, __LINE__);
    return;
}

int uac_mic_stream_read(u8 *buf, u32 len)
{
    /* putchar('m'); */
    memset(buf, 0, len);
    return len;
}

u32 uac_mic_stream_open(u32 samplerate, u32 frame_len, u32 ch)
{
    log_info("func:%s(), line:%d\n", __func__, __LINE__);
    return 0;
}

void uac_mic_stream_close()
{
    log_info("func:%s(), line:%d\n", __func__, __LINE__);
    return;
}

_WEAK_
s8 app_audio_get_volume(u8 state)
{
    return 88;
}
_WEAK_
void usb_audio_demo_exit(void)
{

}
_WEAK_
int usb_audio_demo_init(void)
{
    return 0;
}
_WEAK_
u8 get_max_sys_vol(void)
{
    return 100;
}
_WEAK_
void *audio_local_sample_track_open(u8 channel, int sample_rate, int period)
{
    return NULL;
}
_WEAK_
int audio_local_sample_track_in_period(void *c, int samples)
{
    return 0;
}
_WEAK_
void audio_local_sample_track_close(void *c)
{
}
#endif

