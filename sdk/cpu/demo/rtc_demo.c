#include "rtc/rtc_dev.h"
#include "timer.h"
#include "asm/power_interface.h"

struct sys_time temp;
u16 dump_timer;

void alm_ring_fun(const struct rtc_event_t *ev)
{
    if (ev->event & BIT(RTC_ALARM_EVENT)) {
        printf("\nalarm_wakeup_event\n");
    }
}

void rtc_time_test_dump(void)
{
    rtc_read_time(&temp);
    printf("rtc_sys_time: %d-%d-%d %d:%d:%d\n", temp.year, temp.month, temp.day, temp.hour, temp.min, temp.sec);
    rtc_read_alarm(&temp);                  //读当前alarm时间
    printf("rtc_rtc_read_alarm: %d-%d-%d %d:%d:%d\n", temp.year, temp.month, temp.day, temp.hour, temp.min, temp.sec);


}

void rtc_test_demo_start(void)
{
    struct sys_time def_sys_time_t = {
        .year = 2026,
        .month = 4,
        .day = 1,
        .hour = 0,
        .min = 0,
        .sec = 0,
    };

    struct rtc_dev_platform_data rtc_config_t = {
        .default_sys_time = &def_sys_time_t,
        .cbfun = alm_ring_fun,
    };

    rtc_init(&rtc_config_t);
    printf("---------------\n");
    printf("\nrtc_dev_init\n");
    rtc_read_time(&temp);
    printf("rtc_default_sys_time: %d-%d-%d %d:%d:%d\n", temp.year, temp.month, temp.day, temp.hour, temp.min, temp.sec);

    datetime_add_seconds(&temp, 55);
    rtc_alarm_switch(1);
    rtc_write_alarm(&temp);
    rtc_read_alarm(&temp);
    printf("rtc_default_alarm: %d-%d-%d %d:%d:%d\n", temp.year, temp.month, temp.day, temp.hour, temp.min, temp.sec);
    printf("---------------\n");

    dump_timer = usr_timer_add(NULL, (void *)rtc_time_test_dump, 1000, 0);
}

void rtc_reset_alarm_presoftoff_test(void)
{
    if (!dump_timer) {
        return;
    }
    // softoff前设置闹钟
    rtc_read_time(&temp);
    if (temp.sec < 55) {
        temp.sec += 5;
    } else {
        temp.min += 1;
        temp.sec -= 55;
    }
    rtc_alarm_switch(1);
    rtc_write_alarm(&temp);

    // 休眠 等待闹钟唤醒
    /* sys_softoff(); */
}

void rtc_test_demo_close(void)
{
    if (dump_timer) {
        usr_timer_del(dump_timer);
        dump_timer = 0;
    }
    rtc_dev_deinit();
}
