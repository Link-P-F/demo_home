#ifndef __DRV_LED_H__
#define __DRV_LED_H__
#include <stm32f4xx.h>

//led时钟
#define LED_RCC_123 RCC_AHB1Periph_GPIOC
#define LED_RCC_D2 RCC_AHB1Periph_GPIOF

//led端口组
#define LED_PORT_123 GPIOC
#define LED_PORT_D2 GPIOF

//led引脚
#define LED_PIN_1 GPIO_Pin_4
#define LED_PIN_2 GPIO_Pin_3
#define LED_PIN_3 GPIO_Pin_13
#define LED_PIN_D2 GPIO_Pin_8

//LED编号
#define LED_ID_1     1   
#define LED_ID_2     2  
#define LED_ID_3     3   
#define LED_ID_D2    4  
void drv_led_init(void);
#endif //__DRV_LED_H__
