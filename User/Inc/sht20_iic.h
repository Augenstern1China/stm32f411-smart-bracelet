#ifndef __IIC_H__
#define __IIC_H__

#include "stm32f4xx.h"
#include "time.h"

//typedef unsigned char u8;
#define SCL_L()    GPIOB->BSRRH = (1 << 8);   // PB8拉低
#define SCL_H()    GPIOB->BSRRL = (1 << 8);   // PB8拉高
#define SDA_L()    GPIOB->BSRRH = (1 << 9);   // PB9拉低
#define SDA_H()    GPIOB->BSRRL = (1 << 9);   // PB9拉高

#define READ_SDA()  ((GPIOB->IDR & (1<<9)) ? 1 : 0)


void IIC_GPIO_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Send_ACK(void);
void IIC_Send_NoACK(void);
void IIC_Send_Data(u8 dat);
u8 IIC_Get_ACK(void);
u8 IIC_Read_Data(u8 ack);


#endif


