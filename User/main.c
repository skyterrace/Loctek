#include "stm32f10x.h"
#include "misc.h"
#include "delay.h"
#include "smbus.h"
#include "mlx90614.h"
#include "USART.h"
#include "ZE08.h"
#include "AM2320.h"
#include "GL5528.h"
#include "ZP01.h"
#include "LocTek.h"
//#include "ESP8266AT.h"
//#include "JSN_SR04T.h"

volatile uint8_t FlagSecond;

int16_t MLX90614_obj_celsius,MLX90614_amb_celsius;  //MLX90614Ŀ���¶Ⱥͱ����¶ȣ��Ŵ�100����
uint16_t CH2O_ppb;  //ZE08��ȩŨ�ȣ�ppb
int16_t AM2320_Temp;  //AM2320�¶�ֵ���Ŵ�10��
uint16_t AM2320_Humidity;  //AM2320ʪ��ֵ���Ŵ�10��
int16_t CPU_Temp; //�������¶ȴ�����
uint16_t GL5528_Light;
uint8_t ZP01_Grade; //ZP01�����ȼ�
uint16_t US1_Dis; //�������1����λ��cm

uint8_t UploadMode; //�����ϴ�ģʽ��0x81�ʴ�ģʽ��0x83�����ϴ�ģʽ
uint8_t FrameBuff[FRAME_BYTE_LENGTH-6];

//RCC_Configuration(), NVIC_Configuration(), GPIO_Config() ������ǰҪ����һ�¡�
void RCC_Configuration(void); //ϵͳʱ������
void NVIC_Configuration(void); //�ж�����
void GPIO_Config(void); //ͨ����������˿�����
void TIM4_Init(void);

int main(void)
{
	uint16_t temp_ambient, temp_object,i;
	
	RCC_Configuration(); //ʱ�ӳ�ʼ��
	GPIO_Config(); //�˿ڳ�ʼ��
	NVIC_Configuration();  //�жϳ�ʼ��
	
	SMBus_init();
	MLX90614_Init();
	MLX90614_TestConnection();
//	MLX90614_SetAddress(0x5A);	
	
//	AM2320_init();
	
	USART1_Init();
	ZE08_Init();
	ZE08_SetMode(ZE08_MODE_ASK);
	
	ADC_Configuration();
	ZP01_Init();
	
	//ESP8266_Init();
	//US_Init();
	
	UploadMode = UPLOAD_MODE_DIR; //Ĭ���������ϴ�ģʽ
	TIM4_Init();
	FlagSecond = 0;
	
	while(1)
	{
		if(FlagSecond == 1)
		{
			FlagSecond = 0;
			if(UploadMode == UPLOAD_MODE_DIR) //�����ϴ�ģʽ����ÿ1s�ϴ�һ������
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_8);   //��PA8��1
				
				//��ȡMLX90614����
				temp_ambient = MLX90614_ReadAmbientTemp();
				MLX90614_obj_celsius = temp_object*2-27315; //ת�������϶ȣ�����һ�ٱ�0.01
				temp_object = MLX90614_ReadObjectTemp();
				MLX90614_amb_celsius = temp_ambient*2-27315; //ת�������϶ȣ�����һ�ٱ�0.01
				
				//��ȡ��ȩŨ�ȣ�ppb
				CH2O_ppb = ZE08_GetResult(1);
				
				//��ȡ��ʪ��
				if(AM2320_get_sensor_data(0x00, 0x04, 0x08))  //��ʪ�ȴ�������ȡ�ɹ������þ���ֵ
					AM2320_get_RH_and_temperature(&AM2320_Humidity, &AM2320_Temp);	
				else //��ȡ���ɹ���PA8�øߣ������ã�ǰ���Ѿ��ø�
					GPIO_SetBits(GPIOA,GPIO_Pin_8); 
				
				//��ȡCPU�¶�
				CPU_Temp = ADC_GetTemperature();
				
				//��ȡGL5528��������
				GL5528_Light = ADC_GetLight();
				
				//��ȡ�ļ���������
				ZP01_Grade = ZP01_GetGrade();
				
				//ESP8266_SendAT("AT\r\n",4);
				
				//US1_Dis=US1_GetDistance();
				
				//��������
				for(i=0;i<FRAME_BYTE_LENGTH-6;i++)
				{
					FrameBuff[i]=0;
				}
				FrameBuff[0] = 0x83;
				FrameBuff[2] = CH2O_ppb&0xFF;
				FrameBuff[1] = CH2O_ppb>>8;
				FrameBuff[6] = MLX90614_amb_celsius&0xFF;
				FrameBuff[5] = MLX90614_amb_celsius>>8;
				FrameBuff[4] = MLX90614_obj_celsius&0xFF;
				FrameBuff[3] = MLX90614_obj_celsius>>8;
				FrameBuff[8] = AM2320_Temp&0xFF;
				FrameBuff[7] = AM2320_Temp>>8;
				FrameBuff[10] = AM2320_Humidity&0xFF;
				FrameBuff[9] = AM2320_Humidity>>8;
				FrameBuff[12] = GL5528_Light&0XFF;
				FrameBuff[11] = GL5528_Light>>8;
				FrameBuff[13] = 0x0F;  //����λ������״̬
				FrameBuff[13] |= (ZP01_Grade<<4);  //����λ������������ȼ�


				USART_SendFrame(USART1,FrameBuff,FRAME_BYTE_LENGTH-6); //�۳�֡ͷ��֡β��У�鹲6λ
				
				GPIO_ResetBits(GPIOA,GPIO_Pin_8);  //��PA8��0
			}
		}
