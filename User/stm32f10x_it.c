/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "delay.h"
#include "USART.h"
#include "ZE08.h"
#include "ESP8266AT.h"
#include "JSN_SR04T.h"

extern volatile uint8_t FlagSecond;  //在main.c中定义的全局变量

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles USART1 interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)      //串口1中断服务
{
	uint8_t getchar;
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	   //判断读寄存器是否为空
  {	
    /* Read one byte from the receive data register */
    getchar = USART_ReceiveData(USART1);   //将读寄存器的数据缓存到接收缓冲区里
	
    USART_GetChar(getchar);
  }
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)                   //这段是为了避免STM32 USART第一个字节发送不出去的BUG 
  { 
     USART_ITConfig(USART1, USART_IT_TXE, DISABLE);					     //禁止发送缓冲器空中断
  }	
  
}

void USART2_IRQHandler(void)      //串口2中断服务
{
	uint8_t getchar;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)	   //判断读寄存器是否为空
  {	
    /* Read one byte from the receive data register */
    getchar = USART_ReceiveData(USART2);   //将读寄存器的数据缓存到接收缓冲区里
		ESP8266_GetChar(getchar);
  }
  
  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)                   //这段是为了避免STM32 USART第一个字节发送不出去的BUG 
  { 
     USART_ITConfig(USART2, USART_IT_TXE, DISABLE);					     //禁止发送缓冲器空中断
  }	
}

void USART3_IRQHandler(void)      //串口3中断服务
{
	uint8_t getchar;
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)	   //判断读寄存器是否为空
  {	
    /* Read one byte from the receive data register */
    getchar = USART_ReceiveData(USART3);   //将读寄存器的数据缓存到接收缓冲区里
	
    ZE08_GetChar(getchar);
  }
  
  if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)                   //这段是为了避免STM32 USART第一个字节发送不出去的BUG 
  { 
     USART_ITConfig(USART3, USART_IT_TXE, DISABLE);					     //禁止发送缓冲器空中断
  }	
  
}

void TIM3_IRQHandler(void)
{
	 //是否有更新中断
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{
		 //清除中断标志
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 
		//处理中断
		US_TimUpdateCount++;

	}
}

void TIM4_IRQHandler(void)
{
	 //是否有更新中断
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		 //清除中断标志
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update); 
		//处理中断
		FlagSecond = 1;

	}
}


/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
