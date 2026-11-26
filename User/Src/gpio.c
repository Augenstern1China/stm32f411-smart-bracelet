#include "gpio.h"


void LED_Init(void)//PA7 低电平亮
{
	RCC->AHB1ENR |=(1<<0);
	
	GPIOA->MODER &=(~(3<<14));
	GPIOA->MODER |=(1<<14);//输出模式
	GPIOA->OTYPER &=(~(1<<7));//推挽
	GPIOA ->OSPEEDR |=(2<<14);
	
	GPIOA->ODR |=(1<<7);//高电平
}



