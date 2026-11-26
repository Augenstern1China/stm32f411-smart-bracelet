#include "key.h"

/*
PA0---接收是否按下---浮空输入
PA3---接收电平---模拟输入
*/

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	ADC_PA3_Init();
}

u8 KEY_scan(void)//按键消抖
{
	
	static u8 flag=0;
	if((flag==0)&&(GPIOA->IDR & (1<<0)))
	{
		TIM3_ms(20);
		flag=1;
		if(GPIOA->IDR & (1<<0))//证明是PA0按键按下
		{
			return 5;
		}
		
	}
	else if(!(GPIOA->IDR & (1<<0)))
	{
		flag=0;
	}
	return 0;
}


u8 key_state = 0;//进入标志位  检测空闲状态
u8 key_cnt = 0;//进入次数

u8 ADC_scan(void)//消抖
{
    u8 ret = 0;
    u8 curr_key = GET_ADC();   //全部按键来自ADC

    if(curr_key != 0)
    {
        if(key_state == 0)
        {
            key_cnt++;
            if(key_cnt >= 2)//连续2次采样一致，确认按键按下
            {
                key_state = 1;
                ret = curr_key;   //只输出一次按键事件
            }
        }
    }
    else
    {
        //检测到松手，全部清零
        key_cnt = 0;
        key_state = 0;
    }
    return ret;
}

u8 GET_KEY_fx(void)
{
	u16 val=0;
	u8 fx=0;
	val=ADC_scan();

	if(KEY_scan()==5)
	{
		val=5;
	}
	
	switch(val)
	{
		case 1:fx=1;break;
		case 2:fx=2;break;
		case 3:fx=3;break;
		case 4:fx=4;break;
		case 5:fx=5;break;
		default :
			break;
	}
	return fx;
}





