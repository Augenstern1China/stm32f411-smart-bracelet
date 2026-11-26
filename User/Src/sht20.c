#include "sht20.h"

/**
SHT20_ip =0x80;
全部为非主机模式
GPIO 模拟 I2C 不推荐用保持主机模式，时钟拉伸对模拟 IIC 很不友好，容易失败；
硬件 I2C 外设才适合 Hold?master
**/
float Get_SHT20_temp(void)//获取温度
{
	u8 msb, lsb;
	u16 st;
	float temp;
	
	IIC_Start();
	IIC_Send_Data(0x80);//写操作
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}
	IIC_Send_Data(0xF3);//触发T测量
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}
	
	IIC_Stop();          //非保持模式发送命令后必须停止总线
  TIM3_ms(70);         //温度测量最大时间85ms
	
	IIC_Start();
	IIC_Send_Data(0x81);//读操作
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}

	msb = IIC_Read_Data(1);    //读高字节 ACK
	lsb = IIC_Read_Data(0);    //读低字节 NACK，不读CRC
	IIC_Stop();

	st = (msb << 8) | (lsb & 0xFC); //低2位状态位清零！！
	temp = -46.85f + 175.72f * ((float)st / 65536.0f);
	return temp;
}


float GET_SHT20_shidu(void)//获取湿度
{
	u8 msb, lsb;
	u16 st;
	float shidu;
	
	IIC_Start();
	IIC_Send_Data(0x80);//写操作
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}
	IIC_Send_Data(0xF5);//触发RT测量
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}
	
	IIC_Stop();          //非保持模式发送命令后必须停止总线
  TIM3_ms(29);         //温度测量最大时间29ms
	
	IIC_Start();
	IIC_Send_Data(0x81);//读操作
	if(IIC_Get_ACK())
	{
		IIC_Stop();
		return 0;
	}

	msb = IIC_Read_Data(1);
	lsb = IIC_Read_Data(0);
	IIC_Stop();

	st = (msb << 8) | (lsb & 0xFc); //低4位状态清零
	shidu = -6.0f + 125.0f * ((float)st / 65536.0f);
	return shidu;
}
	

u8 SHT20_SoftReset(void)//软复位
{
    IIC_Start();
    IIC_Send_Data(0x80);
    if(IIC_Get_ACK())
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Data(0xfe);
    if(IIC_Get_ACK())
    {
        IIC_Stop();
        return 1;
    }
    IIC_Stop();
    TIM3_ms(15);   //复位最多15ms
    return 0;
}
	








