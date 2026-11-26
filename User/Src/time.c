#include "time.h"

//100mhz

/*
定时器2的中断
*/
void TIM2_Init(u32 psc,u32 apr)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct={0};
	NVIC_InitTypeDef NVIC_InitStruct={0};

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);//开启时钟
	
	TIM_TimeBaseInitStruct.TIM_Period=apr-1;//自动重装载值
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc-1;//设置预分频值
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//设置向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);//配置时钟基数
	
	TIM_SetCounter(TIM2, 0);//设置计数器的初始值
	
//	TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Repetitive);//设置为循环模式
	
	TIM_ARRPreloadConfig(TIM2,ENABLE); //打开ARR这一组影子缓冲
	TIM_UpdateDisableConfig(TIM2,DISABLE);//使能更新事件--默认打开
	TIM_GenerateEvent(TIM2, TIM_EventSource_Update); //软件产生更新事件UEV
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//开启中断
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清除标志位
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;  //抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;         //子优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);//设置中断源
	
	TIM_Cmd(TIM2, DISABLE);//失能定时器
}

/*
中断服务函数
*/
volatile u8 step_sample_flag;
void TIM2_IRQHandler(void)
{
	//判断是不是更新溢出中断
	if(TIM_GetITStatus(TIM2, TIM_IT_Update)==1)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //清除中断挂起位，必须写
//		LED1=!LED1;
		step_sample_flag = 1;
	}
}

/*
定时器3的ms us
*/
void TIM3_Init(u32 psc,u32 apr)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct={0};

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//开启时钟
	
	TIM_TimeBaseInitStruct.TIM_Period=apr-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc-1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);//配置时钟基数
	TIM_SetCounter(TIM3, 0);//设置计数器的初始值
	
//	TIM_ARRPreloadConfig(TIM3,ENABLE); //打开ARR这一组影子缓冲
//	TIM_UpdateDisableConfig(TIM3,DISABLE);//使能更新事件--默认打开
//	TIM_GenerateEvent(TIM3, TIM_EventSource_Update); //软件产生更新事件UEV
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除标志位
	
	TIM_Cmd(TIM3, ENABLE);//使能定时器
}

void TIM3_ms(u32 ms)
{
	TIM3_Init(100,1000);
	while(ms--)
	{
		while(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == 0);
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	}
	TIM_Cmd(TIM3, DISABLE);//关闭定时器3
}


void TIM3_us(u32 us)
{
	TIM3_Init(1,100);
	while(us--)
	{
		while(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) == 0);
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	}
	TIM_Cmd(TIM3, DISABLE);//关闭定时器3
}
