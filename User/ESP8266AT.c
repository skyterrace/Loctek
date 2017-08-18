#include "ESP8266AT.h"
#include "delay.h"

uint8_t ESP8266_Rx_Count;
uint8_t ESP8266_Rx_Buff[64];
void ESP8266_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;  //NVIC中断向量结构体
	
	/* config USART2 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 ,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
	
	//WIFI_RST:PA4
	GPIO_InitStructure.GPIO_Pin = ESP8266_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESP8266_RST_PORT, &GPIO_InitStructure);	
	//WIFI_EN:PA5
	GPIO_InitStructure.GPIO_Pin = ESP8266_EN_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESP8266_EN_PORT, &GPIO_InitStructure);	
	//WIFI_GPIO0:PB2
	GPIO_InitStructure.GPIO_Pin = ESP8266_GPIO0_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESP8266_GPIO0_PORT, &GPIO_InitStructure);	
	//WIFI_GPIO15:PA0
	GPIO_InitStructure.GPIO_Pin = ESP8266_GPIO15_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ESP8266_GPIO15_PORT, &GPIO_InitStructure);	
	
	/* USART2 GPIO config */ 
	/* Configure USART2 Tx (PA.02) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USART2 Rx (PA.03) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
//	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	
	/* Enable USART2 Receive and Transmit interrupts */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 

	USART_Cmd(USART2, ENABLE);
	
	//中断配置，使能USART2中断
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	

  ESP8266_Rx_Count = 0;
	
	//Flash Boot模式：GPIO0=1，GPIO15=0
	GPIO_SetBits(ESP8266_GPIO0_PORT,ESP8266_GPIO0_PIN);
	GPIO_ResetBits(ESP8266_GPIO15_PORT,ESP8266_GPIO15_PIN);
	
	//reset一下esp8266
	GPIO_SetBits(ESP8266_RST_PORT,ESP8266_RST_PIN);
	GPIO_ResetBits(ESP8266_EN_PORT,ESP8266_EN_PIN);
	Delay_ms(1);
	GPIO_SetBits(ESP8266_EN_PORT,ESP8266_EN_PIN);
	Delay_ms(1);
	GPIO_ResetBits(ESP8266_RST_PORT,ESP8266_RST_PIN);	
	Delay_ms(1);
	GPIO_SetBits(ESP8266_RST_PORT,ESP8266_RST_PIN);
	
}

int8_t ESP8266_SendAT(uint8_t *cmd, unsigned int nLen)
{
	uint8_t i;
	for(i=0;i<nLen;i++)
	{
		USART_SendData(USART2, *cmd);
		while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);
		cmd++;
	}
	
//			USART_SendData(USART2, 'A');
//		while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);
//				USART_SendData(USART2, 'T');
//		while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);
	return 1;
}

void ESP8266_GetChar(uint8_t nChar) //串口接收到一个字节
{
	if(ESP8266_Rx_Count>63)
		ESP8266_Rx_Count = 0;
	
	if(nChar != 0x0A && nChar != 0x0D )
	{
		ESP8266_Rx_Buff[ESP8266_Rx_Count++]=nChar;  //保存到缓冲区
	}
	else
	{
		ESP8266_Rx_Buff[ESP8266_Rx_Count++]=nChar; 
	}

}