//		AM2320_Temp = AM2320_ReadTemp();
		
		//����������
//		USART_Process();
//			USART_SendData(USART3,0xFF);
//		while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
//			USART_SendData(USART3,0x01);
//				while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
//			USART_SendData(USART3,0x86);
//				while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
//			USART_SendData(USART3,0x00);
//					while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
//			USART_SendData(USART3,0x00);
//					while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
//			USART_SendData(USART3,0x00);
//					while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
//			USART_SendData(USART3,0x00);
//					while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
//			USART_SendData(USART3,0x00);
//					while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
//			USART_SendData(USART3,0x79);
			
			
		//���߿���ֱ��ʹ������֡��־λ�Լ����ջ���������
		if(USART_FrameFlag == 1)
		{
			if(USART_Rx1Buff[2] == 0x01) //������л�����
			{
				if(USART_Rx1Buff[3] == UPLOAD_MODE_ASK || USART_Rx1Buff[3] == UPLOAD_MODE_DIR) 
					UploadMode = USART_Rx1Buff[3];
			}
			else if(USART_Rx1Buff[2] == 0x11 && UploadMode == UPLOAD_MODE_ASK)
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_8);   //��PA8��1
						
				//��ȡMLX90614����
				temp_ambient = MLX90614_ReadAmbientTemp();
				MLX90614_obj_celsius = temp_object*2-27315; //ת�������϶ȣ�����һ�ٱ�0.01
				temp_object = MLX90614_ReadObjectTemp();
				MLX90614_amb_celsius = temp_ambient*2-27315; //ת�������϶ȣ�����һ�ٱ�0.01
				
				//��ȡ��ȩŨ�ȣ�ppb
				CH2O_ppb = ZE08_GetResult(1);
				
				//��ȡ��ʪ��
				if(AM2320_get_sensor_data(0x00, 0x04, 0x08))  //��ʪ�ȴ�������ȡ�ɹ������þ���ֵ
					AM2320_get_RH_and_temperature(&AM2320_Humidity, &AM2320_Temp);	
				else //��ȡ���ɹ���PA8�øߣ������ã�ǰ���Ѿ��ø�
					GPIO_SetBits(GPIOA,GPIO_Pin_8); 
				
				//��ȡCPU�¶�
				CPU_Temp = ADC_GetTemperature();
				
				//��ȡGL5528��������
				GL5528_Light = ADC_GetLight();
				
				//��ȡ�ļ���������
				ZP01_Grade = ZP01_GetGrade();
				
				//��������
				for(i=0;i<FRAME_BYTE_LENGTH-6;i++)
				{
					FrameBuff[i]=0;
				}
				FrameBuff[0] = 0x81;
				FrameBuff[2] = CH2O_ppb&0xFF;
				FrameBuff[1] = CH2O_ppb>>8;
				FrameBuff[6] = MLX90614_amb_celsius&0xFF;
				FrameBuff[5] = MLX90614_amb_celsius>>8;
				FrameBuff[4] = MLX90614_obj_celsius&0xFF;
				FrameBuff[3] = MLX90614_obj_celsius>>8;
				FrameBuff[8] = AM2320_Temp&0xFF;
				FrameBuff[7] = AM2320_Temp>>8;
				FrameBuff[10] = AM2320_Humidity&0xFF;
				FrameBuff[9] = AM2320_Humidity>>8;
				FrameBuff[12] = GL5528_Light&0XFF;
				FrameBuff[11] = GL5528_Light>>8;
				FrameBuff[13] = 0x0F;  //����λ������״̬
				FrameBuff[13] |= (ZP01_Grade<<4);  //����λ������������ȼ�
				USART_SendFrame(USART1,FrameBuff,FRAME_BYTE_LENGTH-6); //�۳�֡ͷ��֡β��У�鹲6λ
				
				GPIO_ResetBits(GPIOA,GPIO_Pin_8);  //��PA8��0
			}
			
			USART_FrameFlag = 0;
			
		}
		
	}
	
}

