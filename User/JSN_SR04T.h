#ifndef __JSN_SR04T_H
#define __JSN_SR04T_H
#include "stm32f10x.h"

#define US1_TRIG_PORT GPIOC
#define US1_TRIG_PIN GPIO_Pin_4

#define US1_ECHO_PORT GPIOC
#define US1_ECHO_PIN GPIO_Pin_6

extern volatile uint8_t US_TimUpdateCount;
void US_Init(void);
uint16_t US1_GetDistance(void);

#endif

