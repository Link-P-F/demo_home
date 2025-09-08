#include "drv_fan.h"
void drv_fan_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//打开时钟
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//输入模式
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;//引脚PC3
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_ResetBits(GPIOC, GPIO_Pin_6);//默认不转
}

void drv_fan_open(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_6);//转
}

void drv_fan_close(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);//不转
}