/*ϵͳʱ�����ú���*/
void RCC_Configuration(void)
{
	
//        SystemInit();//Դ��system_stm32f10x.c�ļ�,ֻ��Ҫ���ô˺���,������RCC��Ĭ������.�����뿴2_RCC
	
				//���ǻ����Լ������ð�
	      RCC_DeInit();

				RCC_HSEConfig(RCC_HSE_ON);  //ʹ���ⲿ8MHz����
        
        while(RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)        
        {        
        }

//        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

//        FLASH_SetLatency(FLASH_Latency_2);
				
        /* HCLK = SYSCLK */
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        //APB2
        RCC_PCLK2Config(RCC_HCLK_Div1);
        //APB1
        RCC_PCLK1Config(RCC_HCLK_Div2);
        //PLL ��Ƶ
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);        //�ⲿ����*9���ⲿ����8MHz������PLL����Ƶ9����PLL���72MHz
        RCC_PLLCmd(ENABLE);                        										//ʹ�ܱ�Ƶ
                                                                                                         
				//�ȴ�ָ���� RCC ��־λ���óɹ� �ȴ�PLL��ʼ���ɹ�
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);        //��PLL�����Ϊϵͳʱ�ӡ�

        /**************************************************
        ��ȡRCC����Ϣ,������
        ��ο�RCC_ClocksTypeDef�ṹ�������,��ʱ��������ɺ�,
        ���������ֵ��ֱ�ӷ�ӳ�������������ֵ�����Ƶ��
        ***************************************************/
                
        while(RCC_GetSYSCLKSource() != 0x08){}		//����0x08˵��ʹ��PLL��Ϊϵͳʱ�ӳɹ�
					
}

/*�ж����ú���*/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;  //NVIC�ж������ṹ��
	RCC_ClocksTypeDef RCC_Clocks;  //RCCʱ�ӽṹ��
  /* Configure the NVIC Preemption Priority Bits */  
  /* Configure one bit for preemption priority */
  /* ���ȼ��� ˵������ռ���ȼ����õ�λ�����������ȼ����õ�λ����������2��2*/    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		   
  

	  /* Enable the RTC Interrupt ʹ��ʵʱʱ���ж�*/
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;					//�����ⲿ�ж�Դ�����жϣ� 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
	
	/*ʹ�ܶ�ʱ�ж�*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;												//ָ���ж�Դ
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;							
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;										//ָ����Ӧ���ȼ���
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;												//ָ���ж�Դ
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;							
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;										//ָ����Ӧ���ȼ���
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure); 
	//ʹ��USART1�ж�
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	
	
	//ʹ��USART3�ж�
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);		
	
	//�������ʱ���ж�
	RCC_GetClocksFreq(&RCC_Clocks);		//��ȡϵͳʱ��
	if (SysTick_Config(RCC_Clocks.SYSCLK_Frequency/100000))		   //ʱ�ӽ����ж�ʱ10usһ��  ���ڶ�ʱ�� ����ʱ��Ƶ��48MHz��/480����10usһ��
  { 
    /* Capture error */ 
    while (1);
  } 
}

/*�˿����ú���*/
void GPIO_Config(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//ʹ��GPIOA/GPIOD�����ߣ�����˿ڲ��ܹ����������������˿ڣ����Բ���ʹ�ܡ�
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);	

	//����PA8ΪLED0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				     
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			 //���߷�ת�ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	
	//����PD2ΪLED1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //PD2	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			 //���߷�ת�ٶ�Ϊ50MHz
  GPIO_Init(GPIOD, &GPIO_InitStructure);	
	
//	/*ʹ��  USART1 ʱ�� */
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
//	/* USART1 GPIO config */ 
//	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	/* Configure USART1 Rx (PA.10) as input floating */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

void TIM4_Init(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	
	//Ԥ��Ƶֵ�ļ��㷽����ϵͳʱ��Ƶ��/TIM6����ʱ��Ƶ�� - 1
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 2000) - 1;
	//ʹTIM6���Ƶ�ʵļ��㷽����TIM6����ʱ��Ƶ��/��ARR+1���������ARR����TIM_Period��ֵ�����1999�����TIM6����ʱ��Ƶ��Ϊ2K�������Ƶ��Ϊ1Hz
	TIM_TimeBaseStructure.TIM_Period = (uint16_t) (2000/1) - 1;

	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //������ж�
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_Cmd(TIM4,ENABLE);
}
