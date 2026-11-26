#include "stm32f4xx.h"
#include <stm32f4xx_conf.h>
#include "stdio.h"
#include "gpio.h"
#include "time.h"
#include "io_bit.h"
#include "usart.h"
#include "spi.h"
#include "start.h"
#include "sht20.h"
#include "hp6.h"
#include "mpu6050.h"
#include "food.h"
#include "rtc.h"
 
int main()
{
	char buff[50];
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //F4必须配置优先级分组
	
	LED_Init();
//	TIM2_Init(100,2000);
	USART1_Init(115200);
	KEY_Init();
	SPI_1_Init();
	OLED_Init();
	IIC_GPIO_Init();
	SHT20_SoftReset();
	HP6_Init();
	MPU6050_Init();
	if(RTC1_Init() == 0)
	{
		printf("RTC初始化失败！\r\n");
	}
	printf("hello");
	HuanXing_Init(1);
	while(1)
	{
	
		
		caidan();
		u8 fx=GET_KEY_fx();
		
		//====非阻塞：间隔大约20ms执行一次计步采样====
            static uint32_t cnt_ms = 0;
            cnt_ms++;
            if(cnt_ms >=20)
            {
                cnt_ms = 0;
                Food_MPU_Sample();
            }
//		Food_MPU_Sample();//从ACCEL_XOUT_H连续读取14字节 加速度 温度 陀螺仪
		if((fx==5)&&(RIGHT_LEFT==1))
		{
			uint8_t ret;
			int i=1;
			OLED_Clear(0,0,127,7);
			OLED_DrawBMP(0,0,127,7, xinwz);
			TIM3_ms(5000);
			OLED_Clear(0,0,127,7);
			
			printf("\r\n--- 开始测量心率 ---\r\n");
				if(HP6_Hr_Start() != 0)             // 打开心率模块，失败返回1
				{
					printf("启动失败，请检查硬件连接\r\n");
					TIM3_ms(5000);
					continue;
				}
			while(fx!=2)
			{
				fx=GET_KEY_fx();
				while(i)
				{
					TIM3_ms(1000);
					ret = HP6_Hr_ReadResult(&hp6_res);
					if(ret == 0 && hp6_res.hr_rate != 0)   // 拿到有效心率
					{
						OLED_Clear(0,0,127,7);
						printf("心率 = %d bpm\r\n", hp6_res.hr_rate);
						sprintf(buff,"心率:%d bpm\r\n", hp6_res.hr_rate);
						OLED_Dis_String(25,2,buff);
						HP6_Hr_Stop();                      // 关闭心率模块
						i=0;
						printf("测量结束\r\n");
						break;
					}
					else                                   // ret==2 或 心率还是0
					{
						printf("测量中...\r\n");
						OLED_Dis_String(25,2,"测量中...");
					}
				}
			}
			OLED_Clear(0,0,127,7);
		}
		else if((fx==5)&&(RIGHT_LEFT==2))
		{
			uint8_t ret;
			int i=1;
			OLED_Clear(0,0,127,7);
			OLED_DrawBMP(0,0,127,7, xuewz);
			TIM3_ms(5000);
			OLED_Clear(0,0,127,7);
			
			printf("\r\n--- 开始测量血压 ---\r\n");
				if(HP6_Bp_Start() != 0)             // 打开血压模块，失败返回1
				{
					printf("启动失败，请检查硬件连接\r\n");
					TIM3_ms(5000);
					continue;
				}
			while(fx!=2)
			{
				fx=GET_KEY_fx();
				while(i)
				{
					TIM3_ms(1000);                  // 每秒读一次结果
					ret = HP6_Bp_ReadResult(&hp6_res);
					if(ret == 3)  // 测量中
					{						
						printf("测量中...\r\n");
						OLED_Dis_String(25,2,"测量中...");
					}
					else if(ret == 5)               // 测量失败
					{
						OLED_Clear(0,0,127,7);
						printf("测量失败\r\n");
						OLED_Dis_String(25,5,"测量失败");
						i=0;
						HP6_Bp_Stop();                      // 关闭模块（协议关闭 + 硬件断电）
						printf("测量结束\r\n");
						break;
					}
					else if(ret == 4)               // 测量完成
					{
						OLED_Clear(0,0,127,7);
						printf("测量完成：收缩压=%d mmHg，舒张压=%d mmHg，心率=%d bpm\r\n",
								hp6_res.sys, hp6_res.dia, hp6_res.hr_bp);
						sprintf(buff,"收缩压:%dmmHg",hp6_res.sys);
						OLED_Dis_String(15,1,buff);
						sprintf(buff,"舒张压:%dmmHg",hp6_res.dia);
						OLED_Dis_String(15,3,buff);
						sprintf(buff,"心率:%dbpm",hp6_res.hr_bp);
						OLED_Dis_String(15,5,buff);
						HP6_Bp_Stop();   						// 关闭模块（协议关闭 + 硬件断电）
						printf("测量结束\r\n");
						i=0;
						break;
					}
				}
			}
			OLED_Clear(0,0,127,7);
		}
		else if((fx==5)&&(RIGHT_LEFT==3))
		{
			OLED_Clear(0,0,127,7);
			float temp, humi;
			while(fx!=2)
			{
				fx=GET_KEY_fx();
				TIM3_ms(500);
				temp = Get_SHT20_temp();
				humi = GET_SHT20_shidu();
				printf("T:%.2f℃ RH:%.2f %%RH\r\n", temp, humi);
				sprintf(buff,"温度:%.2f℃",temp);
				OLED_Dis_String(15,1,buff);
				sprintf(buff,"湿度:%.2f%%",humi);
				OLED_Dis_String(15,3,buff);
			}
			OLED_Clear(0,0,127,7);
		}
		else if((fx==5)&&(RIGHT_LEFT==4))
		{
			OLED_Clear(0,0,127,7);
			while(fx!=2)
			{
				fx=GET_KEY_fx();
				
				Food_MPU_Sample();
				sport_data_t sport = Food_GetSportData();
	    	printf("step:%d dist:%.2fkm cal:%.1fkcal\r\n",
				sport.step_cnt,sport.distance_km,sport.calorie_kcal);
				sprintf(buff,"step:%d",sport.step_cnt);
				OLED_Dis_String(15,1,buff);
				sprintf(buff,"dist:%.2fkm",sport.distance_km);
				OLED_Dis_String(15,3,buff);
				sprintf(buff,"cal:%.1fkcal",sport.calorie_kcal);
				OLED_Dis_String(15,5,buff);
			}
			OLED_Clear(0,0,127,7);
		}
		
		
	}
}











