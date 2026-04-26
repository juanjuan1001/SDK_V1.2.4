/*
* @file  		at_char_drv.c
* @brief        AT_CHAR协议驱动及应用接口
* @details		AT_CHAR DRIVER / AT_CHAR API
* @author		JL
* @date     	2023-06-14
* @version  	V1.0
* @copyright    Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
 */

#include "at_char_drv.h"
#include "uart_v2.h"
#include "system/includes.h"
#include "malloc.h"

#define LOG_TAG_CONST   AT_CHAR
#define LOG_TAG	        "[AT_CHAR]"
#include "debug.h"

struct at_char_info {
    char *at_char_send_buf; //发送数据buf
    void *uart_rx_cbuf;		//uart接收缓冲区
    void *rx_buf;			//uart数据读取内存
    s32 uart_num;			//uart序号
    u8 init_ok;				//初始化标志
    volatile u8 run_state;	//at_char工作状态
    u8 slave_ch;			//从机工作通道
    u32 e_value;			//发布消息携带的数据
    u8 conn_state;			//蓝牙芯片连接状态
};
static struct at_char_info _at_char_info = {
    .at_char_send_buf = NULL,
    .uart_rx_cbuf = NULL,
    .rx_buf = NULL,
    .init_ok = 0,
    .slave_ch = AT_CHAR_CH,
};
#define __this (&_at_char_info)

#define DMA_RX_CBUF_SIZE  (64)
#define RX_BUF_SIZE 	  (32)

//头部信息列表
const char *const at_char_head_table[] = {AT_HEAD_AT_CMD, AT_HEAD_AT_CHL};

//命令列表
const char *const at_char_cmd_table[] = {
    AT_STR_ENTER,     AT_STR_OK,       AT_STR_ERR, 		   AT_STR_GVER,
    AT_STR_GCFGVER,   AT_STR_NAME,     AT_STR_LBDADDR, 	   AT_STR_BAUD,
    AT_STR_ADV, 	  AT_STR_ADVPARAM, AT_STR_ADVDATA, 	   AT_STR_SRDATA,
    AT_STR_CONNPARAM, AT_STR_SCAN, 	   AT_STR_TARGETUUID,  AT_STR_CONN,
    AT_STR_DISC,      AT_STR_OTA, 	   AT_STR_CONN_CANNEL, AT_STR_POWEROFF,
    AT_STR_LOWPOWER,
};

//特殊字符列表
const char *const at_spec_char_table[] = {
    SPEC_CHAR_PLUS_SIGN, 	 SPEC_CHAR_GREATER_THAN_SIGN, SPEC_CHAR_EQUAL_SIGN,
    SPEC_CHAR_QUESTION_SIGN, SPEC_CHAR_ENTER_SIGN,        SPEC_CHAR_COMMA_SIGN,
};

__attribute__((weak))
u8 update_body_info(const char *info)
{
    return 0;
}

/*
 * @brief 串口透传发送裸包数据
 * @param send_buf: 发送的数据, send_len: 发送长度
 * @note 在连接成功之后，为透传模式，可直接用该接口发送数据
 */
void at_char_uart_send_direct(void *send_buf, u8 send_len)
{
    ASSERT(send_buf, "at char send empty packet!!!")
    uart_send_blocking(__this->uart_num, send_buf, send_len, 0);
}

/*
 * @brief AT_CHAR数据包发送和接收函数
 * @param packet: 发送数据包内容结构体
 * @return 接收从机数据的buf指针，返回NULL代表失败，否则成功
 */
