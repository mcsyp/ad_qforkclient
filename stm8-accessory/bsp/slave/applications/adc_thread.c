//user thread
#include "adc_thread.h"
#include "adc_init.h"

/******************************
 *ir update task related part
*******************************/
#define IR_UPDATE_TIMEOUT  20 //100ms
#define IR_QUEUE_SIZE 20
rt_uint16_t g_ir_dist=0;
float convert_ir_distance(float volt) {
  float dist = 0.0f;

  //TODO:a distance calc
  if (volt < 0.4f) {
    dist = 150.0f;
  } else if (volt > 2.8f) {
    dist = 15.0f;
  } else {
    dist = 28.0f * volt * volt - 132.0f * volt + 175.0f; //in cm
  }
  return dist;
}
static void ir_dist_update(){
	static float dist_sum=0.0f;
	static rt_uint8_t dist_len=0;
	
	float ir_volt=(float)rt_hw_sharp_ir_read()*3.3f/4096.0f;
	float ir_dist=convert_ir_distance(ir_volt);

	//push to ir_queue
	dist_sum+=ir_dist;
	++dist_len;

	if(dist_len>=IR_QUEUE_SIZE)
	{//queue is full
		g_ir_dist = (rt_uint16_t)(dist_sum*10.0f/dist_len);//convert to mm
		dist_len=0;
		dist_sum=0.0f;
	}
}

/******************************
 *battery update task related part
*******************************/
#define BAT_QUEUE_SIZE 10
rt_uint16_t g_bat_volt=0;
static void bat_volt_update()
{
	static float volt_sum=0.0f;
	static rt_uint8_t volt_len=0;
	
	float volt=(float)rt_hw_battery_read()*2*3300.0f/4096.0f;//mv

	//push to ir_queue
	volt_sum+=volt;
	++volt_len;
	
	if(volt_len>=BAT_QUEUE_SIZE)
	{//queue is full
		g_bat_volt = (rt_uint16_t)(volt_sum/volt_len);
		volt_len=0;
		volt_sum=0.0f;
	}
}
rt_uint16_t adc_read_ir_dist(void){
	return g_ir_dist;
}
rt_uint16_t adc_read_bat_volt(void){
	return g_bat_volt;
}
void adc_thread_entry(void* parameter){
	while(1){
		ir_dist_update();
		bat_volt_update();
		
		rt_thread_delay(IR_UPDATE_TIMEOUT);
		rt_thread_yield();
	}
}

void adc_thread_init(void)
{
	rt_thread_t adc_thread; 
	
	rt_hw_ad_init();
  adc_thread = rt_thread_create("adc_thread",
																adc_thread_entry, RT_NULL,
																256, 20, 20);
	if(adc_thread != RT_NULL)rt_thread_startup(adc_thread);
}