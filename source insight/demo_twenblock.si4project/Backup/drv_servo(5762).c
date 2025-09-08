#include "drv_servo.h"
#include "drv_systick.h"
#include "FreeRTOS.h"
#include "task.h"
/***************************************************
函数名称：drv_servo_init
函数功能：初始化舵机
输入参数：无
输出参数：无
其他说明：占用io口
		左前腿->PA7(通道2)		右前腿->PA6(通道1)
		左后腿->PB1(通道4)		右后腿->PB0(通道3)
***************************************************/

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


/***************************************************
函数名称：drv_servo_set_angle
函数功能：控制四条腿的转动
输入参数：channel(通道号，控制哪条腿),angle(控制转动角度)
输出参数：无
其他说明：占用io口
		左前腿->PA7(通道2)		右前腿->PA6(通道1)
		左后腿->PB1(通道4)		右后腿->PB0(通道3)
***************************************************/
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

/***************************************************
函数名称：drv_servo_stand
函数功能：控制舵机站立
输入参数：无
输出参数：无
其他说明：占用io口
		左前腿->PA7(通道2)		右前腿->PA6(通道1)
		左后腿->PB1(通道4)		右后腿->PB0(通道3)
***************************************************/
void drv_servo_stand(void)
{
	drv_servo_set_angle(1,90);
	drv_servo_set_angle(2,90);
	drv_servo_set_angle(3,90);
	drv_servo_set_angle(4,90);
}

/***************************************************
函数名称：drv_servo_down
函数功能：控制舵机趴下
输入参数：无
输出参数：无
其他说明：占用io口
		左前腿->PA7(通道2)		右前腿->PA6(通道1)
		左后腿->PB1(通道4)		右后腿->PB0(通道3)
***************************************************/
void drv_servo_down(void)
{
	drv_servo_set_angle(1,180);
	drv_servo_set_angle(2,0);
	drv_servo_set_angle(3,180);
	drv_servo_set_angle(4,0);
}

/***************************************************
函数名称：drv_servo_sit_down
函数功能：控制舵机坐下
输入参数：无
输出参数：无
其他说明：占用io口
		左前腿->PA7(通道2)		右前腿->PA6(通道1)
		左后腿->PB1(通道4)		右后腿->PB0(通道3)
***************************************************/
void drv_servo_sit_down(void)
{
	drv_servo_set_angle(1,90);
	drv_servo_set_angle(2,90);
	drv_servo_set_angle(3,180);
	drv_servo_set_angle(4,0);
}

/***************************************************
函数名称：drv_servo_advance
函数功能：控制舵机前进
输入参数：无
输出参数：无
其他说明：占用io口
		左前腿->PA7(通道2)		右前腿->PA6(通道1)
		左后腿->PB1(通道4)		右后腿->PB0(通道3)
***************************************************/
void drv_servo_advance(void)
{
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
}

/***************************************************
函数名称：drv_servo_back
函数功能：控制舵机后退
输入参数：无
输出参数：无
其他说明：占用io口
		左前腿->PA7(通道2)		右前腿->PA6(通道1)
		左后腿->PB1(通道4)		右后腿->PB0(通道3)
***************************************************/
void drv_servo_back(void)
{
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
}

/***************************************************
函数名称：drv_servo_turn_left
函数功能：控制舵机左转（四足同步转向）
输入参数：无
输出参数：无
其他说明：右侧腿（右前+右后）向前摆，左侧腿保持站立，形成左转扭矩
        左前腿->PA7(通道2)  右前腿->PA6(通道1)
        左后腿->PB1(通道4)  右后腿->PB0(通道3)
***************************************************/
void drv_servo_turn_left(void)
{
    // 第1步：右侧腿（右前+右后）向前抬起，左侧腿保持站立
    RIGHTF_UP;    // 右前腿向前（135°）
    RIGHTH_UP;    // 右后腿向前（135°）
    LEFTF_STAND;  // 左前腿保持站立（90°）
    LEFTH_STAND;  // 左后腿保持站立（90°）
    vTaskDelay(pdMS_TO_TICKS(200));  // 等待舵机到位（FreeRTOS延迟）

    // 第2步：右侧腿回正，左侧腿轻微向后（辅助转向稳定）
    RIGHTF_STAND; // 右前腿回正（90°）
    RIGHTH_STAND; // 右后腿回正（90°）
    LEFTF_DOWN;   // 左前腿轻微向后（135°）
    LEFTH_DOWN;   // 左后腿轻微向后（135°）
    vTaskDelay(pdMS_TO_TICKS(200));

    // 第3步：左侧腿回正，右侧腿再次向前（强化转向角度）
    LEFTF_STAND;  // 左前腿回正（90°）
    LEFTH_STAND;  // 左后腿回正（90°）
    RIGHTF_UP;    // 右前腿再次向前（135°）
    RIGHTH_UP;    // 右后腿再次向前（135°）
    vTaskDelay(pdMS_TO_TICKS(200));

    // 第4步：所有腿回正，完成一次左转循环
    RIGHTF_STAND; // 右前腿回正（90°）
    RIGHTH_STAND; // 右后腿回正（90°）
    LEFTF_STAND;  // 左前腿回正（90°）
    LEFTH_STAND;  // 左后腿回正（90°）
    vTaskDelay(pdMS_TO_TICKS(200));
}

/***************************************************
函数名称：drv_servo_turn_right
函数功能：控制舵机右转（四足同步转向）
输入参数：无
输出参数：无
其他说明：左侧腿（左前+左后）向前摆，右侧腿保持站立，形成右转扭矩
        左前腿->PA7(通道2)  右前腿->PA6(通道1)
        左后腿->PB1(通道4)  右后腿->PB0(通道3)
***************************************************/
void drv_servo_turn_right(void)
{
    // 第1步：左侧腿（左前+左后）向前抬起，右侧腿保持站立
    LEFTF_UP;     // 左前腿向前（45°，左前腿机械结构与右前腿对称）
    LEFTH_UP;     // 左后腿向前（45°，左后腿机械结构与右后腿对称）
    RIGHTF_STAND; // 右前腿保持站立（90°）
    RIGHTH_STAND; // 右后腿保持站立（90°）
    vTaskDelay(pdMS_TO_TICKS(200));

    // 第2步：左侧腿回正，右侧腿轻微向后（辅助转向稳定）
    LEFTF_STAND;  // 左前腿回正（90°）
    LEFTH_STAND;  // 左后腿回正（90°）
    RIGHTF_DOWN;  // 右前腿轻微向后（45°）
    RIGHTH_DOWN;  // 右后腿轻微向后（45°）
    vTaskDelay(pdMS_TO_TICKS(200));

    // 第3步：右侧腿回正，左侧腿再次向前（强化转向角度）
    RIGHTF_STAND; // 右前腿回正（90°）
    RIGHTH_STAND; // 右后腿回正（90°）
    LEFTF_UP;     // 左前腿再次向前（45°）
    LEFTH_UP;     // 左后腿再次向前（45°）
    vTaskDelay(pdMS_TO_TICKS(200));

    // 第4步：所有腿回正，完成一次右转循环
    LEFTF_STAND;  // 左前腿回正（90°）
    LEFTH_STAND;  // 左后腿回正（90°）
    RIGHTF_STAND; // 右前腿回正（90°）
    RIGHTH_STAND; // 右后腿回正（90°）
    vTaskDelay(pdMS_TO_TICKS(200));
}


