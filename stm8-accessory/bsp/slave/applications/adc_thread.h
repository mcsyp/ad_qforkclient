#ifndef __ADC_THREAD__
#define __ADC_THREAD__
#include <rthw.h>
#include <rtthread.h>
#include <stm32f0xx.h>

void adc_thread_init(void);
void adc_thread_entry(void* parameter);

rt_uint16_t adc_read_ir_dist(void);
rt_uint16_t adc_read_bat_volt(void);
#endif
