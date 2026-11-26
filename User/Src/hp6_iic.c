#include "hp6_iic.h"
static bool hp6_i2c_error = false;
/*
PA1---开漏---SDA
PA2---开漏---SCL
PC13---推挽--低电平开启
*/
void HP6_IIC_GPIO_Init(void)
{
	RCC->AHB1ENR |=(5<<0);
	
	GPIOC->MODER &=(~(3<<26));
	GPIOC->MODER |=(1<<26);//输出模式
	GPIOC->OTYPER &=(~(1<<13));//推挽
	GPIOC ->OSPEEDR |=(2<<26);//PC13
	
	GPIOA->MODER &=(~(15<<2));
	GPIOA->MODER |=(5<<2);
	
	GPIOA->OTYPER |=(3<<1);//开漏输出
	
	GPIOA->OSPEEDR &=(~(15<<2));
	GPIOA->OSPEEDR |=(10<<2);//50mhz
	
	HP6_OFF();
	HP6_SCL_H();
	HP6_SDA_H();
}
/**
 *  等待SCL被从机释放，处理时钟拉伸，带超时，防止死循环
HP6 血压模块：收到命令后内部跑算法、处理大量 ADC 采样，
它就会拉低 SCL，强制 STM32 主机停下来等待。
成功 true 1
失败 false 0
 */
static bool HP6_I2C_WaitSCLRelease(void)
{
    uint32_t timeout = 2000U;
    HP6_SCL_H();
    TIM3_us(5U);
    while( HP6_READ_SCL() == 0U )
    {
        timeout--;
        if(timeout == 0U)
        {
            hp6_i2c_error = true;
            return false;
        }
        TIM3_us(1U);
    }
    return true;
}
bool HP6_I2C_Start(void)
{
	hp6_i2c_error = false;
	HP6_SDA_H();
	HP6_SCL_H();
	TIM3_us(5U);
	if(!HP6_I2C_WaitSCLRelease())
	{
		return false;
	}
	HP6_SDA_L();
	TIM3_us(4U);
	HP6_SCL_L();
	return true;
}
void HP6_I2C_Stop(void)
{
	HP6_SCL_L();
	HP6_SDA_L();
	TIM3_us(5U);
	if(!HP6_I2C_WaitSCLRelease())
	{
		return;
	}
	HP6_SDA_H();
	TIM3_us(5U);
}
/**
 * @brief I2C发送1字节，返回ACK状态：0应答成功，1NACK/失败
 */
uint8_t HP6_I2C_Send_Byte(uint8_t dat)
{
	uint8_t i;
	uint8_t nack = 1U;
	HP6_SCL_L();
	for(i=0;i<8;i++)
	{
		if(dat & 0x80U)
		{
			HP6_SDA_H();
		}
		else
		{
			HP6_SDA_L();
		}
		dat <<= 1U;
		TIM3_us(5U);
		if(!HP6_I2C_WaitSCLRelease())
		{
			return 1U;
		}
		HP6_SCL_L();
	}
	//读取ACK
	HP6_SDA_H();
	TIM3_us(5U);
	if(!HP6_I2C_WaitSCLRelease())
	{
		return 1U;
	}
	nack = HP6_READ_SDA();
	HP6_SCL_L();
	return nack;
}
/**
 * @brief I2C读1字节
 * @param ack 1:主机回复ACK；0：回复NACK（最后一字节）
 * @return 读到的数据
 */
uint8_t HP6_I2C_Read_Byte(uint8_t ack)
{
	uint8_t i;
	uint8_t dat = 0U;
	HP6_SDA_H();
	for(i=0;i<8;i++)
	{
		HP6_SCL_L();
		TIM3_us(5U);
		if(!HP6_I2C_WaitSCLRelease())
		{
			return 0xFFU;
		}
		dat <<=1U;
		if(HP6_READ_SDA())
			dat |= 0x01U;
	}
//发送ACK/NACK：先拉低SCL，再改变SDA（避免总线违规）
	HP6_SCL_L();
	TIM3_us(5U);
	if(ack)
	{
		HP6_SDA_L();
	}
	else
	{
		HP6_SDA_H();
	}
	TIM3_us(5U);
	if(!HP6_I2C_WaitSCLRelease())
	{
		return 0xFFU;
	}
	HP6_SCL_L();
	HP6_SDA_H();
	return dat;
}
/**
 * @brief HP6对外写接口，给HP_6_protocol.c调用
 * @param Hp_6_address 7位I2C地址，HP6=0x66
 * @param pData 数据缓存
 * @param bytes 字节数
 * @retval true成功 false失败
 */
bool Hp_6_I2CWrite(uint8_t Hp_6_address, uint8_t *pData, uint8_t bytes)
{
	uint8_t i;
	if(!HP6_I2C_Start())
		return false;
	//7bit地址左移1位，写方向
	if( HP6_I2C_Send_Byte( Hp_6_address << 1U ) != 0U )
	{
		HP6_I2C_Stop();
		return false;
	}
	for(i=0;i<bytes;i++)
	{
		if( HP6_I2C_Send_Byte(pData[i]) !=0U )
		{
			HP6_I2C_Stop();
			return false;
		}
	}
	HP6_I2C_Stop();
	return true;
}
bool Hp_6_I2CRead(uint8_t Hp_6_address, uint8_t *pData, uint8_t bytes)
{
	uint8_t i;
	if(!HP6_I2C_Start())
		return false;
	//7bit地址左移1位 | 读bit
	if( HP6_I2C_Send_Byte( (Hp_6_address << 1U)|0x01U ) !=0U )
	{
		HP6_I2C_Stop();
		return false;
	}
	for(i=0;i<bytes;i++)
	{
		if(i == bytes-1U)
		{
			//最后一字节发送NACK
			pData[i] = HP6_I2C_Read_Byte(0U);
		}
		else
		{
			pData[i] = HP6_I2C_Read_Byte(1U);
		}
		if(hp6_i2c_error)
		{
			HP6_I2C_Stop();
			return false;
		}
	}
	HP6_I2C_Stop();
	return true;
}
