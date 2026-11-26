#ifndef __SHT20_H__
#define __SHT20_H__

#include "stm32f4xx.h"
#include "sht20_iic.h"
#include "time.h"


float Get_SHT20_temp(void);
float GET_SHT20_shidu(void);
u8 SHT20_SoftReset(void);



#endif
