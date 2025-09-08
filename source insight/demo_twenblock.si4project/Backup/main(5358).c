#include "drv_led.h"
#include "drv_usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "drv_servo.h"
#include "drv_systick.h"
#include <stdio.h>


#define TAST_STARTDEPTH 128
#define TASK_STARTPRIORITY 0
TaskHandle_t task_startHandle;

#define TAST1_TDEPTH 128
#define TASK1_PRIORITY 0
TaskHandle_t task1_Handle;

#define TAST2_DEPTH 128
#define TASK2_PRIORITY 0
TaskHandle_t task2_Handle;


void task1( void * pvParameters )
{
	for( ;; )
	{
		// Task code goes here.
	}
}

void task2( void * pvParameters )
{
	for( ;; )
	{
		// Task code goes here.
	}
}

void task_start( void * pvParameters )
{
	xTaskCreate(task1, "task1", TAST_STARTDEPTH, NULL, TASK_STARTPRIORITY, &task1_Handle);
	xTaskCreate(task2, "task2", TAST_STARTDEPTH, NULL, TASK_STARTPRIORITY, &task2_Handle);
	vTaskDelete(task_startHandle);

}


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断组2
	drv_led_init();
	drv_usart2_init();
	drv_servo_init();
//	xTaskCreate(task_start, "task_start", TAST_STARTDEPTH, NULL, TASK_STARTPRIORITY, &task_startHandle);
//	vTaskStartScheduler();
	while(1)
	{
        drv_led_open(LED_ID_D2);
        TIM_SetCompare3(TIM3, 2000);
        drv_systick_ms(500);
        TIM_SetCompare3(TIM3, 1500);
        drv_led_close(LED_ID_D2);
        drv_systick_ms(500);
	}
}
