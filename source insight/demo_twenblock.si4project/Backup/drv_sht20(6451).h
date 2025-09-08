#ifndef __DRV_SHT20_H__
#define __DRV_SHT20_H__
#include <stm32f4xx.h>
typedef struct
{
	uint8_t tem_h;
	uint8_t tem_l;
	uint8_t hum_h;
	uint8_t hum_l;
	double tem;
	double hum;
}sht20_val;

#endif //__DRV_SHT20_H__

