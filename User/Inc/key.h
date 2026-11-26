#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f4xx.h"
#include "adc.h"
#include "time.h"
#include "usart.h"

void KEY_Init(void);
u8 KEY_scan(void);
u8 ADC_scan(void);
u8 GET_KEY_fx(void);


extern u8 key_state;
extern u8 key_cnt;

#endif

