#ifndef __ZE08_H
#define __ZE08_H
#include "stm32f10x.h"
#define ZE08_FRAME_LENGTH 9 //֡����
#define ZE08_MODE_AUTO 0 //�����ϴ�ģʽ
#define ZE08_MODE_ASK 1 //�ʴ�ģʽ
void ZE08_Init(void);
void ZE08_GetChar(uint8_t getchar);
uint8_t ZE08_CheckSum(uint8_t *pbuff,uint8_t nLen);
//����ZE08����ģʽ��0-�����ϴ���1-�ʴ��ϴ�
void ZE08_SetMode(uint8_t nMode);
uint16_t ZE08_GetResult(uint8_t nType);  //���ؼ��ֵ��nType = 1-ppb��2-ug��3-���̣�����-����0

#endif