char *at_char_send_and_recv(const AT_CHAR_SEND *packet)
{
    u8 str_len;
    u8 offset = 0;
    u8 param_len = 0;

    ASSERT(__this->init_ok, "at char module not init !!!")
    ASSERT(packet, "at char send empty packet!!!")

    if (__this->slave_ch != AT_CHAR_CH) {
        __this->run_state = 1;
        char *at_ch_set[1] = {"AT>9\r"};
        at_char_uart_send_direct(at_ch_set[0], 5);
        memset(__this->rx_buf, 0, RX_BUF_SIZE);
        uart_recv_blocking(__this->uart_num, __this->rx_buf, RX_BUF_SIZE, 50);
        __this->run_state = 0;
        if (strstr(__this->rx_buf, at_char_cmd_table[OK])) {
            __this->slave_ch = AT_CHAR_CH;
        } else {
            return NULL;
        }
    }

    if (__this->at_char_send_buf) {
        free(__this->at_char_send_buf);
        __this->at_char_send_buf = NULL;
    }

    if (!packet->param_size) {
        str_len = strlen(at_char_head_table[packet->head]) + strlen(at_char_cmd_table[packet->cmd]) + 1 + 1;
    } else {
        for (u8 i = 0; i < packet->param_size; i++) {
            param_len += strlen(packet->param_buf[i]);
        }
        str_len = strlen(at_char_head_table[packet->head]) + strlen(at_char_cmd_table[packet->cmd]) + 1 +  param_len + (packet->param_size - 1) + 1;
    }
    __this->at_char_send_buf = (char *)malloc(str_len);
    if (__this->at_char_send_buf == NULL) {
        log_error("at char send malloc err!!!\n");
        return NULL;
    }

    memset(__this->at_char_send_buf, 0, str_len);
    memcpy(__this->at_char_send_buf, at_char_head_table[packet->head], strlen(at_char_head_table[packet->head]));
    offset += strlen(at_char_head_table[packet->head]);
    memcpy(__this->at_char_send_buf + offset, at_char_cmd_table[packet->cmd], strlen(at_char_cmd_table[packet->cmd]));
    offset += strlen(at_char_cmd_table[packet->cmd]);
    memcpy(__this->at_char_send_buf + offset, at_spec_char_table[packet->spec], 1);
    offset += 1;

    if (!packet->param_size) {
        memcpy(__this->at_char_send_buf + offset, at_spec_char_table[ENTER_SIGN], 1);
        offset += 1;
    } else {
        for (u8 i = 0; i < packet->param_size; i++) {
            memcpy(__this->at_char_send_buf + offset, packet->param_buf[i], strlen(packet->param_buf[i]));
            offset += strlen(packet->param_buf[i]);
            if (i == (packet->param_size - 1)) {
                memcpy(__this->at_char_send_buf + offset, at_spec_char_table[ENTER_SIGN], 1);
            } else {
                memcpy(__this->at_char_send_buf + offset, at_spec_char_table[COMMA_SIGN], 1);
            }
            offset += 1;
        }
    }

    __this->run_state = 1;
    uart_send_blocking(__this->uart_num, __this->at_char_send_buf, str_len, 0);
    /* log_info("send: %s\n", __this->at_char_send_buf); */

    memset(__this->at_char_send_buf, 0, str_len);
    free(__this->at_char_send_buf);
    __this->at_char_send_buf = NULL;

    memset(__this->rx_buf, 0, RX_BUF_SIZE);
    uart_recv_blocking(__this->uart_num, __this->rx_buf, RX_BUF_SIZE, 50);
    log_info("%s\n", __this->rx_buf);
    __this->run_state = 0;

    if (strstr(__this->rx_buf, at_char_cmd_table[OK])) {
        return __this->rx_buf;
    } else if (strstr(__this->rx_buf, at_char_cmd_table[ERR])) {
        log_error("comm succ, but control err!!!\n");
        return NULL;
    } else {
        log_error("comm err, please sure slave exist!!!\n");
        return NULL;
    }
}

/*
 * @brief 获取蓝牙芯片连接状态
 */
u8 get_at_char_conn_state(void)
{
    return __this->conn_state;
}

/*
 * @brief AT_CHAR资源释放
 */
void at_char_uninit(void)
{
    if (__this->init_ok == 0) {
        return;
    }
    if (__this->at_char_send_buf) {
        free(__this->at_char_send_buf);
        __this->at_char_send_buf = NULL;
    }
    if (__this->uart_rx_cbuf) {
        free(__this->uart_rx_cbuf);
        __this->uart_rx_cbuf = NULL;
    }
    if (__this->rx_buf) {
        free(__this->rx_buf);
        __this->rx_buf = NULL;
    }
    uart_deinit(__this->uart_num);
    __this->init_ok = 0;
}

/*
 * @brief 串口中断处理函数
 * @note 此处用于更新蓝牙芯片的工作通道
 */
