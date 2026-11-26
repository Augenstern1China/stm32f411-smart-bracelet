#include "adc.h"

/*
PA3---接收电平---模拟输入
*/

void ADC_PA3_Init(void)
{
	RCC->AHB1ENR |= (1 << 0);      // GPIOA时钟使能
	RCC->APB2ENR |= (1 << 8);     // ADC1时钟使能
	
	GPIOA->MODER |=  (3<< (3*2));
  GPIOA->PUPDR &= ~(3<< (3*2));
	
	ADC->CCR &= ~(3 << 16);
  ADC->CCR |=  (3 << 16);//8分频
	
	ADC1->CR1 &= ~(3 << 24);  // RES=00 12bit
  ADC1->CR1 &= ~(1 << 8);   // SCAN=0 禁止扫描模式
	
	ADC1->CR2 |=  (1 << 1);    // CONT=1 连续转换模式 1--1--1--1
  ADC1->CR2 &= ~(3 << 28);   // EXTEN=00 关闭外部触发，只用软件触发
  ADC1->CR2 &= ~(1 << 10);  // EOCS=0：转换结束后EOC只在序列末尾置位
	
	ADC1->SMPR2 |=  (7U << 9);  // CH3采样时间480cycles 111 
	
	ADC1->SQR1 &= ~(15 << 20);// 规则通道序列：1个转换
	
	ADC1->SQR3 = 3;// SQR3：第1个转换通道 = 通道3
	 
	// 开启ADC模块 ADON=1
	ADC1->CR2 |= (1 << 0);

//	TIM3_ms(200);// 等待ADC模拟部分上电稳定


	// 软件启动转换 SWSTART
	ADC1->CR2 |= (1 << 30);

}


u16 GET_ADC(void)
{
	while(!(ADC1->SR & (1<< 1)));
	
	u16 val=ADC1->DR;
	
		 if(val < 200)
    {
        return 0;       // 没按
    }
    else if(val > 800 && val < 1200)
    {
        return 1;       // UP
    }
    else if(val > 1150 && val < 1600)
    {
        return 2;       // DOWN
    }
    else if(val > 1800 && val < 2300)
    {
        return 4;       // RIGHT 右
    }
    else if(val > 3500)
    {
        return 3;       // LEFT 左
    }
    else
    {
        return 0;       // 无效
    }
	
}













