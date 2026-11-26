#include "spi.h"

//使用软件spi
void SPI_1_Init(void)
{
	//定义一个结构体变量
	SPI_InitTypeDef SPI_InitStruct = {0};
	//开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);
	
	//配置SPI
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//配置全双工模式
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//主机模式
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//配置为8位数据模式
/*		//模式3
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;//时钟极性
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;//偶数边沿采集
	*/
	//模式0
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;//时钟极性
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;//奇数边沿采集

	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//软件管理片选
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//100/分频<= 18Mhz
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//先发高位
	SPI_Init(SPI1, &SPI_InitStruct);
	
	//使能SPI模块
	SPI_Cmd(SPI1,ENABLE);
	
}

void SPI1_Write_Byte(u8 data)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);  // 等发送完成
}


