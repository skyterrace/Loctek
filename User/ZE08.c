#include "ZE08.h"
#include "USART.h"
#include "delay.h"
uint8_t ZE08_Rx[ZE08_FRAME_LENGTH];
uint8_t ZE08_Rx_Counter,ZE08_Mode;
uint16_t ZE08_Result_ppb;
uint16_t ZE08_Result_ug;
uint16_t ZE08_MAX;
void ZE08_Init(void)
{
	USART3_Init();
	ZE08_Rx_Counter = 0;
	ZE08_Mode = 0;
}

//设置ZE08传输模式，0-主动上传，1-问答上传
void ZE08_SetMode(uint8_t nMode)
{
	uint8_t i;
	uint8_t ZE08_Tx[ZE08_FRAME_LENGTH];
	ZE08_Tx[0] = 0xFF;
	ZE08_Tx[1] = 0x01;
	ZE08_Tx[2] = 0x78;
	ZE08_Tx[3] = 0x40+nMode;
	ZE08_Tx[4] = 0x00;
	ZE08_Tx[5] = 0x00;
	ZE08_Tx[6] = 0x00;
	ZE08_Tx[7] = 0x00;
	ZE08_Tx[8] = 0x47-nMode;
	for(i=0;i<ZE08_FRAME_LENGTH;i++)
	{
		USART_SendData(USART3,ZE08_Tx[i]);
		while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
	}
	ZE08_Mode = nMode;
}

//ZE08串口接收字符处理函数
void ZE08_GetChar(uint8_t getchar)
{
	ZE08_Rx[ZE08_Rx_Counter++] = getchar;	
	if(ZE08_Rx_Counter>=ZE08_FRAME_LENGTH)
	{
		if(ZE08_Rx[0] == 0xFF)  //这是一个完整的帧
		{
			ZE08_Rx_Counter=0;
			if(ZE08_CheckSum(ZE08_Rx,ZE08_FRAME_LENGTH) == ZE08_Rx[ZE08_FRAME_LENGTH-1])  //校验正确，则保存检测数据
			{
				if(ZE08_Mode == ZE08_MODE_ASK)
				{
					ZE08_Result_ug = (ZE08_Rx[2]<<8) + ZE08_Rx[3];
					ZE08_Result_ppb = (ZE08_Rx[6]<<8) + ZE08_Rx[7];
				}
				else
				{
					ZE08_Result_ppb = (ZE08_Rx[4]<<8) + ZE08_Rx[5];
					ZE08_MAX = (ZE08_Rx[6]<<8) + ZE08_Rx[7];
				}
			}
		}
		else if(getchar == 0xFF) //如果当前是0xFF，就认为是新帧来了。
		{
			ZE08_Rx[0] = getchar;
			ZE08_Rx_Counter=1;
		}
		else
		{
			ZE08_Rx_Counter=ZE08_FRAME_LENGTH-1;
		}
	}

}

/**********************************************************************
* 函数名: unsigned char FucCheckSum(uchar *i,ucharln)
* 功能描述:求和校验（取发送、接收协议的1\2\3\4\5\6\7的和取反+1）
* 函数说明:将数组的元素1-倒数第二个元素相加后取反+1（元素个数必须大于2）
**********************************************************************/
uint8_t ZE08_CheckSum(uint8_t *pbuff,uint8_t nLen)
{
	uint8_t j,tempq=0;
	pbuff+=1;
	for(j=0;j<(nLen-2);j++)
	{
		tempq+=*pbuff;
		pbuff++;
	}
	tempq=(~tempq)+1;
	return(tempq);
}

uint16_t ZE08_GetResult(uint8_t nType)
{
	uint8_t i;
	uint8_t ZE08_Tx[ZE08_FRAME_LENGTH];
	if(ZE08_Mode == ZE08_MODE_ASK)
	{
		ZE08_Tx[0] = 0xFF;
		ZE08_Tx[1] = 0x01;
		ZE08_Tx[2] = 0x86;
		ZE08_Tx[3] = 0x00;
		ZE08_Tx[4] = 0x00;
		ZE08_Tx[5] = 0x00;
		ZE08_Tx[6] = 0x00;
		ZE08_Tx[7] = 0x00;
		ZE08_Tx[8] = 0x79;
		for(i=0;i<ZE08_FRAME_LENGTH;i++)
		{
			USART_SendData(USART3,ZE08_Tx[i]);
			while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);
		}
		Delay_ms(10);
	}
	switch(nType)
	{
		case 1:
			return ZE08_Result_ppb;
		case 2:
			return ZE08_Result_ug;
		case 3:
			return ZE08_MAX;
		default:
			return 0;
	}
}
