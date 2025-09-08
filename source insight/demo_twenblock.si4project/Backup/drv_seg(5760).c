#include "drv_seg.h"
#include "drv_spi.h"
#include "drv_systick.h"
#include "drv_time.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t task_cmd_Handle;

//八段共阴极数码管
static uint8_t duan_xuan[] = {
    0x3f,  // 0
    0x06,  // 1
    0x5b,  // 2
    0x4f,  // 3
    0x66,  // 4
    0x6d,  // 5
    0x7d,  // 6
    0x07,  // 7
    0x7f,  // 8
    0x6f,  // 9
    0x77,  // A
    0x7c,  // B
    0x39,  // C
    0x5e,  // D
    0x79,  // E
    0x71   // F
};

static uint8_t wei_xuan[] = {
	0x01,
	0x02,
	0x04,
	0x08
};

/***************************************************
函数名称：drv_74hc595_init
函数功能：初始化数码管
输入参数：无
输出参数：无
其他说明：		
***************************************************/
void drv_74hc595_init(void)
{
	uint8_t i;
	drv_spi2_init();
	for(i = 0; i < 4; i++)
	{
		drv_seg_display(i, 0x00);
	}
}

/***************************************************
函数名称：drv_seg_display
函数功能：选择数码管打印数据
输入参数：无
输出参数：无
其他说明：		
***************************************************/
void drv_seg_display(uint8_t wei, uint8_t duan)
{
	drv_spi2_senddata(wei_xuan[wei]);
	drv_spi2_senddata(duan);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	drv_time2_us(1000);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}

/***************************************************
函数名称：drv_seg_display_double
函数功能：对温湿度数据进行处理打印
输入参数：无
输出参数：无
其他说明：		
***************************************************/
void drv_seg_display_double(double num)
{
	uint16_t val;
	uint8_t data[4];
	int i = 0;

	if(num<0 || num>=100)
	{
		taskENTER_CRITICAL();
		drv_seg_display(0, 0x40);
		drv_seg_display(1, 0x40);
		drv_seg_display(2, 0x40);
		drv_seg_display(3, 0x40);
		taskEXIT_CRITICAL();
		return ;
	}
	
	val = (uint16_t)(num * 100);
	data[0] = (val/1000)%10;

	data[1] = (val/100)%10;
	data[2] = (val/10)%10;
	data[3] = (val/1)%10;

	taskENTER_CRITICAL();
	for(i=0;i<100;i++)
	{
	drv_seg_display(0, duan_xuan[data[0]]);
	//drv_systick_ms(2);
	drv_time2_ms(2);
	drv_seg_display(1, duan_xuan[data[1]] | 0x80);
	//drv_systick_ms(2);
	drv_time2_ms(2);
	drv_seg_display(2, duan_xuan[data[2]]);
	//drv_systick_ms(2);
	drv_time2_ms(2);
	drv_seg_display(3, duan_xuan[data[3]]);
	drv_seg_display(3, 0x00);
	}
	taskEXIT_CRITICAL();
}

