#ifndef __RTC_H__
#define __RTC_H__

#include "stm32f4xx.h"                  // Device header
#include "time.h"
#include "gpio.h"
#include "string.h"
extern u8 hour,min,sec;
extern u8 year,month,day,week;
extern char RTC_time[50];
extern char RTC_day[50];
extern char RTC_week[50];

u8 DEC_TO_BCD(u8 data);
u8 BCD_TO_DEC(u8 data);
void Set_Time(u8 hour,u8 min,u8 sec);
void Set_Date(u8 year,u8 month,u8 day,u8 week);
void Read_Time(u8 *hour,u8 *min,u8 *sec);
void Read_Date(u8 *year,u8 *month,u8 *day,u8 *week);
u8 RTC_Get_Week(u8 year, u8 month, u8 day);
u8 RTC1_Init(void);
void HuanXing_Init(u16 time);
void Alarm_A_Init(u8 week,u8 hour,u8 min,u8 sec);

#endif


