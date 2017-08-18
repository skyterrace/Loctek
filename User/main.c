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

int16_t MLX90614_obj_celsius,MLX90614_amb_celsius;  //MLX90614目标温度和背景温度，放大100倍。
uint16_t CH2O_ppb;  //ZE08甲醛浓度，ppb
int16_t AM2320_Temp;  //AM2320温度值，放大10倍
uint16_t AM2320_Humidity;  //AM2320湿度值，放大10倍
int16_t CPU_Temp; //处理器温度传感器
uint16_t GL5528_Light;
uint8_t ZP01_Grade; //ZP01空气等级
uint16_t US1_Dis; //超声测距1，单位：cm

uint8_t UploadMode; //数据上传模式，0x81问答模式，0x83主动上传模式
uint8_t FrameBuff[FRAME_BYTE_LENGTH-6];

//RCC_Configuration(), NVIC_Configuration(), GPIO_Config() 在引用前要声明一下。
void RCC_Configuration(void); //系统时钟配置
void NVIC_Configuration(void); //中断配置
void GPIO_Config(void); //通用输入输出端口配置
void TIM4_Init(void);

int main(void)
{
	uint16_t temp_ambient, temp_object,i;
	
	RCC_Configuration(); //时钟初始化
	GPIO_Config(); //端口初始化
	NVIC_Configuration();  //中断初始化
	
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
	
	UploadMode = UPLOAD_MODE_DIR; //默认是主动上传模式
	TIM4_Init();
	FlagSecond = 0;
	
	while(1)
	{
		if(FlagSecond == 1)
		{
			FlagSecond = 0;
			if(UploadMode == UPLOAD_MODE_DIR) //主动上传模式，则每1s上传一次数据
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_8);   //把PA8置1
				
				//读取MLX90614体温
				temp_ambient = MLX90614_ReadAmbientTemp();
				MLX90614_obj_celsius = temp_object*2-27315; //转换成摄氏度，扩大一百倍0.01
				temp_object = MLX90614_ReadObjectTemp();
				MLX90614_amb_celsius = temp_ambient*2-27315; //转换成摄氏度，扩大一百倍0.01
				
				//读取甲醛浓度：ppb
				CH2O_ppb = ZE08_GetResult(1);
				
				//读取温湿度
				if(AM2320_get_sensor_data(0x00, 0x04, 0x08))  //温湿度传感器读取成功，则获得具体值
					AM2320_get_RH_and_temperature(&AM2320_Humidity, &AM2320_Temp);	
				else //读取不成功，PA8置高，调试用，前面已经置高
					GPIO_SetBits(GPIOA,GPIO_Pin_8); 
				
				//读取CPU温度
				CPU_Temp = ADC_GetTemperature();
				
				//读取GL5528光敏电阻
				GL5528_Light = ADC_GetLight();
				
				//读取四级空气质量
				ZP01_Grade = ZP01_GetGrade();
				
				//ESP8266_SendAT("AT\r\n",4);
				
				//US1_Dis=US1_GetDistance();
				
				//发送数据
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
				FrameBuff[13] = 0x0F;  //低四位传感器状态
				FrameBuff[13] |= (ZP01_Grade<<4);  //高四位保存空气质量等级


				USART_SendFrame(USART1,FrameBuff,FRAME_BYTE_LENGTH-6); //扣除帧头、帧尾和校验共6位
				
				GPIO_ResetBits(GPIOA,GPIO_Pin_8);  //把PA8置0
			}
		}
