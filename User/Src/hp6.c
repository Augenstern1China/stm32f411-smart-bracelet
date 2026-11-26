#include "hp6.h"

hp6_result_t hp6_res;

/*
#define SCL_L()     // PA2拉低
#define SCL_H()     // PA2拉高
#define SDA_L()    // PA1拉低
#define SDA_H()      // PA1拉高

#define HP6_OFF() //高电平 关闭
#define HP6_ON()   //低电平 开启
*/
void HP6_Init(void)
{
	HP6_IIC_GPIO_Init();
}

// 内部私有：电源操作，外部不可调用
static void HP6_PowerOn(void)
{
	HP6_ON();
	TIM3_ms(80);	//上电等待模块启动，不可删除
}

static void HP6_PowerOff(void)
{
	HP6_OFF();
	TIM3_ms(20);
}

/**
 * @brief 开启血压测量
 * @retval 0成功，1通信失败
 */
uint8_t HP6_Bp_Start(void)
{
	HP6_PowerOn();			//先硬件上电、等待启动
	uint8_t ret = HP_6_OpenBp();
	if(ret != HP_6_SUCCESS)
	{
		HP6_PowerOff();		//开启命令失败，立刻断电
		return 1;
	}
	return 0;
}

/**
 * @brief 关闭血压测量
 * @retval 0成功，1通信失败
 */
uint8_t HP6_Bp_Stop(void)
{
	uint8_t ret = HP_6_CloseBp();
	HP6_PowerOff();			//无论协议成功失败，都硬件断电
	if(ret != HP_6_SUCCESS)
	{
		return 1;
	}
	return 0;
}

/**
 * @brief 读取血压结果
 * @param res 输出结果结构体
 * @retval 1通信CRC错误；2未开启测量；3测量中；4测量完成；5测量失败
 */
uint8_t HP6_Bp_ReadResult(hp6_result_t *res)
{
	uint8_t rx_buf[24] = {0};
	uint8_t ret;

	ret = HP_6_GetBpResult();
	if(ret != HP_6_SUCCESS)
	{
		return 1;
	}
	HP_6_GetResultData(rx_buf);

	if(rx_buf[6] == 0)
	{
		return 2; //血压未开启
	}

	res->bp_state = rx_buf[7];
	res->sys = rx_buf[10];
	res->dia = rx_buf[11];
	res->hr_bp = rx_buf[12];

	if(res->bp_state == 0)
	{
		return 3;
	}
	else if(res->bp_state == 1)
	{
		return 4;
	}
	else if(res->bp_state == 2)
	{
		return 5;
	}
	return 0;
}

/**
 * @brief 开启心率测量
 * @retval 0成功，1失败
 */
uint8_t HP6_Hr_Start(void)
{
	HP6_PowerOn();
	uint8_t ret = HP_6_OpenRate();
	if(ret != HP_6_SUCCESS)
	{
		HP6_PowerOff();
		return 1;
	}
	return 0;
}

/**
 * @brief 关闭心率测量
 * @retval 0成功，1失败
 */
uint8_t HP6_Hr_Stop(void)
{
	uint8_t ret = HP_6_CloseRate();
	HP6_PowerOff();
	if(ret != HP_6_SUCCESS)
	{
		return 1;
	}
	return 0;
}

/**
 * @brief 读取心率通道结果
 * @param res 输出
 * @retval 1通信失败；2未开启；0正常读到数据
 */
uint8_t HP6_Hr_ReadResult(hp6_result_t *res)
{
	uint8_t rx_buf[24] = {0};
	uint8_t ret;

	ret = HP_6_GetRateResult();
	if(ret != HP_6_SUCCESS)
	{
		return 1;
	}
	HP_6_GetResultData(rx_buf);


	if(rx_buf[6] == 0)
	{
		return 2;
	}
	res->hr_rate = rx_buf[7];
	return 0;
}


