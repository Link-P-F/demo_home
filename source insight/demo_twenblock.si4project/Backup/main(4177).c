#include "drv_led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "drv_servo.h"
#include "drv_systick.h"
#include <string.h>
#include <stdio.h>
#include "semphr.h"
#include "drv_usart.h"
#include "event_groups.h"

#define TAST_STARTDEPTH 512
#define TASK_STARTPRIORITY 4
TaskHandle_t task_startHandle = NULL;

#define TASK_ADVANCE_DEPTH 512
#define TASK_ADVANCE_PRIORITY 3
TaskHandle_t task_advance_Handle = NULL;


#define TASK_BACK_DEPTH 512
#define TASK_BACK_PRIORITY 3
TaskHandle_t task_back_Handle = NULL;


#define TASK_LED_RUN_DEPTH 512
#define TASK_LED_RUN_PRIORITY 2
TaskHandle_t task_led_run_Handle = NULL;

//双缓存防止数据争夺
volatile char cmd_write[50] = {0};//写入数据
static char cmd_read[50] = {0};//读取数据
volatile char cmd_ready = 0;//标志位，0表示可以写入数据
volatile uint8_t cmd_index = 0;
volatile uint8_t uart2_irq_count = 0;

SemaphoreHandle_t xUartSemaphore = NULL;

void task_advance( void * pvParameters )
{
	for(;;)
	{
		//任务存在则暂停
		if(task_back_Handle != NULL)
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
		//任务存在则暂停
		if(task_advance_Handle != NULL)
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

void task_start( void * pvParameters )
{
	for(;;)
	{
		printf("task_start running... irq_count:%d, cmd_ready:%d\r\n", uart2_irq_count, cmd_ready);
		if(xSemaphoreTake(xUartSemaphore, portMAX_DELAY) == pdPASS && cmd_ready==1)
		{	
			strncpy(cmd_read, (const char*)cmd_write, sizeof(cmd_read)-1);//瞬间将数据写入读取数据部分
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

int main(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//中断组4
	drv_led_init();
    drv_systick_init(168);
	drv_usart2_init();
	drv_ustart6_init();
	drv_servo_init();

	xUartSemaphore = xSemaphoreCreateBinary();
	if(xUartSemaphore == NULL)
    {
        // 信号量创建失败，用LED闪烁报错（假设drv_led_open/close可用）
        while(1)
        {
            drv_led_open(LED_ID_D2);
            drv_systick_ms(500);
            drv_led_close(LED_ID_D2);
            drv_systick_ms(500);
        }
    }

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

