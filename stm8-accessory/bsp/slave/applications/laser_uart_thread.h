#ifndef __LASER_UART_THREAD_H__
#define __LASER_UART_THREAD_H__

#include <rthw.h>
#include <rtthread.h>
#include <stm32f0xx.h>

void laser_io_init(void);
void laser_on(void);
void laser_off(void);

#endif
