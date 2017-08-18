#include "USART.h"
#include "LocTek.h"

__IO uint8_t nRx1Counter=0; //接收字节数
__IO uint8_t USART_Rx1Buff[FRAME_BYTE_LENGTH]; //接收缓冲区
__IO uint8_t USART_Tx1Buff[FRAME_BYTE_LENGTH]; //发送缓冲区
__IO uint8_t USART_FrameFlag = 0; //接收完整数据帧标志，1完整，0不完整
void USART1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	/* USART1 GPIO config */ 
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = 115200;
//	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE); 

	USART_Cmd(USART1, ENABLE);
}

void USART3_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	/* config USART1 clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/* USART3 GPIO config */ 
	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* Configure USART3 Rx (PB.11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* USART3 mode config */
	USART_InitStructure.USART_BaudRate = 9600;
//	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	
	/* Enable USART1 Receive and Transmit interrupts */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(USART3, USART_IT_TXE, ENABLE); 

	USART_Cmd(USART3, ENABLE);
}

/******************************************************
		格式化串口输出函数
        "\r"	回车符	   USART_OUT(USART1, "abcdefg\r")   
		"\n"	换行符	   USART_OUT(USART1, "abcdefg\r\n")
		"%s"	字符串	   USART_OUT(USART1, "字符串是：%s","abcdefg")
		"%d"	十进制	   USART_OUT(USART1, "a=%d",10)
**********************************************************/
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...){ 
	const char *s;
    int d;
    char buf[16];
    va_list ap;
    va_start(ap, Data);

	while(*Data!=0){				                          //判断是否到达字符串结束符
		if(*Data==0x5c){									  //'\'
			switch (*++Data){
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);	   
					Data++;
					break;
				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data++;
					break;
				
				default:
					Data++;
				    break;
			}
	
			 
		}
		else if(*Data=='%'){									  //
			switch (*++Data){				
				case 's':										  //字符串
                	s = va_arg(ap, const char *);
                	for ( ; *s; s++) {
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
            	case 'd':										  //十进制
                	d = va_arg(ap, int);
                	itoa(d, buf, 10);
                	for (s = buf; *s; s++) {
                    	USART_SendData(USARTx,*s);
						while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
                	}
					Data++;
                	break;
				default:
					Data++;
				    break;
			}		 
		}
		else USART_SendData(USARTx, *Data++);
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	}
}
/******************************************************
		整形数据转字符串函数
        char *itoa(int value, char *string, int radix)
		radix=10 标示是10进制	非十进制，转换结果为0;  

	    例：d=-379;
		执行	itoa(d, buf, 10); 后
		
		buf="-379"							   			  
**********************************************************/
char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

//重定向printf到串口
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (unsigned char) ch);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
	
// 	USART_SendData(USART2, (unsigned char) ch);
// 	while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);
	return (ch);
}

void USART_GetChar(uint8_t nChar) //串口接收到一个字节
{
	if(USART_FrameFlag == 1) return;   //如果上次的数据帧还没处理过，则返回
	
	if(nRx1Counter==0 && nChar == (FRAME_START>>8))
	{
		USART_Rx1Buff[nRx1Counter++]=nChar;  //保存到缓冲区
	}
	else if(nRx1Counter==1 && nChar == (FRAME_START&0xFF))
	{
		USART_Rx1Buff[nRx1Counter++]=nChar;  //保存到缓冲区
	}
	else if(nRx1Counter>1) //接收到帧头以后才继续保存
	{
		USART_Rx1Buff[nRx1Counter++]=nChar;  //保存到缓冲区
		if(nRx1Counter>=FRAME_BYTE_LENGTH)  //接收到一帧数据
		{
			nRx1Counter = 0;
			if(USART_Rx1Buff[FRAME_BYTE_LENGTH-2] == (FRAME_END>>8) && USART_Rx1Buff[FRAME_BYTE_LENGTH-1] == (FRAME_END&0xFF)) //如果最后两个字节是帧尾，则数据帧完整
			{
				USART_FrameFlag=1;
			}
		}
	}
}

void USART_Process(void) //处理数据帧
{
	uint8_t i;
	if(USART_FrameFlag == 1)
	{
		//将数据原封不动发送回去
		for(i=0;i<FRAME_BYTE_LENGTH;i++)
		{
			USART_SendData(USART1,USART_Rx1Buff[i]);
		}
		//处理完毕，将标志清0
		USART_FrameFlag = 0; 
	}
}

//发送一帧数据
void USART_SendFrame(USART_TypeDef* USARTx,uint8_t *Buff, uint8_t nLen)
{
	//计算CRC16
	uint16_t nCRC,i;
	nCRC = CRC16_calc(Buff,nLen);
		
	//发送帧头
	USART_SendData(USARTx, FRAME_START>>8);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
	USART_SendData(USARTx,FRAME_START&0x00FF);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
	
	//发送数据
	for(i=0;i<nLen;i++)
	{
		USART_SendData(USARTx,*Buff++);
		while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
	}
	
	//发送校验码
	USART_SendData(USARTx, nCRC>>8);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
	USART_SendData(USARTx, nCRC&0xFF);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
	
	//发送帧尾
	USART_SendData(USARTx, FRAME_END>>8);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
	USART_SendData(USARTx,FRAME_END&0x00FF);
	while( USART_GetFlagStatus(USART1,USART_FLAG_TC)!= SET);
	
}
