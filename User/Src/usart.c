#include "usart.h"

/*
PA9  USART1_TX
PA10 USART1_RX

*/
void USART1_Init(u32 baud)
{
	float div=0,xiao=0;
	u16 zheng=0;
	
	RCC->AHB1ENR |=(1<<0);//开GPIOA
	RCC->APB2ENR |=(1<<4);//开USART
	
	//配置端口 PA9--TX   
	GPIOA->MODER &=(~(3<<(9*2)));
	GPIOA->MODER |=(2<<(9*2));//写入10配置成复用模式
	GPIOA->OTYPER &=(~(1<<9));//复用推挽输出--//可不配置具体模式
	GPIOA->AFR[1] |=(7<<4);	//开启复用功能
	
	//配置端口    PA10--RX
	GPIOA->MODER &=(~(3<<(10*2)));
	GPIOA->MODER |=(2<<(10*2));//写入10配置成复用模式
	GPIOA->PUPDR &=(~(3<<(10*2)));//浮空输入
	GPIOA->AFR[1] |= (7<<8);//开启复用功能 
	
	div=100000000.0/8/2/baud;
	zheng=(u16)div;
	xiao=(div-zheng);
	USART1->BRR =zheng<<4 | ((int)(xiao*16));
	
	USART1->CR1 &=(~(1<<15));//16倍采样
	USART1->CR1 &=(~(1<<12));//8位
	USART1->CR1 &=(~(1<<10));//禁止奇偶校验
	USART1->CR2 &=(~(3<<12));//停止位
	
	USART1->CR1 |=(1<<3);//使能接收
	USART1->CR1 |=(1<<2);//使能发送
	
	USART1->CR1 |=(1<<13);//开启USART1
}




/*
printf：修改printf的底层代码-fputc---printf的底层函数
重定向
*/
int fputc(int data,FILE*file)
{
	while(!(USART1->SR & 1<<6))
	{
		;//轮询等待数据发送器是否发送数据完成
	}
	USART1->DR = data;
	return data;
}
