#include <string.h>
#include <ctype.h>
#include <rtthread.h>
#include <stm32f0xx.h>
#include "led.h"
#include "ble_uart_thread.h"
#include "laser_uart_thread.h"

#define RX_MSG_SIZE 32
#define RX_TIMEOUT 20 //100ms

static struct rt_semaphore ble_uartrx_sem;
static rt_device_t ble_dev = RT_NULL;

static rt_uint8_t  rx_buffer[RX_MSG_SIZE];
static rt_uint16_t rx_len=0;

uint8_t g_ble_connected=0;

static void update_ble_connected_status(void){
  uint8_t status = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0);
	if(status!=g_ble_connected){
		g_ble_connected =status;
		if(g_ble_connected){
			laser_on();
			rt_hw_led_on();
		}else{
			laser_off();
			rt_hw_led_off();
		}
	}
}
uint8_t ble_connected(){
  if(g_ble_connected==0){
    update_ble_connected_status();
  }
	return g_ble_connected;
}

static rt_err_t finsh_rx_ind(rt_device_t dev, rt_size_t size)
{
	RT_ASSERT(shell != RT_NULL);
	/* release semaphore to let finsh thread rx data */
	rt_sem_release(&ble_uartrx_sem);
	return RT_EOK;
}

static void ble_rx_thread_entry(void *parameter)
{
	while (1)
	{
		/* wait receive */
		if (rt_sem_take(&ble_uartrx_sem, RT_WAITING_FOREVER) != RT_EOK) continue;

		/* read one character from device */
		rt_thread_delay(RX_TIMEOUT);
		rt_memset(rx_buffer,0,RX_MSG_SIZE);
		rx_len = rt_device_read(ble_dev,0,rx_buffer,RX_MSG_SIZE);
		if(rx_len<=0)continue;
	}
}

void ble_uart_sendmsg(rt_uint8_t* msg, rt_uint8_t len)
{
    if (ble_dev != RT_NULL) {
        rt_device_write(ble_dev, 0, msg, len); 
    }        
}

void EXTI0_1_IRQHandler()
{
  update_ble_connected_status();
  EXTI_ClearITPendingBit(EXTI_Line0);
}
static void init_ble_state_timer(){
	GPIO_InitTypeDef  GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;//interrupt
	NVIC_InitTypeDef NVIC_InitStructure;//interrupt
	
	//step1. init state IO
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//interrupt

	/* Configure the GPIO_LASER pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*******************
	 **Interrupt Begin**
	 *******************/
	//EXTI
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;   
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	//NVIC
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_Init(&NVIC_InitStructure);
	/**Interrupt End**/
}
void ble_uart_thread_init(void)
{
	rt_thread_t uart_recv_thread; 
	
	//init ble state hw
	init_ble_state_timer();
	rt_hw_led_off();
	
	ble_dev = rt_device_find("ble_uart1");
	if(ble_dev == RT_NULL)
	{
		 rt_kprintf("finsh: can not find device: uart1\n");
		 return;
	}

	if(rt_device_open(ble_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK){
		 rt_kprintf("finsh: open ble_uart1\n");
		 rt_device_set_rx_indicate(ble_dev, finsh_rx_ind);
	}

	rt_sem_init(&ble_uartrx_sem, "ble_uartrx", 0, 0);
	uart_recv_thread = rt_thread_create("ble_uart_thread",
																			ble_rx_thread_entry, RT_NULL,
																			1024, 20, 20);
	if(uart_recv_thread != RT_NULL)rt_thread_startup(uart_recv_thread);
}
