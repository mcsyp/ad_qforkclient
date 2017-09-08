/*
 * File      : eeprom.h
 */

#ifndef __SLAVE_I2C_H__
#define __SLAVE_I2C_H__

#include <rthw.h>
#include <rtthread.h>
#include <stm32f0xx.h>

#define SLAVE_I2C_ADDR	(0xA0)
#define SLAVE_I2C_NAME 	"i2c1"

int rt_hw_slave_i2c_init(void);
rt_uint8_t rt_slave_i2c_read(rt_uint8_t addr, rt_uint8_t* buf, rt_uint8_t len);
rt_uint8_t rt_slave_i2c_write(rt_uint8_t addr, rt_uint8_t* buf, rt_uint8_t len);

#endif
