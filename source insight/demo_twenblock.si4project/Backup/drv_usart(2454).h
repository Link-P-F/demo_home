#ifndef __DRV_USART_H__
#define __DRV_USART_H__
#include <stm32f4xx.h>
#include <stdio.h>
#include <string.h>
#include "drv_led.h"
//端口
#define USART2_GPIO_PORT GPIOA
#define USART6_GPIO_PORT GPIOG
//引脚
#define USART2_TX GPIO_Pin_2
#define USART2_RX GPIO_Pin_3
#define USART_GPIO_PIN_RXD GPIO_Pin_9
#define USART_GPIO_PIN_TXD GPIO_Pin_14

//时钟
#define USART2_GPIO_CLOCK RCC_AHB1Periph_GPIOA
#define USART2_CLOCK RCC_APB1Periph_USART2
#define USART6_GPIO_CLOCK RCC_AHB1Periph_GPIOG
#define USART6_CLOCK RCC_APB2Periph_USART6


#define EVENT_UART_CMD_DONE (1<<0)



void drv_usart2_init(void);
void drv_usart6_init(void);

#endif //__DRV_USART_H__
