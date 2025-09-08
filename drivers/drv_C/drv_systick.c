#include "drv_systick.h"
static uint32_t fs_us = 0;
static uint32_t fs_ms = 0;

void drv_systick_init(uint8_t systick)
{
	SysTick->CTRL &= ~0x05;//关闭时钟
	fs_us = systick/8;
	fs_ms = fs_us * 1000;
}

void drv_systick_ms(uint16_t ms)
{
	uint32_t temp = 0;
    int i=0;
    SysTick->CTRL |= 0x01;
    for(i=0;i<ms;i++)
	{
        SysTick->LOAD = fs_ms;
        do
        {
            temp = SysTick->CTRL;
        }
        while (!(temp&(0x01<<16)));
    }
    SysTick->CTRL &= ~0x01; 
	SysTick->VAL = 0;
}

void drv_systick_us(uint16_t us)
{
	uint32_t temp = 0;
	SysTick->LOAD = fs_us*us;
    SysTick->CTRL |= 0x01;
    do
    {
        temp = SysTick->CTRL;
    }
    while (!(temp&(0x01<<16)));
	SysTick->CTRL &= ~0x01;
	SysTick->VAL = 0;
}

