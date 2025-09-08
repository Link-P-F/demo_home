#ifndef __DRV_USART_H__
#define __DRV_USART_H__
#include <stm32f4xx.h>
#include <stdio.h>
#include <string.h>
#include "drv_led.h"
//端口
#define USART2_GPIO_PORT GPIOA
//引脚
#define USART2_TX GPIO_Pin_2
#define USART2_RX GPIO_Pin_3
//时钟
#define USART2_GPIO_CLOCK RCC_AHB1Periph_GPIOA
#define USART2_CLOCK RCC_APB1Periph_USART2
void drv_usart2_init(void);
#endif //__DRV_USART_H__
