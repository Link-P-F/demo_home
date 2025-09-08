#include "drv_led.h"
#include "drv_usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>


#define TAST_STARTDEPTH 128
#define TASK_STARTPRIORITY 0
TaskHandle_t *task_startHandle;

void task_start( void * pvParameters )
{
	for( ;; )
	{
		// Task code goes here.
	}
}


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断组2
	drv_led_init();
	drv_usart2_init();
	xTaskCreate(task_start, "task_start", TAST_STARTDEPTH, NULL, TASK_STARTPRIORITY, task_startHandle);
	vTaskStartScheduler();
	while(1)
	{
        
	}
}
