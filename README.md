# STM32F411 智能手环

基于 STM32F411 的智能手环固件工程，使用 Keil MDK 开发。

## 功能

- OLED 屏幕显示
- MPU6050 运动数据采集
- SHT20 温湿度采集
- RTC 实时时钟
- 按键输入
- SPI、I2C 与 USART 外设通信
- 计步、距离与卡路里计算
- 心率、血氧等数据通信处理

## 硬件平台

- MCU：STM32F411CEUx
- 显示屏：OLED
- 传感器：MPU6050、SHT20
- 开发环境：Keil MDK 5
- 编译器：ARMCC 5

## 使用方法

1. 使用 Keil MDK 5 打开 `STM32_Project.uvprojx`。
2. 根据实际硬件连接检查引脚和外设配置。
3. 编译工程。
4. 将生成的固件烧录到 STM32F411 开发板。

## 目录结构

- `User/`：应用层代码与头文件
- `Libraries/`：STM32 标准外设库
- `Cmsisi/`：CMSIS 与启动文件
- `STM32_Project.uvprojx`：Keil 工程文件

## 说明

本项目为嵌入式学习与功能验证工程。使用前请根据实际硬件修改引脚配置、传感器地址和显示参数。
