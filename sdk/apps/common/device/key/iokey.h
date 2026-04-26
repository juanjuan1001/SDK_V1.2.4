#ifndef DEVICE_IOKEY_H
#define DEVICE_IOKEY_H

#include "typedef.h"
#include "key_driver.h"

#define ONE_PORT_TO_LOW 		0 		//按键一个端口接低电平, 另一个端口接IO
#define ONE_PORT_TO_HIGH		1 		//按键一个端口接高电平, 另一个端口接IO

struct iokey_port {
    u8 key_port;
    u8 connect_way;
    u8 key_value;
};

struct iokey_platform_data {
    u8 num;
    const struct iokey_port *port;
};

extern int iokey_init(void);
extern u8 io_get_key_value(void);
extern struct key_driver_para iokey_scan_para;


#endif


