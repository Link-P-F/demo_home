#include "drv_led.h"
/***************************************************
函数名称：drv_led_init
函数功能：对led1、led2、led3、ledD2进行初始化
输入参数：无
输出参数：无
其他：		LED引脚、所属组见"drv_led.h"
		led1、led2、led3高电平亮，ledD2低电平亮
***************************************************/
void drv_led_init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStruct;
	//打开时钟
	RCC_AHB1PeriphClockCmd(LED_RCC_123, ENABLE);
	RCC_AHB1PeriphClockCmd(LED_RCC_D2, ENABLE);

	//GPIO配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_25MHz;

	//初始化
	GPIO_InitStruct.GPIO_Pin = LED_PIN_1;
	GPIO_Init(LED_PORT_123, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = LED_PIN_2;
	GPIO_Init(LED_PORT_123, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = LED_PIN_3;
	GPIO_Init(LED_PORT_123, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = LED_PIN_D2;
	GPIO_Init(LED_PORT_D2, &GPIO_InitStruct);

	//默认灯不亮
	GPIO_ResetBits(LED_PORT_123, LED_PIN_1);
	GPIO_ResetBits(LED_PORT_123, LED_PIN_2);
	GPIO_ResetBits(LED_PORT_123, LED_PIN_3);
	GPIO_SetBits(LED_PIN_D2, LED_PIN_D2);
}

/***************************************************
函数名称：drv_led_open
函数功能：根据输入参数打开某个led
输入参数：led_id
输出参数：无
其他：		LED引脚、所属组见"drv_led.h"
		led1、led2、led3高电平亮，ledD2低电平亮
***************************************************/
void drv_led_open(uint32_t led_id)
{
	switch (led_id)
	{
		case LED_ID_1:
			GPIO_SetBits(LED_PORT_1, LED_PIN_1);
			break;
		case LED_ID_2:
			GPIO_SetBits(LED_PORT_2, LED_PIN_2);
			break;
		case LED_ID_3:
			GPIO_SetBits(LED_PORT_3, LED_PIN_3);
			break;
		case LED_ID_D2:
			GPIO_ResetBits(LED_PORT_D2, LED_PIN_D2);
			break;
	}
}

/***************************************************
函数名称：drv_led_close
函数功能：根据输入参数关闭某个led
输入参数：led_id
输出参数：无
其他：		LED引脚、所属组见"drv_led.h"
		led1、led2、led3高电平亮，ledD2低电平亮
***************************************************/
void drv_led_close(uint32_t pin)
{
	switch (led_id)
	{
		case LED_ID_1:
			GPIO_ResetBits(LED_PORT_1, LED_PIN_1);
			break;
		case LED_ID_2:
			GPIO_ResetBits(LED_PORT_2, LED_PIN_2);
			break;
		case LED_ID_3:
			GPIO_ResetBits(LED_PORT_3, LED_PIN_3);
			break;
		case LED_ID_D2:
			GPIO_SetBits(LED_PORT_D2, LED_PIN_D2);
			break;
	}
}
