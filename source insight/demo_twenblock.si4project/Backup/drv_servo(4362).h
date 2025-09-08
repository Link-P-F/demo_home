#ifndef __DRV_SERVO_H__
#define __DRV_SERVO_H__
#include <stm32f4xx.h>
//引脚
#define RIGHT_HIDE GPIO_Pin_0	//PB0
#define LEFT_HIDE GPIO_Pin_1	//PB1
#define LEFT_FRONT GPIO_Pin_6	//PA6
#define RIGHT_FRONT GPIO_Pin_7	//PA7
//端口 
#define SERVO_PORT_B GPIOB
#define SERVO_PORT_A GPIOA
//时钟
#define SERVO_RCC_A RCC_AHB1Periph_GPIOA
#define SERVO_RCC_B RCC_AHB1Periph_GPIOB
#define SERVO_RCC_TIM RCC_APB1Periph_TIM3
void drv_servo_init(void);
void drv_servo_set_angle(TIM_TypeDef* TIMx, uint8_t channel, uint16_t angle);
void drv_servo_set_all_mid(void);
#endif //__DRV_SERVO_H__
