/*
 * File      : eeprom.h
 */

#ifndef __GY530_H__
#define __GY530_H__

#include <rthw.h>
#include <rtthread.h>
#include <stm32f0xx.h>

#define GY530_I2C_ADDR	(0x52>>1)
#define GY530_I2C_NAME "i2c1"

int rt_hw_gy530_init(void);
rt_uint8_t rt_gy530_read(rt_uint8_t addr, rt_uint8_t* buf, rt_uint8_t len);
rt_uint8_t rt_gy530_write(rt_uint8_t addr, rt_uint8_t* buf, rt_uint8_t len);

#endif
