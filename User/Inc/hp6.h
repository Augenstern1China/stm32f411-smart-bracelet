#ifndef __HP6_H__
#define __HP6_H__

#include "stm32f4xx.h"
#include "time.h"
#include <stdint.h>
#include <stdbool.h>
#include "HP_6_crc16.h"
#include "HP_6_protocol.h"
#include "hp6_iic.h"


//测量结果结构体
typedef struct
{
	uint8_t bp_state;	//血压状态：0测量中，1测量完成，2测量失败
	uint8_t sys;		//收缩压(高压)
	uint8_t dia;		//舒张压(低压)
	uint8_t hr_bp;		//血压通道附带心率
	uint8_t hr_rate;	//独立心率通道心率
}hp6_result_t;

extern hp6_result_t hp6_res;


void HP6_Init(void);

//血压对外接口
uint8_t HP6_Bp_Start(void);
uint8_t HP6_Bp_Stop(void);
uint8_t HP6_Bp_ReadResult(hp6_result_t *res);

//心率对外接口
uint8_t HP6_Hr_Start(void);
uint8_t HP6_Hr_Stop(void);
uint8_t HP6_Hr_ReadResult(hp6_result_t *res);

#endif
