#ifndef __ZE08_H
#define __ZE08_H
#include "stm32f10x.h"
#define ZE08_FRAME_LENGTH 9 //帧长度
#define ZE08_MODE_AUTO 0 //主动上传模式
#define ZE08_MODE_ASK 1 //问答模式
void ZE08_Init(void);
void ZE08_GetChar(uint8_t getchar);
uint8_t ZE08_CheckSum(uint8_t *pbuff,uint8_t nLen);
//设置ZE08传输模式，0-主动上传，1-问答上传
void ZE08_SetMode(uint8_t nMode);
uint16_t ZE08_GetResult(uint8_t nType);  //返回检测值，nType = 1-ppb；2-ug；3-量程；其他-返回0

#endif
