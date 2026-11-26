#ifndef __FOOD_H__
#define __FOOD_H__

#include "stm32f4xx.h"
#include "mpu6050.h"
#include "time.h"
#include <math.h>

//====================计步参数配置区====================
#define PEAK_THRESH      0.9f     // 脚步冲击阈值，越大越不灵敏；越小越灵敏
#define MIN_STEP_GAP_MS  300U      // 两步最小间隔ms；350ms最快约170步/分钟
#define USER_STEP_LENGTH 0.70f     // 用户步长，单位：米，成年人普通步长0.65~0.75
#define USER_WEIGHT_KG   60.0f     // 用户体重，单位kg，用于卡路里估算
//======================================================

// MPU6050原始六轴结构体
typedef struct
{
	int16_t ax;
	int16_t ay;
	int16_t az;
	int16_t gx;
	int16_t gy;
	int16_t gz;
}mpu6050_raw_t;

// 运动输出数据：步数、距离、卡路里
typedef struct
{
	uint32_t step_cnt;     // 总步数
	float distance_km;     // 距离，单位km
	float calorie_kcal;   // 消耗卡路里 kcal
}sport_data_t;

void Food_MPU_Sample(void);               // MPU采样+计步处理，建议20ms调用一次
sport_data_t Food_GetSportData(void);     // 获取运动数据副本
mpu6050_raw_t Food_GetMpuRaw(void);       // 获取原始六轴数据
void Food_Step_Clear(void);               // 清零计步所有数据


#endif



