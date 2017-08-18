#ifndef __USART_H__
#define __USART_H__
#include "stm32f10x.h"
#include "stdio.h"
#include "stdarg.h"

//使用usart1，对应PA9/TX，PA10/RX，
//端口初始化在USART.c文件中的USART1_Init函数中；
//中断设置在main.c文件中的RCC_Configuration函数中，
//中断响应在stm32f10x_it.c文件中USART1_IRQHandler函数中

#define FRAME_BYTE_LENGTH 20 //串口通讯一帧数据的字节数（含帧头和帧尾），譬如20个字节为一个完整的数据帧，第3个字节代表命令类型，第4~16字节是命令参数，第17、18两个字节为CRC校验码
#define FRAME_START 0xA5A5 //帧头，两个字节
#define FRAME_END 0x5A5A  //帧尾，两个字节

//这里把数据帧接收标志和接收缓冲区定义成可供usart.c以外的源代码直接使用，只要include了USART.h文件。
extern __IO uint8_t USART_Rx1Buff[FRAME_BYTE_LENGTH]; //接收缓冲区
extern __IO uint8_t USART_Tx1Buff[FRAME_BYTE_LENGTH]; //发送缓冲区
extern __IO uint8_t USART_FrameFlag; //接收完整数据帧标志，1完整，0不完整

void USART1_Init(void);
void USART3_Init(void);
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);
char *itoa(int value, char *string, int radix);
int fputc(int ch, FILE *f);
void USART_GetChar(uint8_t nChar); //串口接收到一个字节
void USART_Process(void);
void USART_SendFrame(USART_TypeDef* USARTx,uint8_t *Buff, uint8_t nLen); //发送一帧数据

#endif
