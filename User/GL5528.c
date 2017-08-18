#include "GL5528.h"

uint16_t ADC_ConvertedValue[10];

/****************************************************************************
* 名    称：void ADC_Configuration(void)
* 功    能：ADC 配置函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/ 
void ADC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	//配置PC0为模拟输入端口
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* Enable DMA clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  	/* DMA channel1 configuration ----------------------------------------------*/
	//使能DMA
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;			            //DMA通道1的地址 
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue[0];	            //DMA传送地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;					            //传送方向
	DMA_InitStructure.DMA_BufferSize = 10;								            //传送内存大小，100个16位
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	 
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;				            //传送内存地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;		//ADC1转换的数据是16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;				//传送的目的地址是16位宽度
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;									//循环
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
    
	/* 允许DMA1通道1传输结束中断 */
	//DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);


	//使能DMA通道1
	DMA_Cmd(DMA1_Channel1, ENABLE); 
  
	//ADC配置
	/* Resets ADC1 */ 
	ADC_DeInit(ADC1);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC1工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;		//模数转换工作在扫描模式（多通道）还是单次（单通道）模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//模数转换工作在扫描模式（多通道）还是单次（单通道）模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 2;               //规定了顺序进行规则转换的ADC通道的数目。这个数目的取值范围是1到16
	ADC_Init(ADC1, &ADC_InitStructure);
	
	/* ADC1 regular channels configuration [规则模式通道配置]*/ 

	//ADC1 规则通道配置
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_239Cycles5);	  //通道16样时间 239.5周期
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 2, ADC_SampleTime_239Cycles5);	  //通道16样时间 239.5周期
	
	ADC_TempSensorVrefintCmd(ENABLE); 

	//使能ADC1 DMA 
	ADC_DMACmd(ADC1, ENABLE);
	//使能ADC1
	ADC_Cmd(ADC1, ENABLE);	
	
	// 初始化ADC1校准寄存器
	ADC_ResetCalibration(ADC1);
	//检测ADC1校准寄存器初始化是否完成
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	//开始校准ADC1
	ADC_StartCalibration(ADC1);
	//检测是否完成校准
	while(ADC_GetCalibrationStatus(ADC1));
	
	//ADC1转换启动
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	 
}

int16_t ADC_GetTemperature(void)
{
	//ADC转换值DMA缓冲区
	uint16_t tmin,tmax;
	int16_t temp,i;
	uint16_t val;
	
	
	val=tmin=tmax=ADC_ConvertedValue[0];

	for(i=1;i<5;i++) 
	{
		val+=ADC_ConvertedValue[2*i];
		if(ADC_ConvertedValue[2*i]> tmax)tmax=ADC_ConvertedValue[2*i];
		if(ADC_ConvertedValue[2*i]< tmin)tmin=ADC_ConvertedValue[2*i];
	}
	val = (val-tmin-tmax)/3;
	temp = (0x6EF-val)*10 / 43 + 25;
// 	temp = (1.43-val*3/4096)*1000 / 4.3 + 25;
// 	temp = (0x6EF- ADC_ConvertedValue[0])*10 / 0x2B + 25;
	return temp;
}

uint16_t ADC_GetLight(void)
{
	//ADC转换值DMA缓冲区
	uint16_t vmin,vmax;
	uint16_t temp,i;
	uint32_t temp_r;
	uint16_t val;
	
	val=vmin=vmax=ADC_ConvertedValue[1];

	for(i=1;i<5;i++) 
	{
		val+=ADC_ConvertedValue[2*i+1];
		if(ADC_ConvertedValue[2*i+1]> vmax)vmax=ADC_ConvertedValue[2*i+1];
		if(ADC_ConvertedValue[2*i+1]< vmin)vmin=ADC_ConvertedValue[2*i+1];
	}
	val = (val-vmin-vmax)/3;

	temp_r = val * 3300;
	temp = temp_r/4096;  //电压值，mV
	//计算光敏电阻值，0.01KOhm
	temp_r = 10000*4096;
	temp_r =  temp_r / val -10000;
	if(temp_r > 650000) temp = 65000;  //超过650K，就算650K
	else temp = temp_r/10;  //转换到0.01KOhm，否则超出表示范围

	return temp;
}