static void at_char_uart_irq_callback(uart_dev uart_num, enum uart_event event)
{
    u8 calibrate_w[3];
    if (event & UART_EVENT_RX_DATA) {
        if (!__this->run_state) {
            memset(__this->rx_buf, 0, RX_BUF_SIZE);
            uart_recv_bytes(uart_num, __this->rx_buf, RX_BUF_SIZE);
            if (strstr(__this->rx_buf, "IM_CONN:")) {
                __this->conn_state = 1;
                __this->slave_ch = *(strstr(__this->rx_buf, "IM_CONN:") + strlen("IM_CONN:")) - '0';
                sys_msg_event_notify(MSG_EVENT_AT_CHAR_CONN, __this->e_value);
            } else if (strstr(__this->rx_buf, "IM_DISC:")) {
                __this->conn_state = 0;
                __this->slave_ch = *(strstr(__this->rx_buf, "IM_DISC:") + strlen("IM_DISC:")) - '0';
                sys_msg_event_notify(MSG_EVENT_AT_CHAR_DISCONN, __this->e_value);
            } else if (strstr(__this->rx_buf, "CALIBRATE:")) {
                for (u8 i = 0; i < 3; i++) {
                    calibrate_w[i] = *(strstr(__this->rx_buf, "CALIBRATE:") + strlen("CALIBRATE:") + i) - '0';
                }
                __this->e_value = calibrate_w[0] * 100 + calibrate_w[1] * 10 + calibrate_w[2];
                sys_msg_event_notify(MSG_EVENT_AT_CHAR_CALIBRATE_WEIGHT, __this->e_value);
            } else if (strstr(__this->rx_buf, "CALIBRATE_END")) {
                sys_msg_event_notify(MSG_EVENT_AT_CHAR_CALIBRATE_END, __this->e_value);
            } else if (strstr(__this->rx_buf, "CALIBRATION_RESET")) {
                sys_msg_event_notify(MSG_EVENT_AT_CHAR_CALIBRATION_RESET, __this->e_value);
            } else if (strstr(__this->rx_buf, "BODY_INFO:")) {
                if (update_body_info(__this->rx_buf)) {
                    sys_msg_event_notify(MSG_EVENT_AT_CHAR_BODY_FAT_SCALE_GO, __this->e_value);
                }
            }
        }
    }
}

/*
 * @brief AT_CHAR模块初始化
 * @return AT_CHAR_SUCC: 成功, AT_CHAR_ERR: 失败
 * @note 内存资源申请，串口初始化等
 */
enum at_char_res at_char_init(void)
{
    if (__this->init_ok == 1) {
        return AT_CHAR_SUCC;
    }
    __this->uart_rx_cbuf = malloc(DMA_RX_CBUF_SIZE);
    __this->rx_buf = malloc(RX_BUF_SIZE);
    if ((__this->uart_rx_cbuf == NULL) || (__this->rx_buf == NULL)) {
        goto _at_char_err;
    }
    memset(__this->uart_rx_cbuf, 0, DMA_RX_CBUF_SIZE);
    memset(__this->rx_buf, 0, RX_BUF_SIZE);

    struct uart_config _uart_config = {
        .baud_rate = TCFG_AT_CHAR_UART_BAUDRATE,
        .tx_pin = TCFG_AT_CHAR_UART_TX_PORT,
        .rx_pin = TCFG_AT_CHAR_UART_RX_PORT,
    };

    struct uart_dma_config _dma_config = {
        .rx_timeout_thresh = 100,
        .frame_size = 32,
        .event_mask = UART_EVENT_RX_DATA,
        .irq_callback = at_char_uart_irq_callback,
        .rx_cbuffer = __this->uart_rx_cbuf,
        .rx_cbuffer_size = DMA_RX_CBUF_SIZE,
    };

    __this->uart_num = uart_init(-1, &_uart_config);
    if (__this->uart_num < 0) {
        goto _at_char_err;
    }
    if (uart_dma_init(__this->uart_num, &_dma_config) != 0) {
        goto _at_char_err;
    }

    //唤醒蓝牙芯片
    u8 ble_wkup[] = {0, 0, 0, 0, 0, 0, 0, 0};
    at_char_uart_send_direct((u8 *)ble_wkup, ARRAY_SIZE(ble_wkup));

    __this->init_ok = 1;
    mdelay(500);
    return AT_CHAR_SUCC;

_at_char_err:
    log_error("at char uart init error !!!\n");
    at_char_uninit();
    return AT_CHAR_ERR;
}
