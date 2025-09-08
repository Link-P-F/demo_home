#include "drv_led.h"
#include "drv_usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "drv_servo.h"
#include "drv_systick.h"
#include <stdio.h>


#define TAST_STARTDEPTH 128
#define TASK_STARTPRIORITY 4
TaskHandle_t task_startHandle;

#define TAST1_DEPTH 128
#define TASK1_PRIORITY 0
TaskHandle_t task1_Handle;


#define TAST2_DEPTH 128
#define TASK2_PRIORITY 0
TaskHandle_t task2_Handle;


#define TAST3_DEPTH 128
#define TASK3_PRIORITY 0
TaskHandle_t task3_Handle;


#define TAST4_DEPTH 128
#define TASK4_PRIORITY 0
TaskHandle_t task4_Handle;


static uint8_t task_status[5] = {0};
static char str[50];
static uint8_t str_index = 0;
static uint16_t data;

void task1( void * pvParameters )
{
	vTaskSuspend(&task2_Handle);
	drv_servo_advance();
	vTaskDelay(pdMS_TO_TICKS(10));
}

void task2( void * pvParameters )
{
	vTaskSuspend(&task1_Handle);
	drv_servo_back();
	vTaskDelay(pdMS_TO_TICKS(10));
}

void task3( void * pvParameters )
{
	drv_led_run();
	vTaskDelay(pdMS_TO_TICKS(10));
}

void task4( void * pvParameters )
{
	for(;;)
	{
		if(strcmp(str,"LEDD2 OPEN") == 0)
		{
			drv_led_open(LED_ID_D2);
		}
		else if(strcmp(str,"LEDD2 CLOSE") == 0)
		{
			drv_led_close(LED_ID_D2);
		}
		else if(strcmp(str,"SERVO STAND") == 0)
		{
			drv_servo_stand();
		}
	}
	vTaskDelay(pdMS_TO_TICKS(10));
}



void task_start( void * pvParameters )
{
	for(;;)
	{
		if(USART_GetFlagStatus(USART2, USART_IT_RXNE))
		{
			USART_ClearFlag(USART2, USART_IT_RXNE);
			data = USART_ReceiveData(USART2);
	
			if(data != '\r'&& data != '\n')
			{
				str[str_index] = data;
				str_index++;
			}
			else
			{
				str[str_index] = '\0';
				if(strcmp(str,"SERVO ADVANCE") == 0)
				{
					if(task_status[1] == 0)
					{
						task_status[1] = 1;
						xTaskCreate(task1, "task1", TAST1_DEPTH, NULL, TASK1_PRIORITY, &task1_Handle);
						vTaskStartScheduler();
					}
					else
					{
						vTaskResume(&task1_Handle);
					}
				}
				else if(strcmp(str,"SERVO BACK") == 0)
				{
					if(task_status[2] == 0)
					{
						task_status[2] = 1;
						xTaskCreate(task2, "task2", TAST2_DEPTH, NULL, TASK2_PRIORITY, &task2_Handle);
						vTaskStartScheduler();
					}
					else
					{
						vTaskResume(&task2_Handle);
					}
				}
				else if(strcmp(str,"LED RUN") == 0)
				{
					if(task_status[3] == 0)
					{
						task_status[3] = 1;
						xTaskCreate(task3, "task3", TAST3_DEPTH, NULL, TASK3_PRIORITY, &task3_Handle);
						vTaskStartScheduler();
					}
					else
					{
						vTaskResume(&task3_Handle);
					}
				}
				else
				{
					if(task_status[4] == 0)
					{	
						task_status[4] = 1;
						xTaskCreate(task4, "task4", TAST3_DEPTH, NULL, TASK3_PRIORITY, &task4_Handle);
						vTaskStartScheduler();
					}
				}
			str_index = 0;
			}
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

}


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断组2
	drv_led_init();
    drv_systick_init(168);
	drv_usart2_init();
	drv_servo_init();
    drv_systick_ms(500);
	task_status[0] = 1;
	xTaskCreate(task_start, "task_start", TAST_STARTDEPTH, NULL, TASK_STARTPRIORITY, &task_startHandle);
	vTaskStartScheduler();
}

