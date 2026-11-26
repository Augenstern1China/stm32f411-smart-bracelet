#ifndef __TIME_H__
#define __TIME_H__

#include "stm32f4xx.h"
#include "gpio.h"


void TIM2_Init(u32 psc,u32 apr);
void TIM3_Init(u32 psc,u32 apr);
void TIM3_ms(u32 ms);
void TIM3_us(u32 us);

extern volatile u8 step_sample_flag;

#endif

