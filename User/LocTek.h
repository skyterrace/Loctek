#ifndef __LocTek_H
#define __LocTek_H
#include "stm32f10x.h"

#define UPLOAD_MODE_DIR  0x83  //主动上传模式
#define UPLOAD_MODE_ASK  0x81   //问答上传模式
//CRC-16校验程序
uint16_t CRC16_calc(uint8_t * pBuff, uint16_t nLen);  //*pBuff 为待校验数据首地址，nLen待校验数据长度。返回值为16位校验结果。
#endif
