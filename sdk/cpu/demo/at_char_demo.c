#include "at_char/at_char_app.h"

//蓝牙串口测试demo
void at_char_test_demo(void)
{
    printf("\n***** AT CHAR module test begin *****\n");

    u8 test_buf[3] = {0x01, 0x02, 0x03};

    if (at_char_init() == AT_CHAR_ERR) {
        printf("at char init err ! ! !\n");
        return;
    }

    if (at_char_ble_slave_broadcast_config() == AT_CHAR_ERR) {
        printf("at char ble br0adcast config err ! ! !\n");
        return;
    }

    at_char_uart_send_direct(test_buf, 3);

    at_char_uninit();

    printf("\n***** AT CHAR module test end *****\n");
}
