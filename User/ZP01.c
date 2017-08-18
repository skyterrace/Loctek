#include "ZP01.h"
#include "stm32f10x_gpio.h"
void ZP01_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//使能GPIOC的总线，否则端口不能工作，如果不用这个端口，可以不用使能。
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	

	//配置PC8/PC9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;				     
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			 //口线翻转速度为50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

uint8_t ZP01_GetGrade(void)
{
	uint8_t temp;
	temp = 0;
	temp = GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9); 
	temp = temp << 1;
	temp = temp | GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8); 
	return temp;
}
