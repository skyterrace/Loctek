#ifndef __LocTek_H
#define __LocTek_H
#include "stm32f10x.h"

#define UPLOAD_MODE_DIR  0x83  //�����ϴ�ģʽ
#define UPLOAD_MODE_ASK  0x81   //�ʴ��ϴ�ģʽ
//CRC-16У�����
uint16_t CRC16_calc(uint8_t * pBuff, uint16_t nLen);  //*pBuff Ϊ��У�������׵�ַ��nLen��У�����ݳ��ȡ�����ֵΪ16λУ������
#endif
