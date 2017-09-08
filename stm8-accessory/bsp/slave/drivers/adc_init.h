#ifndef SHARP_IR_H
#define SHARP_IR_H

#include <rthw.h>
#include <rtthread.h>
#include <stm32f0xx.h>

#define ADC_CHANNEL_SHARP_IR 1
#define ADC_CHANNEL_BATTERY 0

int rt_hw_ad_init(void);
rt_uint16_t rt_hw_ad_read(rt_uint8_t channel);

#define rt_hw_sharp_ir_read() rt_hw_ad_read(ADC_CHANNEL_SHARP_IR)
#define rt_hw_battery_read() rt_hw_ad_read(ADC_CHANNEL_BATTERY)

#endif //sharp_ir_h
