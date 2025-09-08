#ifndef __DRV_FAN_H__
#define __DRV_FAN_H__
#include <stm32f4xx.h>
#define FAN_STOP 0
#define FAN_LOW 1500
#define FAN_MEDIUM 2500
#define FAN_HIGH 4500
void drv_fan_init(void);
void drv_fan_open(void);
void drv_fan_close(void);
void drv_fan_set_speed(uint16_t duty);

#endif //__DRV_FAN_H__

