#ifndef __GL5528_H
#define __GL5528_H

#define ADC1_DR_Address    ((u32)0x4001244C)

#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_gpio.h"

void ADC_Configuration(void); 
int16_t ADC_GetTemperature(void);
uint16_t ADC_GetLight(void);

#endif /* __ADC_H */
