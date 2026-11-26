#include "rtc.h"
#include <stdio.h>
u8 hour,min,sec;
u8 year,month,day,week;
/*
14:13:23
Aug 18 2026
hour = (*(time+0)-'0')*10+(*(time+1)-'0')
char num[100] = {"aWESDFHG"};
*/
char *time = __TIME__;
char *date = __DATE__;
char *mon[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
/****
函数名称：RTC初始化函数
函数参数：无
函数返回值：
		0：初始化失败
		1：初始化成功
函数功能说明：步骤见课件
****/
u8 RTC1_Init(void)
{
	u32 temp = 0,i = 0;;

	RCC->APB1ENR |= 1<<28;//开启PWR时钟

	//========备份域复位，清除备份域异常状态========
	RCC->BDCR |= 1<<16;	//BDRST置1，备份域软件复位
	TIM3_ms(1);
	RCC->BDCR &= ~(1<<16);//释放备份域复位

	PWR->CR |= 1<<8;//使能RTC、备份域寄存器写访问(释放复位后再打开DBP！)

	//========使用LSI内部低速RC时钟，替代外部LSE晶振========
	RCC->CSR |= 1<<0;	//打开LSI内部RC
	while(!(RCC->CSR & 1<<1))
	{
		temp++;
		TIM3_ms(1);
		if(temp>= 10000)//10s超时等待LSI就绪
		{
			return 0;//LSI时钟未准备就绪
		}
	}

	RCC->BDCR &= ~(3<<8);	//清除RTCSEL选择位
	RCC->BDCR |= (2<<8);	//RTCSEL=10，选择LSI作为RTC时钟源
	RCC->BDCR |= 1<<15;	//使能RTC模块时钟
	
	//解除RTC的写保护
	RTC->WPR = 0XCA;
	RTC->WPR = 0X53;
	
	RTC->ISR |= 1<<7;//进入初始化模式
	temp = 0;
	while(!(RTC->ISR & 1<<6))
	{
		temp++;
		TIM3_ms(1);
		if(temp>= 10000)//10s
		{
			return 0;//进入初始化状态失败
		}
	}
	//设置RTC的预分频
	//LSI典型约32000Hz  PREDIV_A=127 PREDIV_S=249 →128*250 =32000
	RTC->PRER |= (128-1)<<16;//异步预分频
	RTC->PRER |= (250-1)<<0; //同步预分频
	
	RTC->CR &=(~(1<<6));//选择24小时制
	RTC->CR &=(~(1<<5));//从影子寄存器当中读取数据

	//设置时间和日期，使用编译时刻的时间
	hour = (*(time+0)-'0')*10+(*(time+1)-'0');
	min = (*(time+3)-'0')*10+(*(time+4)-'0');
	sec = (*(time+6)-'0')*10+(*(time+7)-'0');
	Set_Time(hour,min,sec);
	
	if(*(date+4) == ' ')//日期<10
	{
		day = *(date+5)-'0';
	}
	else
	{
		day = (*(date+4)-'0')*10+*(date+5)-'0';
	}
	year = (*(date+9)-'0')*10+*(date+10)-'0';
	for(i = 0;i < 12;i++)
	{
		if(strncmp(mon[i],date,3)== 0)
		{
			break;
		}
	}
	month = i+1;
	//基姆拉尔森需要完整4位年份，year只存后两位，补20xx
	week = RTC_Get_Week(year,month,day);
	Set_Date(year,month,day,week);

	RTC->ISR &=(~(1<<7));//退出初始化模式
	
	RTC->WPR =0x66;//随机写入一个数据激活写保护
	
	RTC->ISR &=(~(1<<5));//同步标志位清0
	temp = 0;
	while(!(RTC->ISR & 1<<5))
	{
		temp++;
		TIM3_ms(1);
		if(temp>= 10000)//10s
		{
			return 0;//RTC影子寄存器同步失败
		}
	}
	RTC->ISR &=(~(1<<5));//同步标志位清0
	return 1;
}


/**
函数功能：十进制转BCD码
函数参数：要转换的十进制数据
函数返回值：转换完成之后的BCD码
23---2   3
0010 0011
23 % 10
23 
&0x0f
**/
u8 DEC_TO_BCD(u8 data)
{
	return ((data/10)<<4)|(data%10);
}

/**
函数功能：BCD码转十进制
函数参数：要转换完成之后的BCD码
函数返回值：转换完成的十进制数据
23---2   3
0010 0011
**/
u8 BCD_TO_DEC(u8 data)
{
	return ((data>>4)*10)+(data&0x0f);
}
/****
函数名称：设置时间函数
函数参数：
		u8 hour：小时
		u8 min：分钟
		u8 sec:秒
函数返回值：无
函数功能说明：
根据时间寄存器的位数写到寄存器
寄存器当中存放的是BCD码
****/
void Set_Time(u8 hour,u8 min,u8 sec)
{
	RTC->TR = DEC_TO_BCD(hour)<<16 |
				DEC_TO_BCD(min)<<8 |
				DEC_TO_BCD(sec)<<0;
}
/****
函数名称：设置日期函数
函数参数：
		u8 year：年
		u8 month：月
		u8 day:日
		u8 week:星期
函数返回值：无
函数功能说明：
根据日期寄存器的位数写到寄存器
寄存器当中存放的是BCD码
星期存放的时候不是BCD码，不需要转换
****/
void Set_Date(u8 year,u8 month,u8 day,u8 week)
{
	RTC->DR = DEC_TO_BCD(year)<<16 |
				DEC_TO_BCD(month)<<8 |
				DEC_TO_BCD(day)<<0|
				week <<13;
}
/****
函数名称：读取时间函数
函数参数：
	输出参数
		u8 *hour：读取到的小时
		u8 *min：读取到的分钟
		u8 *sec:读取到的秒
函数返回值：无
函数功能说明：
0000 0000 1110 0101 1010 1100 0101 1110
0000 0000 0000 0000 0000 0000 0011 0010
****/
void Read_Time(u8 *hour,u8 *min,u8 *sec)
{
	u32 temp = RTC->TR;
	*hour = BCD_TO_DEC((temp>>16)&0x3f);
	*min = BCD_TO_DEC((temp>>8)&0x7f);
	*sec = BCD_TO_DEC((temp>>0)&0x7f);
}
/****
函数名称：读取日期函数
函数参数：
	输出参数
		u8 *year：读取到的年（只有十位和个位）
		u8 *month:读取到的月
		u8 *day:读取到的日
		u8 *week：读取到的星期
函数返回值：无
函数功能说明：
0000 0000 1110 0101 1010 1100 0101 1110
0000 0000 0000 0000 0000 0000 0011 0010
****/
void Read_Date(u8 *year,u8 *month,u8 *day,u8 *week)
{
	u32 temp = RTC->DR;
	*year = BCD_TO_DEC((temp>>16)&0xff);
	*month = BCD_TO_DEC((temp>>8)&0x1f);
	*day = BCD_TO_DEC((temp>>0)&0x3f);
	*week = (temp>>13)&0x07;
}

/**********************************************************************
函数功能：RTC获取星期值函数
参数：year,month,date:年,月,日
返回值：星期几（1～7）
使用基姆拉尔森计算公式：W=(d+1+2*m+3*(m+1)/5+y+y/4-y/100+y/400)   1.5 
Week的取值范围是0~6，其中0代表星期日，1~6分别代表星期一到星期六。
注意在运算时要把1月和2月看为是上一年的13月和14月代入计算
**********************************************************************/
u8 RTC_Get_Week(u8 year, u8 month, u8 day)
{
	int WeekDay = -1;
	
	if (1 == month || 2 == month) 
    {   
        month += 12; 
        year--;
    }
	
	WeekDay = (day + 1 + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
	if(WeekDay == 0)//week的取值范围是0~6，0代表周日
	{
		WeekDay += 7;
	}
	
	return WeekDay;
}
/****
函数功能:周期性唤醒函数
函数参数：唤醒的时间（s）
返回值：无
****/
void HuanXing_Init(u16 time)
{
	u32 pic = 0;
	RCC->APB1ENR |= 1<<28;//开启PWR时钟
	PWR->CR |= 1<<8;//使能RTC的写访问
	
	//解除RTC的写保护
	RTC->WPR = 0XCA;
	RTC->WPR = 0X53;
	
	RTC->CR &=(~(1<<10));//禁止唤醒定时器
	while(!(RTC->ISR & 1<<2))
	{
		;
	}
	RTC->CR |= 4<<0;//选择RTC日历设置的路径--频率1hz
	RTC->WUTR = 0;//将唤醒定时器寄存器清0
	RTC->WUTR = time-1;//设置自动唤醒时间
	
	RTC->CR |= 1<<14;//使能唤醒中断
	
	RTC->ISR &=(~(1<<10));
	RTC->CR |= 1<<10;//使能唤醒定时器
	RTC->WPR = 0x88;//激活写保护
	
	EXTI->RTSR |= 1<<22;//开启上升沿触发
	
	pic = NVIC_EncodePriority (0x05,1,1);//设置分组、抢占优先级的等级、响应优先级的等级
	NVIC_SetPriority(RTC_WKUP_IRQn,pic);//设置具体的中断源
	NVIC_EnableIRQ (RTC_WKUP_IRQn);//使能中断源
	
	EXTI->PR |= 1<<22;//清标志位
	EXTI->IMR |= 1<<22;//使能中断
}


char RTC_time[50];
char RTC_day[50];
char RTC_week[50];

//中断服务函数
void RTC_WKUP_IRQHandler(void)
{
	if(EXTI->PR & 1<<22)
	{
		EXTI->PR |= 1<<22;
		//添加一步判断是否发送了周期性唤醒的标志
		if(RTC->ISR & 1<<10)
		{
			RTC->ISR &=(~(1<<10));
			Read_Time(&hour,&min,&sec);
			Read_Date(&year,&month,&day,&week);
			printf("%02d:%02d:%02d\r\n",hour,min,sec);
			sprintf(RTC_time,"%02d:%02d:%02d\r\n",hour,min,sec);
			printf("20%02d-%02d-%02d\r\n",year,month,day);
			sprintf(RTC_day,"20%02d-%02d-%02d\r\n",year,month,day);
			printf("week:%d\r\n",week);
			sprintf(RTC_week,"week:%d\r\n",week);
		}
		
	}
}

/****
函数功能:闹钟函数
函数参数：
		u8 week:星期
		u8 hour:小时
		u8 min:分钟
		u8 sec:秒钟
返回值：无
****/
void Alarm_A_Init(u8 week,u8 hour,u8 min,u8 sec)
{
	u32 pic = 0;
	RCC->APB1ENR |= 1<<28;//使能电源接口使用（要接触RTC的写访问，需要用到PWR-CR寄存器）
	PWR->CR |= 1<<8;//使能对 RTC、RTC 备份寄存器和备份 SRAM 的访问
	
	//关闭寄存器的写保护
	RTC->WPR = 0XCA;
	RTC->WPR = 0X53;
	
	RTC->CR &=(~(1<<8));//禁止闹钟A工作
	
	while(!(RTC->ISR & 1<<0))//轮询等待闹钟A可以编写
	{
		;
	}
	RTC->ALRMAR = 0;//闹钟寄存器清零
	//编写闹钟A的寄存器
	RTC->ALRMAR |= 1<<30;//选择DU[3:0]代表的是星期
	RTC->ALRMAR &=(~(1ul<<31));
	//代表和星期匹配，小时分钟秒复位值均为1，配置也为1，所以没有写
	RTC->ALRMAR |= (DEC_TO_BCD(week)<<24) |
				(DEC_TO_BCD(hour)<<16) |
				(DEC_TO_BCD(min)<<8) |
				(DEC_TO_BCD(sec)<<0);
	
	RTC->CR |= 1<<12;//开启闹钟A的中断
	RTC->ISR &=(~(1<<8));//清除闹钟A的标志位
	RTC->CR |=(1<<8);//使能闹钟A
	RTC->WPR = 0x88;//激活写保护
	
	EXTI->RTSR |= 1<<17;//开启上升沿触发
	
	pic = NVIC_EncodePriority (0x05,0,0);//设置分组、抢占优先级的等级、响应优先级的等级
	NVIC_SetPriority(RTC_Alarm_IRQn,pic);//设置具体的中断源
	NVIC_EnableIRQ (RTC_Alarm_IRQn);//使能中断源
	
	EXTI->PR |= 1<<17;//清标志位
	EXTI->IMR |= 1<<17;//使能中断
}

//中断服务函数
void RTC_Alarm_IRQHandler(void)
{
	if(EXTI->PR & 1<<17)
	{
		EXTI->PR |= 1<<17;
		//添加一步判断是否发送了闹钟中断
		if(RTC->ISR & 1<<8)
		{
			RTC->ISR &=(~(1<<10));//清周期性唤醒的标志位
			RTC->ISR &=(~(1<<8));
		}
		
	}
}






