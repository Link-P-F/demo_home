#ifndef __DRV_TIME_H__
#define __DRV_TIME_H__
#include <stm32f4xx.h>

//端口
#define TIM2_GPIO_PORT GPIOA
//引脚
#define TIM2_GPIO_PIN GPIO_Pin_1
//时钟
#define TIM2_GPIO_RCC RCC_AHB1Periph_GPIOA
#define TIM2_RCC RCC_APB1Periph_TIM2

void drv_time2_init(void);
void drv_time2_us(uint32_t us);
//void drv_time2_ms(uint32_t ms);
void drv_time8_init(void);

#endif// __DRV_TIME_H__

