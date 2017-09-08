//user thread

#include "led.h"
#include "led_thread.h"

static rt_uint16_t g_on_tick=0;
static rt_uint16_t g_off_tick=0;
static rt_uint8_t g_repeats=0;
static rt_thread_t g_led_thread=RT_NULL;
void led_start_blink(rt_uint16_t on_tick,rt_uint16_t off_tick,rt_uint8_t repeats)
{
	g_on_tick =  on_tick;
	g_off_tick = off_tick;
	g_repeats = repeats;
	
	if(g_led_thread!=RT_NULL){
		rt_thread_startup(g_led_thread);
	}
}
void led_thread_init(void){
	g_led_thread = rt_thread_create("ked_thread_init",
                                  led_thread_entry, RT_NULL,
                                  512, 8, 20);
}
void led_thread_entry(void* parameter)
{
	int i=0;
	for(i=0;i<g_repeats;++i){
		rt_hw_led_on();
		rt_thread_delay(g_on_tick);
		rt_hw_led_off();
		rt_thread_delay(g_off_tick);
	}
}
