/*
 * File      : uart_thread.c
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "led.h"

#include "laser_uart_thread.h"

#define RX_MSG_SIZE 16
#define LASER_UART_FIRE "D"
#define LASER_UART_ON 	"O"
#define LASER_EN_IO GPIO_Pin_1


void laser_on(void){
  GPIO_SetBits(GPIOB, LASER_EN_IO);
}
void laser_off(void){
	GPIO_ResetBits(GPIOB, LASER_EN_IO);
}
void laser_io_init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	/* Enable the GPIO_LED Clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	/* Configure the GPIO_LASER pin */
	GPIO_InitStructure.GPIO_Pin = LASER_EN_IO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	laser_on();
}
