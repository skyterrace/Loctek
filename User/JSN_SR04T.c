#include "JSN_SR04T.h"
#include "delay.h"

volatile uint8_t US_TimUpdateCount;
void US_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//��ʱ��3��ʼ��
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	
	//Ԥ��Ƶֵ�ļ��㷽����ϵͳʱ��Ƶ��/TIM3����ʱ��Ƶ�� - 1
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 1000000) - 1;
	//ʹTIM6���Ƶ�ʵļ��㷽����TIM6����ʱ��Ƶ��/��ARR+1���������ARR����TIM_Period��ֵ�����1999�����TIM6����ʱ��Ƶ��Ϊ2K�������Ƶ��Ϊ1Hz
	TIM_TimeBaseStructure.TIM_Period = (uint16_t) (1000000/100) - 1;

	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //������ж�
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3,DISABLE);	
	//�˿ڳ�ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		//TRIG
	GPIO_InitStructure.GPIO_Pin = US1_TRIG_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(US1_TRIG_PORT, &GPIO_InitStructure);	
	//ECHO
	GPIO_InitStructure.GPIO_Pin = US1_ECHO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(US1_ECHO_PORT, &GPIO_InitStructure);	
	
	US_TimUpdateCount = 0;
	GPIO_ResetBits(US1_TRIG_PORT,US1_TRIG_PIN);
}

uint16_t US1_GetDistance(void)
{
	uint16_t dis;
	uint32_t temp;
	temp = 0;
	TIM_Cmd(TIM3,DISABLE);
	US_TimUpdateCount = 0;
	GPIO_SetBits(US1_TRIG_PORT,US1_TRIG_PIN);
	Delay_10us(2);
	GPIO_ResetBits(US1_TRIG_PORT,US1_TRIG_PIN);
	TIM_SetCounter(TIM3,0);	
	while(GPIO_ReadInputDataBit(US1_ECHO_PORT,US1_ECHO_PIN) == 0 && temp<1000)temp++;
	TIM_Cmd(TIM3,ENABLE);
	while(GPIO_ReadInputDataBit(US1_ECHO_PORT,US1_ECHO_PIN) == 1 && US_TimUpdateCount<10);
	TIM_Cmd(TIM3,DISABLE);
	if(temp >=1000 || US_TimUpdateCount>=10) return 0;
	dis = TIM_GetCounter(TIM3);
	temp = (uint32_t)US_TimUpdateCount*10000+dis;
	dis = temp * 344 /20000;
	
	return dis;
	
}
