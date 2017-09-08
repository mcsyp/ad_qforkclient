/*
 * File      : led.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006-2013, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-11-15     bright       the first version
 */

#include "adc_init.h"

rt_uint16_t rt_hw_ad_read(rt_uint8_t channel){
    rt_uint16_t value;
    rt_uint8_t timeout;
    
    ADC1->CHSELR = 0;
    ADC_ChannelConfig(ADC1, (rt_uint32_t)(1<<channel), ADC_SampleTime_239_5Cycles);
    ADC_StartOfConversion(ADC1);
    timeout = 0;
    while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET) {
        rt_thread_delay(1);
        timeout++;
        if (timeout>100)
            break;
    }
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    value = ADC_GetConversionValue(ADC1);
    ADC_StopOfConversion(ADC1);
    return value;
}
/* Initial led gpio pin  */
int rt_hw_ad_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef adc_init;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;//sharp_ir and battery
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	adc_init.ADC_Resolution = ADC_Resolution_12b;
	adc_init.ADC_ContinuousConvMode = DISABLE;
	adc_init.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	adc_init.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_TRGO;
	adc_init.ADC_DataAlign = ADC_DataAlign_Right;
	adc_init.ADC_ScanDirection = ADC_ScanDirection_Upward;

	ADC_Init(ADC1, &adc_init);
	ADC_GetCalibrationFactor(ADC1);
	ADC_Cmd(ADC1, ENABLE);   

	return RT_EOK;
}


/* Initial components for device */
//INIT_DEVICE_EXPORT(rt_hw_ad_init);
