#include "drv_servo.h"
#include "drv_systick.h"
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
	TIM_OCInitStruct.TIM_Pulse = 1500;//站立1.5ms
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

void drv_servo_set_angle(uint8_t channel, uint16_t angle) 
{
	uint16_t pulse;
    // 限制角度范围（0~180°）
    if (angle > 180) angle = 180;
    // 计算脉冲宽度：0.5ms(500μs)~2.5ms(2500μs)对应0~180°
    pulse = 500 + (angle * 2000) / 180; 
    // 设置对应通道的脉冲值
    switch(channel) {
        case 1: TIM_SetCompare1(TIM3, pulse); break;//右前腿
        case 2: TIM_SetCompare2(TIM3, pulse); break;//左前腿
        case 3: TIM_SetCompare3(TIM3, pulse); break;//右后腿
        case 4: TIM_SetCompare4(TIM3, pulse); break;//左后腿
    }
}

void drv_servo_stand(void)
{
	drv_servo_set_angle(1,90);
	drv_servo_set_angle(2,90);
	drv_servo_set_angle(3,90);
	drv_servo_set_angle(4,90);
}

void drv_servo_down(void)
{
	drv_servo_set_angle(1,180);
	drv_servo_set_angle(2,0);
	drv_servo_set_angle(3,180);
	drv_servo_set_angle(4,0);
}

void drv_servo_sit_down(void)
{
	drv_servo_set_angle(1,90);
	drv_servo_set_angle(2,90);
	drv_servo_set_angle(3,180);
	drv_servo_set_angle(4,0);
}

void drv_servo_advance(void)
{
	//while(1)
	//{
		LEFTF_UP;
        RIGHTH_UP;
		//drv_systick_ms(200);
		vTaskDelay(pdMS_TO_TICKS(200));
		LEFTH_DOWN;
		RIGHTF_DOWN;
		//drv_systick_ms(200);
		vTaskDelay(pdMS_TO_TICKS(200));
		LEFTF_STAND;
		RIGHTH_STAND;
		//drv_systick_ms(200);
		vTaskDelay(pdMS_TO_TICKS(200));
		LEFTH_STAND;
		RIGHTF_STAND;
		//drv_systick_ms(200);
		vTaskDelay(pdMS_TO_TICKS(200));
	//}
}

void drv_servo_back(void)
{
	//while(1)
	//{
		LEFTF_DOWN;
		RIGHTH_DOWN;
		//drv_systick_ms(200);
		vTaskDelay(pdMS_TO_TICKS(200));
		LEFTH_UP;
		RIGHTF_UP;
		//drv_systick_ms(200);
		vTaskDelay(pdMS_TO_TICKS(200));
		LEFTF_STAND;
		RIGHTH_STAND;
		//drv_systick_ms(200);
		vTaskDelay(pdMS_TO_TICKS(200));
		LEFTH_STAND;
		RIGHTF_STAND;
		//drv_systick_ms(200);
		vTaskDelay(pdMS_TO_TICKS(200));
	//}
}