//		AM2320_Temp = AM2320_ReadTemp();
		
		//处理串口数据
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
			
			
		//或者可以直接使用完整帧标志位以及接收缓冲区变量
		if(USART_FrameFlag == 1)
		{
			if(USART_Rx1Buff[2] == 0x01) //如果是切换命令
			{
				if(USART_Rx1Buff[3] == UPLOAD_MODE_ASK || USART_Rx1Buff[3] == UPLOAD_MODE_DIR) 
					UploadMode = USART_Rx1Buff[3];
			}
			else if(USART_Rx1Buff[2] == 0x11 && UploadMode == UPLOAD_MODE_ASK)
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_8);   //把PA8置1
						
				//读取MLX90614体温
				temp_ambient = MLX90614_ReadAmbientTemp();
				MLX90614_obj_celsius = temp_object*2-27315; //转换成摄氏度，扩大一百倍0.01
				temp_object = MLX90614_ReadObjectTemp();
				MLX90614_amb_celsius = temp_ambient*2-27315; //转换成摄氏度，扩大一百倍0.01
				
				//读取甲醛浓度：ppb
				CH2O_ppb = ZE08_GetResult(1);
				
				//读取温湿度
				if(AM2320_get_sensor_data(0x00, 0x04, 0x08))  //温湿度传感器读取成功，则获得具体值
					AM2320_get_RH_and_temperature(&AM2320_Humidity, &AM2320_Temp);	
				else //读取不成功，PA8置高，调试用，前面已经置高
					GPIO_SetBits(GPIOA,GPIO_Pin_8); 
				
				//读取CPU温度
				CPU_Temp = ADC_GetTemperature();
				
				//读取GL5528光敏电阻
				GL5528_Light = ADC_GetLight();
				
				//读取四级空气质量
				ZP01_Grade = ZP01_GetGrade();
				
				//发送数据
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
				FrameBuff[13] = 0x0F;  //低四位传感器状态
				FrameBuff[13] |= (ZP01_Grade<<4);  //高四位保存空气质量等级
				USART_SendFrame(USART1,FrameBuff,FRAME_BYTE_LENGTH-6); //扣除帧头、帧尾和校验共6位
				
				GPIO_ResetBits(GPIOA,GPIO_Pin_8);  //把PA8置0
			}
			
			USART_FrameFlag = 0;
			
		}
		
	}
	
}

/*系统时钟配置函数*/
void RCC_Configuration(void)
{
	
//        SystemInit();//源自system_stm32f10x.c文件,只需要调用此函数,则可完成RCC的默认配置.具体请看2_RCC
	
				//我们还是自己来配置吧
	      RCC_DeInit();

				RCC_HSEConfig(RCC_HSE_ON);  //使用外部8MHz晶振
        
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
        //PLL 倍频
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);        //外部晶振*9，外部晶振8MHz，进入PLL，倍频9，则PLL输出72MHz
        RCC_PLLCmd(ENABLE);                        										//使能倍频
                                                                                                         
				//等待指定的 RCC 标志位设置成功 等待PLL初始化成功
        while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
        {
        }

        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);        //以PLL输出作为系统时钟。

        /**************************************************
        获取RCC的信息,调试用
        请参考RCC_ClocksTypeDef结构体的内容,当时钟配置完成后,
        里面变量的值就直接反映了器件各个部分的运行频率
        ***************************************************/
                
        while(RCC_GetSYSCLKSource() != 0x08){}		//返回0x08说明使用PLL作为系统时钟成功
					
}

/*中断配置函数*/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;  //NVIC中断向量结构体
	RCC_ClocksTypeDef RCC_Clocks;  //RCC时钟结构体
  /* Configure the NVIC Preemption Priority Bits */  
  /* Configure one bit for preemption priority */
  /* 优先级组 说明了抢占优先级所用的位数，和子优先级所用的位数。这里是2，2*/    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		   
  

	  /* Enable the RTC Interrupt 使能实时时钟中断*/
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;					//配置外部中断源（秒中断） 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
	
	/*使能定时中断*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;												//指定中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;							
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;										//指定响应优先级别
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;												//指定中断源
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;							
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;										//指定响应优先级别
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	
	NVIC_Init(&NVIC_InitStructure); 
	//使能USART1中断
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);	
	
	//使能USART3中断
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);		
	
	//设置嘀嗒时钟中断
	RCC_GetClocksFreq(&RCC_Clocks);		//获取系统时钟
	if (SysTick_Config(RCC_Clocks.SYSCLK_Frequency/100000))		   //时钟节拍中断时10us一次  用于定时。 例如时钟频率48MHz，/480，则10us一次
  { 
    /* Capture error */ 
    while (1);
  } 
}

/*端口配置函数*/
void GPIO_Config(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//使能GPIOA/GPIOD的总线，否则端口不能工作，如果不用这个端口，可以不用使能。
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);	

	//配置PA8为LED0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;				     
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			 //口线翻转速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	
	//配置PD2为LED1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //PD2	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;				//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			 //口线翻转速度为50MHz
  GPIO_Init(GPIOD, &GPIO_InitStructure);	
	
//	/*使能  USART1 时钟 */
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
	//预分频值的计算方法：系统时钟频率/TIM6计数时钟频率 - 1
	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 2000) - 1;
	//使TIM6溢出频率的计算方法：TIM6计数时钟频率/（ARR+1），这里的ARR就是TIM_Period的值，设成1999，如果TIM6计数时钟频率为2K，则溢出频率为1Hz
	TIM_TimeBaseStructure.TIM_Period = (uint16_t) (2000/1) - 1;

	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //打开溢出中断
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_Cmd(TIM4,ENABLE);
}
