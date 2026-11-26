#ifndef __MPU6050_H__
#define __MPU6050_H__


#include "stm32f4xx.h"
#include "sht20_iic.h"


// MPU6050 I2C地址  AD0接GND
#define MPU6050_ADDR_WR     0xD0
#define MPU6050_ADDR_RD     0xD1

// 寄存器定义
#define MPU6050_PWR_MGMT_1    0x6B
#define MPU6050_WHO_AM_I      0x75
#define MPU6050_ACCEL_XOUT_H  0x3B  //6轴数据起始地址

//函数声明
u8 MPU6050_WRITE_danci(u8 reg, u8 data);
u8 MPU6050_READ_danci(u8 reg, u8 *pdata);
/**
 * @brief 连续读多个寄存器
 * @param reg 起始寄存器地址
 * @param pbuf 接收数据缓冲区
 * @param len 读取字节个数
 * @retval 1成功，0失败
 */
u8 MPU6050_READ_continuous(u8 reg, u8 *pbuf, u8 len);
u8 MPU6050_Init(void);

#endif


