#ifndef __LED_THREAD__
#define __LED_THREAD__
#include <rtthread.h>
void led_thread_init(void);
void led_thread_entry(void* parameter);
void led_start_blink(rt_uint16_t on_ticks,rt_uint16_t off_ticks,rt_uint8_t repeats);

#endif
