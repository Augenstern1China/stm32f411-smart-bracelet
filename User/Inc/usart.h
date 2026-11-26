#ifndef __USART_H__
#define __USART_H__

#include "stm32f4xx.h"
#include "stdio.h"


void USART1_Init(u32 baud);



int fputc(int data,FILE*file);

#endif


