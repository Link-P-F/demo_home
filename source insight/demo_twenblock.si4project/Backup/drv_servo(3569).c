#include "drv_servo.h"
void drv_servo_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	RCC_AHB1PeriphClockCmd(SERVO_RCC_A, ENABLE);
	RCC_AHB1PeriphClockCmd(SERVO_RCC_B, ENABLE);
	RCC_APB1PeriphClockCmd(SERVO_RCC_TIM, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = LEFT_FRONT | RIGHT_FRONT;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SERVO_PORT_A, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = LEFT_HIDE | RIGHT_HIDE;
	GPIO_Init(SERVO_PORT_B, &GPIO_InitStruct);

	GPIO_PinAFConfig(SERVO_PORT_A, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_PinAFConfig(SERVO_PORT_A, GPIO_PinSource7, GPIO_AF_TIM3);
	GPIO_PinAFConfig(SERVO_PORT_B, GPIO_PinSource0, GPIO_AF_TIM3);
	GPIO_PinAFConfig(SERVO_PORT_B, GPIO_PinSource1, GPIO_AF_TIM3);

	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 20000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 84-1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = 1500;
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStruct);
	TIM_OC2Init(TIM3, &TIM_OCInitStruct);
    TIM_OC3Init(TIM3, &TIM_OCInitStruct);
    TIM_OC4Init(TIM3, &TIM_OCInitStruct);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_CtrlPWMOutputs(TIM3, ENABLE);

	TIM_Cmd(TIM3, ENABLE);

}
