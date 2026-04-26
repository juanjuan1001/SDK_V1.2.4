#ifndef _AT_CHAR_DRV_H_
#define _AT_CHAR_DRV_H_

#include "typedef.h"
#include "app_config.h"

#define AT_HEAD_AT_CMD		"AT+"
#define AT_HEAD_AT_CHL		"AT>"

#define AT_STR_ENTER		"\r\n"
#define AT_STR_OK			"OK"
#define AT_STR_ERR			"ERR"
#define AT_STR_GVER			"GVER"
#define AT_STR_GCFGVER		"GCFGVER"
#define AT_STR_NAME 		"NAME"
#define AT_STR_LBDADDR 		"LBDADDR"
#define AT_STR_BAUD 		"BAUD"
#define AT_STR_ADV 			"ADV"
#define AT_STR_ADVPARAM 	"ADVPARAM"
#define AT_STR_ADVDATA 		"ADVDATA"
#define AT_STR_SRDATA		"SRDATA"
#define AT_STR_CONNPARAM 	"CONNPARAM"
#define AT_STR_SCAN 		"SCAN"
#define AT_STR_TARGETUUID 	"TARGETUUID"
#define AT_STR_CONN 		"CONN"
#define AT_STR_DISC 		"DISC"
#define AT_STR_OTA 			"OTA"
#define AT_STR_CONN_CANNEL 	"CONN_CANNEL"
#define AT_STR_POWEROFF 	"POWEROFF"
#define AT_STR_LOWPOWER 	"LOWPOWER"

#define SPEC_CHAR_PLUS_SIGN			"+"
#define SPEC_CHAR_GREATER_THAN_SIGN	">"
#define SPEC_CHAR_EQUAL_SIGN		"="
#define SPEC_CHAR_QUESTION_SIGN		"?"
#define SPEC_CHAR_ENTER_SIGN		"\r"
#define SPEC_CHAR_COMMA_SIGN		","

#define MASTER_CH0		(0)
#define MASTER_CH1		(1)
#define MASTER_CH2		(2)
#define SLAVE_CH		(8)
#define AT_CHAR_CH		(9)

enum at_char_head {
    AT_CMD,
    AT_CHL,
};

enum at_char_cmd {
    ENTER,
    OK,
    ERR,
    GVER,
    GCFGVER,
    NAME,
    LBDADDR,
    BAUD,
    ADV,
    ADVPARAM,
    ADVDATA,
    SRDATA,
    CONNPARAM,
    SCAN,
    TARGETUUID,
    CONN,
    DISC,
    OTA,
    CONN_CANNEL,
    POWEROFF,
    LOWPOWER,
};

enum at_spec_char {
    PLUS_SIGN,
    GREATER_THAN_SIGN,
    EQUAL_SIGN,
    QUESTION_SIGN,
    ENTER_SIGN,
    COMMA_SIGN,
};

enum at_char_res {
    AT_CHAR_ERR,
    AT_CHAR_SUCC,
};

typedef struct at_char_packet {
    enum at_char_head head;	//包头
    enum at_char_cmd cmd; 	//命令
    enum at_spec_char spec; //特殊字符
    char **param_buf;		//参数数组
    u8 param_size;			//参数个数
} AT_CHAR_SEND;

/*
 * @brief 获取蓝牙芯片连接状态
 */
u8 get_at_char_conn_state(void);

/*
 * @brief AT_CHAR模块初始化
 * @return AT_CHAR_SUCC: 成功, AT_CHAR_ERR: 失败
 * @note 内存资源申请，串口初始化等
 */
enum at_char_res at_char_init(void);

/*
 * @brief AT_CHAR资源释放
 */
void at_char_uninit(void);

/*
 * @brief AT_CHAR数据包发送和接收函数
 * @param packet: 发送数据包内容结构体
 * @return 返回接收从机数据的buf，返回NULL代表失败，否则成功
 */
char *at_char_send_and_recv(const AT_CHAR_SEND *packet);

/*
 * @brief 串口发送裸包数据，非AT_CHAR协议格式
 * @param send_buf: 发送的数据, send_len: 发送长度
 */
void at_char_uart_send_direct(void *send_buf, u8 send_len);


#endif
