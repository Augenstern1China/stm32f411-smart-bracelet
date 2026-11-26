#include "mpu6050.h"
/**
 * @brief MPU6050单字节写寄存器
 * @param reg 寄存器地址
 * @param data 要写入的数据
 * @retval 1成功  0失败
 */
u8 MPU6050_WRITE_danci(u8 reg, u8 data)
{
	IIC_Start();
	IIC_Send_Data(MPU6050_ADDR_WR);
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}

	IIC_Send_Data(reg);
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}

	IIC_Send_Data(data);
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}

	IIC_Stop();
	return 1;
}

/**
 * @brief MPU6050单字节读寄存器
 * @param reg 寄存器地址
 * @param pdata 读出数据存放指针
 * @retval 1成功  0失败
 */
u8 MPU6050_READ_danci(u8 reg, u8 *pdata)
{
	IIC_Start();
	IIC_Send_Data(MPU6050_ADDR_WR);
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}

	IIC_Send_Data(reg);
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}

	IIC_Start();          //重复起始
	IIC_Send_Data(MPU6050_ADDR_RD);
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}

	*pdata = IIC_Read_Data(0); //最后一字节NACK
	IIC_Stop();
	return 1;
}

/**
 * @brief MPU6050连续读寄存器，寄存器地址自动自增
 * @param reg 起始寄存器
 * @param pbuf 接收缓存
 * @param len 需要读取多少字节
 * @retval 1成功，0失败
 */
u8 MPU6050_READ_continuous(u8 reg, u8 *pbuf, u8 len)
{
	u8 i;
	IIC_Start();
	IIC_Send_Data(MPU6050_ADDR_WR);
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}

	IIC_Send_Data(reg);
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}

	IIC_Start();
	IIC_Send_Data(MPU6050_ADDR_RD);
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}

	//前面 len-1 个字节回复ACK，最后1字节NACK
	for(i = 0; i < len; i++)
	{
		if(i == len - 1)
		{
			pbuf[i] = IIC_Read_Data(0);
		}
		else
		{
			pbuf[i] = IIC_Read_Data(1);
		}
	}

	IIC_Stop();
	return 1;
}

/**
 * @brief MPU6050初始化，唤醒传感器
 * @retval 1成功，0通信失败
 */
u8 MPU6050_Init(void)
{
	// PWR_MGMT_1 写0x00退出休眠
	if(MPU6050_WRITE_danci(MPU6050_PWR_MGMT_1, 0x00) == 0)
	{
		return 0;
	}
	return 1;
}









