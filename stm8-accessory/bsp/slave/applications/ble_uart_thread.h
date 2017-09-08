#ifndef __BLE_UART_THREAD_H__
#define __BLE_UART_THREAD_H__

#include <rthw.h>
#include <rtthread.h>
#include <stm32f0xx.h>


void 		ble_uart_thread_init(void);
void 		ble_uart_sendmsg(rt_uint8_t* msg, rt_uint8_t len);
uint8_t ble_connected(void);

#endif
