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

extern volatile uint8_t FlagSecond;  //��main.c�ж����ȫ�ֱ���

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
void USART1_IRQHandler(void)      //����1�жϷ���
{
	uint8_t getchar;
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	   //�ж϶��Ĵ����Ƿ�Ϊ��
  {	
    /* Read one byte from the receive data register */
    getchar = USART_ReceiveData(USART1);   //�����Ĵ��������ݻ��浽���ջ�������
	
    USART_GetChar(getchar);
  }
  
  if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)                   //�����Ϊ�˱���STM32 USART��һ���ֽڷ��Ͳ���ȥ��BUG 
  { 
     USART_ITConfig(USART1, USART_IT_TXE, DISABLE);					     //��ֹ���ͻ��������ж�
  }	
  
}

void USART2_IRQHandler(void)      //����2�жϷ���
{
	uint8_t getchar;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)	   //�ж϶��Ĵ����Ƿ�Ϊ��
  {	
    /* Read one byte from the receive data register */
    getchar = USART_ReceiveData(USART2);   //�����Ĵ��������ݻ��浽���ջ�������
		ESP8266_GetChar(getchar);
  }
  
  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)                   //�����Ϊ�˱���STM32 USART��һ���ֽڷ��Ͳ���ȥ��BUG 
  { 
     USART_ITConfig(USART2, USART_IT_TXE, DISABLE);					     //��ֹ���ͻ��������ж�
  }	
}

void USART3_IRQHandler(void)      //����3�жϷ���
{
	uint8_t getchar;
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)	   //�ж϶��Ĵ����Ƿ�Ϊ��
  {	
    /* Read one byte from the receive data register */
    getchar = USART_ReceiveData(USART3);   //�����Ĵ��������ݻ��浽���ջ�������
	
    ZE08_GetChar(getchar);
  }
  
  if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)                   //�����Ϊ�˱���STM32 USART��һ���ֽڷ��Ͳ���ȥ��BUG 
  { 
     USART_ITConfig(USART3, USART_IT_TXE, DISABLE);					     //��ֹ���ͻ��������ж�
  }	
  
}

void TIM3_IRQHandler(void)
{
	 //�Ƿ��и����ж�
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{
		 //����жϱ�־
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update); 
		//�����ж�
		US_TimUpdateCount++;

	}
}

void TIM4_IRQHandler(void)
{
	 //�Ƿ��и����ж�
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)
	{
		 //����жϱ�־
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update); 
		//�����ж�
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
