#ifndef _IIC_EEPROM_H_
#define _IIC_EEPROM_H_

#define EEPROM_EN 1

#if EEPROM_EN
#include "typedef.h"

void eeprom_write(int iic, u8 *buf, u32 addr, u32 len);
void eeprom_read(int iic, u8 *buf, u32 addr, u32 len);
#endif
#endif

