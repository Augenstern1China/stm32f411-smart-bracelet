#include "food.h"

// 保存运动计算结果：步数，距离，卡路里
static sport_data_t g_sport;

// 保存MPU6050读到的原始六轴数据
static mpu6050_raw_t g_mpu_raw;

//========计步算法内部静态变量（外部不能访问）========
static uint32_t step_time_ms = 0;     // 两次脚步之间的计时，单位ms
static float acc_mag_baseline = 9.8f; // 低通滤波得到重力加速度基线，跟踪设备静态重力

// MPU6050读取缓存，存放IIC读到的14字节原始数据(加速度+陀螺仪)
static u8 mpu_buf[14];

/**
 * @brief 读取MPU6050并且执行计步处理 5 vvbv5 
 * @note 主循环必须大约20ms调用一次，固定50Hz采样；调用周期不准计步效果会变差
 * @retval 无
 */
void Food_MPU_Sample(void)
{
	
	// =========【关键】关闭TIM2更新中断，防止打断软件I2C时序 =========
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
	// IIC连续读取MPU6050加速度+陀螺仪共14个字节
	// 返回1代表I2C读取成功；0读取失败，直接跳过本次计算
    if(MPU6050_READ_continuous(MPU6050_ACCEL_XOUT_H, mpu_buf, 14) == 1)
    {
        // 解析加速度高低字节，拼接成16位有符号int16
        g_mpu_raw.ax = ( (int16_t)mpu_buf[0] << 8 ) | mpu_buf[1];
        g_mpu_raw.ay = ( (int16_t)mpu_buf[2] << 8 ) | mpu_buf[3];
        g_mpu_raw.az = ( (int16_t)mpu_buf[4] << 8 ) | mpu_buf[5];

        // 解析陀螺仪高低字节
        g_mpu_raw.gx = ( (int16_t)mpu_buf[8]  << 8 ) | mpu_buf[9];
        g_mpu_raw.gy = ( (int16_t)mpu_buf[10] << 8 ) | mpu_buf[11];
        g_mpu_raw.gz = ( (int16_t)mpu_buf[12] << 8 ) | mpu_buf[13];

        // ======================计步算法开始======================
        // MPU6050 ±2g量程：16384 LSB/g，原始ADC值转为g重力单位
        float acc_x = (float)g_mpu_raw.ax / 16384.0f;
        float acc_y = (float)g_mpu_raw.ay / 16384.0f;
        float acc_z = (float)g_mpu_raw.az / 16384.0f;

        // 计算三轴合加速度幅值 sqrt(ax2+ay2+az2)
        // 设备静止平放时，acc_mag理论≈9.8(g)
        float acc_mag = sqrtf(acc_x*acc_x + acc_y*acc_y + acc_z*acc_z);

        /*
         * 一阶低通滤波：用来跟踪重力基线acc_mag_baseline
         * alpha_lp 滤波系数：数值越小，基线变化越慢，抗抖动越强
         * 0.02代表缓慢跟随姿态变化，走路时不会被脚步冲击干扰基线
         */
        const float alpha_lp = 0.02f;
        acc_mag_baseline = acc_mag_baseline * (1.0f - alpha_lp) + acc_mag * alpha_lp;

        // delta_acc = 当前合加速度 ? 重力基线
        // 走路脚往下落地冲击，delta_acc会出现正向尖峰；抬脚会出现负尖峰
        float delta_acc = acc_mag - acc_mag_baseline;

        /*
         * 脚步判断条件
         * 峰值窗口判断：只识别合适大小的落地冲击
			  * delta_acc > PEAK_THRESH：加速度变化量超过阈值，判定出现脚步冲击
		  	* delta_acc < PEAK_THRESH + 0.6f：限制峰值上限，过滤摔落、大力敲击这类超大噪声
				 * 两个条件同时满足，才认为是一次有效脚步信号
         * step_time_ms >= MIN_STEP_GAP_MS：防短时间重复计数，模拟人走路最小脚步间隔
         */
        if( delta_acc > PEAK_THRESH && delta_acc < PEAK_THRESH + 0.6f )
        {
            if(step_time_ms >= MIN_STEP_GAP_MS)
            {
                g_sport.step_cnt++;        // 步数+1
                step_time_ms = 0;          // 重置步间隔计时器
            }
        }

        step_time_ms += 20;       // 每次采样固定20ms，计时累加

        // ==========估算行走距离、卡路里==========
        // 距离(米) = 总步数 × 单步步长
        float dist_m = g_sport.step_cnt * USER_STEP_LENGTH;
        g_sport.distance_km = dist_m / 1000.0f;   // 米转为公里

        // 经验估算公式：卡路里(kcal)=体重(kg)*距离(km)*0.8
        // 注意：只是经验估算，不是专业设备真实测量
        g_sport.calorie_kcal = USER_WEIGHT_KG * g_sport.distance_km * 0.8f;
    }
		// =========读完I2C，重新打开TIM2更新中断=========
		TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

/**
 * @brief 获取运动数据副本
 * @retval sport_data_t 返回步数、距离、卡路里
 */
sport_data_t Food_GetSportData(void)
{
    return g_sport;
}

/**
 * @brief 获取MPU6050原始六轴数据副本
 * @retval mpu6050_raw_t 六轴原始ADC
 */
mpu6050_raw_t Food_GetMpuRaw(void)
{
    return g_mpu_raw;
}

/**
 * @brief 重置计步所有参数，清零步数、距离、卡路里，重置滤波与计时器
 */
void Food_Step_Clear(void)
{
    g_sport.step_cnt = 0;
    g_sport.distance_km = 0;
    g_sport.calorie_kcal = 0;

    step_time_ms = MIN_STEP_GAP_MS;
    acc_mag_baseline = 9.8f;   // 重置重力基线
}
