#ifndef __HP6_IIC_H__
#define __HP6_IIC_H__


#include "stm32f4xx.h"
#include "time.h"
#include <stdint.h>
#include <stdbool.h>

#define HP6_SCL_L()    GPIOA->BSRRH = (1 << 2);   // PA2拉低
#define HP6_SCL_H()    GPIOA->BSRRL = (1 << 2);   // PA2拉高
#define HP6_SDA_L()    GPIOA->BSRRH = (1 << 1);   // PA1拉低
#define HP6_SDA_H()    GPIOA->BSRRL = (1 << 1);   // PA1拉高

#define HP6_OFF()  GPIOC->ODR |=(1<<13);//高电平 关闭
#define HP6_ON()   GPIOC->ODR &=(~(1<<13));//低电平 开启


#define HP6_READ_SDA()  ((GPIOA->IDR & (1<<1)) ? 1 : 0)
#define HP6_READ_SCL()  ((GPIOA->IDR & (1<<2)) ? 1 : 0)

void HP6_IIC_GPIO_Init(void);
bool HP6_I2C_Start(void);
void HP6_I2C_Stop(void);
uint8_t HP6_I2C_Send_Byte(uint8_t dat);
uint8_t HP6_I2C_Read_Byte(uint8_t ack);

//给protocol调用的对外接口
bool Hp_6_I2CWrite(uint8_t Hp_6_address, uint8_t *pData, uint8_t bytes);
bool Hp_6_I2CRead(uint8_t Hp_6_address, uint8_t *pData, uint8_t bytes);



#endif


