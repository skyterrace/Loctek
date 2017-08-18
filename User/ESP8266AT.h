#ifndef __ESP8266AT_H
#define __ESP8266AT_H
#include "stm32f10x.h"

#define ESP8266_RST_PORT GPIOA
#define ESP8266_RST_PIN GPIO_Pin_4

#define ESP8266_EN_PORT GPIOA
#define ESP8266_EN_PIN GPIO_Pin_5

#define ESP8266_GPIO0_PORT GPIOB
#define ESP8266_GPIO0_PIN GPIO_Pin_2

#define ESP8266_GPIO15_PORT GPIOA
#define ESP8266_GPIO15_PIN GPIO_Pin_0

void ESP8266_Init(void);
int8_t ESP8266_SendAT(uint8_t *cmd, unsigned int nLen);
void ESP8266_GetChar(uint8_t nChar);

#endif
