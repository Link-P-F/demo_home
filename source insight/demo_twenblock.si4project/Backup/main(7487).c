#include "drv_led.h"
#include "drv_usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "drv_servo.h"
#include "drv_systick.h"
#include <string.h>
#include <stdio.h>


#define TAST_STARTDEPTH 128
#define TASK_STARTPRIORITY 4
TaskHandle_t task_startHandle = NULL;

#define TASK_ADVANCE_DEPTH 128
#define TASK_ADVANCE_PRIORITY 0
TaskHandle_t task_advance_Handle = NULL;


#define TASK_BACK_DEPTH 128
#define TASK_BACK_PRIORITY 0
TaskHandle_t task_back_Handle = NULL;


#define TASK_LED_RUN_DEPTH 128
#define TASK_LED_RUN_PRIORITY 0
TaskHandle_t task_led_run_Handle = NULL;


#define TASK_CMD_READ_DEPTH 128
#define TASK_CMD_READ_PRIORITY 3
TaskHandle_t task_cmd_read_Handle = NULL;

//双缓存防止数据争夺
static char cmd_write[50] = {0};//写入数据
static char cmd_read[50] = {0};//读取数据
static char cmd_ready = 0;//标志位，0表示可以写入数据

static uint8_t cmd_index = 0;
static uint16_t data;

void task_advance( void * pvParameters )
{
	for(;;)
	{
		//任务存在且就绪则暂停
		if(task_back_Handle != NULL && eTaskGetState(task_back_Handle) == eReady)
		{
			vTaskSuspend(task_back_Handle);
		}
		drv_servo_advance();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void task_back( void * pvParameters )
{
	for(;;)
	{
		//任务存在且就绪则暂停
		if(task_advance_Handle != NULL && eTaskGetState(task_advance_Handle) == eReady)
		{
			vTaskSuspend(task_advance_Handle);
		}
		drv_servo_back();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void task_led_run( void * pvParameters )
{
	for(;;)
	{
		drv_led_run();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

void task_cmd_read( void * pvParameters )
{
	for(;;)
	{
		if(cmd_ready==1)
		{
			strncpy(cmd_read, cmd_write, sizeof(cmd_read)-1);//瞬间将数据写入读取数据部分
			cmd_read[sizeof(cmd_read)-1] = '\0';

			cmd_ready = 0;//继续接收数据
			
			if(strcmp(cmd_read,"SERVO ADVANCE") == 0)
			{
				if(task_advance_Handle == NULL)
				{
					xTaskCreate(task_advance, "task_advance", TASK_ADVANCE_DEPTH, NULL, TASK_ADVANCE_PRIORITY, &task_advance_Handle);
				}
				else
				{
					vTaskResume(task_advance_Handle);
				}
				}
			else if(strcmp(cmd_read,"SERVO BACK") == 0)
			{
				if(task_back_Handle == NULL)
				{
					xTaskCreate(task_back, "task_back", TASK_BACK_DEPTH, NULL, TASK_BACK_PRIORITY, &task_back_Handle);
				}
				else
				{
					vTaskResume(task_back_Handle);
				}
			}
			else if(strcmp(cmd_read,"LED RUN") == 0)
			{
				if(task_led_run_Handle == NULL)
				{
					xTaskCreate(task_led_run, "task_led_run", TASK_LED_RUN_DEPTH, NULL, TASK_LED_RUN_PRIORITY, &task_led_run_Handle);
				}
				else
				{
					vTaskResume(task_led_run_Handle);
				}
			}
			else if(strcmp(cmd_read,"LEDD2 OPEN") == 0)
			{
				drv_led_open(LED_ID_D2);
			}
			else if(strcmp(cmd_read,"LEDD2 CLOSE") == 0)
			{
				drv_led_close(LED_ID_D2);
			}
			else if(strcmp(cmd_read,"SERVO STAND") == 0)
			{
				if(task_advance_Handle)
				{
					vTaskSuspend(task_advance_Handle);
				}
				if(task_back_Handle)
				{
					vTaskSuspend(task_back_Handle);
				}
				drv_servo_stand();
			}
			else if(strcmp(cmd_read,"SERVO SIT") == 0)
			{
				if(task_advance_Handle)
				{
					vTaskSuspend(task_advance_Handle);
				}
				if(task_back_Handle)
				{
					vTaskSuspend(task_back_Handle);
				}
				drv_servo_sit_down();
			}
			else if(strcmp(cmd_read,"SERVO DOWN") == 0)
			{
				if(task_advance_Handle)
				{
					vTaskSuspend(task_advance_Handle);
				}
				if(task_back_Handle)
				{
					vTaskSuspend(task_back_Handle);
				}
				drv_servo_down();
			}
			else
			{
				
			}
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}



void task_start( void * pvParameters )
{
	if(task_cmd_read_Handle == NULL)
	{		
		xTaskCreate(task_cmd_read, "task_cmd_read", TASK_CMD_READ_DEPTH, NULL, TASK_CMD_READ_PRIORITY, &task_cmd_read_Handle);
	}
	for(;;)
	{
		taskENTER_CRITICAL();
		if(cmd_ready == 0 && USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)
		{
			USART_ClearFlag(USART2, USART_FLAG_RXNE);
			data = USART_ReceiveData(USART2);
	
			if(data != '\r'&& data != '\n')
			{
				cmd_write[cmd_index] = data;
				cmd_index++;
			}
			else
			{
				cmd_write[cmd_index] = '\0';
				cmd_index = 0;
				cmd_ready = 1;
			}
		}
		taskEXIT_CRITICAL();
		if(cmd_ready == 1)
		{
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
	xTaskCreate(task_start, "task_start", TAST_STARTDEPTH, NULL, TASK_STARTPRIORITY, &task_startHandle);
	vTaskStartScheduler();
	// 调度器启动失败（内存不足）
    //while(1)
    //{
        //drv_led_open(LED_ID_D2);
        //drv_systick_ms(500);
		//drv_led_close(LED_ID_D2);
		//drv_systick_ms(500);
    //}
}

