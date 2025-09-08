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
extern sht20_val tem_hum_data;

void drv_sht20_init(void);
uint8_t drv_sht20_reset(void);
uint16_t drv_sht20_read_tem(void);
double drv_sht20_gettem(void);
uint16_t drv_sht20_read_hum(void);
double drv_sht20_gethum(void);

#endif //__DRV_SHT20_H__

