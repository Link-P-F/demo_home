#include "drv_fan.h"
#include "drv_time.h"
/**********************************
函数名称: drv_fan_init
函数作用: 打开风扇
输入参数: 无
输出参数: 无
其    他: io口: PC6
**********************************/
void drv_fan_init(void)
{
	drv_time8_init();

	GPIO_ResetBits(GPIOC, GPIO_Pin_6);//默认不转
}
/**********************************
函数名称: drv_fan_open
函数作用: 打开化风扇
输入参数: 无
输出参数: 无
其    他: 
**********************************/

void drv_fan_open(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_6);//转
}
/**********************************
函数名称: drv_fan_close
函数作用: 关闭化风扇
输入参数: 无
输出参数: 无
其    他: 
**********************************/

void drv_fan_close(void)
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_6);//不转
}
/**********************************
函数名称: drv_fan_set_speed
函数作用: 更换风扇档位
输入参数: 无
输出参数: 无
其    他: 
**********************************/

void drv_fan_set_speed(uint16_t duty)
{
	if(duty > 4999) duty = 4999;
	TIM_SetCompare1(TIM8, duty);
}

