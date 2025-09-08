#include "drv_time.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

/***************************************************
函数名称：drv_time2_init
函数功能：定时器初始化，用于微妙级别延时
输入参数：无
输出参数：无
其他说明：		
***************************************************/

void drv_time2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	
	//时钟
	RCC_AHB1PeriphClockCmd(TIM2_GPIO_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(TIM2_RCC, ENABLE);

	//io口初始化配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = TIM2_GPIO_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM2_GPIO_PORT, &GPIO_InitStruct);

	GPIO_PinAFConfig(TIM2_GPIO_PORT, GPIO_PinSource15, GPIO_AF_TIM2);

	//时钟初始化
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 0xFFFFFFFF;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84-1;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
}

/***************************************************
函数名称：drv_time2_us
函数功能：微妙级别延时
输入参数：无
输出参数：无
其他说明：		
***************************************************/
void drv_time2_us(uint32_t us)
{
	//使能计时器
	TIM_Cmd(TIM2, ENABLE);
    TIM2->CNT = 0;
	while(TIM2->CNT < us);
	TIM_Cmd(TIM2, DISABLE);
}

/***************************************************
函数名称：drv_time2_ms
函数功能：毫秒妙级别延时
输入参数：无
输出参数：无
其他说明：要在任务中用		
***************************************************/
/*void drv_time2_ms(uint32_t ms)
{
	TIM_Cmd(TIM2, ENABLE);
	uint32_t ulStartVal = TIM_GetCounter(TIM2);
    uint32_t ulTarget;
	ulTarget = (uint32_t)(ms * (1000UL)); // 84MHz 下，1μs = 1 个计数
	
    // 处理定时器溢出（32 位定时器溢出概率极低，可简化处理）
    while ((TIM_GetCounter(TIM2) - ulStartVal) < ulTarget)
    {
        taskYIELD(); // 可选：延时期间让出 CPU，允许其他任务执行（非忙等）
    }
	TIM_Cmd(TIM2, DISABLE)；
}
*/
/***************************************************
函数名称：drv_time3_init
函数功能：定时器初始化，用于调控风扇档位
输入参数：无
输出参数：无
其他说明：		
***************************************************/
void drv_time3_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);

	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 5000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84-1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);

	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 0;
	TIM_OC1Init(TIM3, &TIM_OCInitStruct);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_CtrlPWMOutputs(TIM3, ENABLE);
	
	TIM_Cmd(TIM3, ENABLE);
}



