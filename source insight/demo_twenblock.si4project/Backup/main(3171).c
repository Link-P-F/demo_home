#include <stm32f4xx.h>
#include "drv_led.h"
#include "drv_usart.h"
#include "drv_fan.h"
#include "drv_servo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
#include <stdio.h>

SemaphoreHandle_t xUartSemaphore = NULL;//通信量用于同步

//命令解析任务配置
#define TASK_CMD_DEPTH 128
#define TASK_CMD_PRIOPITY 4
TaskHandle_t task_cmd_Handle = NULL;

//前进任务配置
#define TASK_ADVANCE_DEPTH 128
#define TASK_ADVANCE_PRIOPITY 3
TaskHandle_t task_advance_Handle = NULL;

//后退任务配置
#define TASK_BACK_DEPTH 128
#define TASK_BACK_PRIOPITY 3
TaskHandle_t task_back_Handle = NULL;

//流水灯任务配置
#define TASK_LED_RUN_DEPTH 128
#define TASK_LED_RUN_PRIOPITY 3
TaskHandle_t task_led_run_Handle = NULL;



char cmd_write[50] = {0};
char cmd_receive[50] = {0};
uint8_t cmd_index = 0;

void task_advance(void * pvParameters)
{
	printf("ACVANCE...\r\n");
	for(;;)
	{
		drv_servo_advance();
	}
}

void task_back(void * pvParameters)
{
	printf("BACK...\r\n");
	for(;;)
	{
		drv_servo_back();
	}
}

void task_led_run(void * pvParameter)
{
	printf("LED RUN...\r\n");
	for(;;)
	{
		drv_led_run();
	}
}

void task_cmd(void * pvParameters)
{
	for(;;)
	{	
		int i=0;
		printf("CMD RUN...\r\n");
		if(xSemaphoreTake(xUartSemaphore, portMAX_DELAY) == pdPASS)	
		{
			printf("cmding\r\n");
			printf("cmd_write_main:%s\r\n",cmd_write);
			strncpy(cmd_receive, (const char*)cmd_write, sizeof(cmd_receive)-1);//瞬间将数据写入读取数据部分
			cmd_receive[sizeof(cmd_receive)-1] = '\0';

			//数据处理
			if(strcmp(cmd_receive, "SERVO ADVANCE") == 0)
			{
				if(task_back_Handle!=NULL)
				{
					vTaskSuspend(task_back_Handle);
				}
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_advance_Handle==NULL)
				{
					xTaskCreate(task_advance, "task_advance", TASK_ADVANCE_DEPTH, NULL, TASK_ADVANCE_PRIOPITY, &task_advance_Handle);
				}
				else
				{
					vTaskResume(task_advance_Handle);
				}
			}
			else if(strcmp(cmd_receive, "SERVO BACK") == 0)
			{
				if(task_advance_Handle!=NULL)
				{
					vTaskSuspend(task_advance_Handle);
				}
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_back_Handle==NULL)
				{
					xTaskCreate(task_back, "task_back", TASK_BACK_DEPTH, NULL, TASK_BACK_PRIOPITY, &task_back_Handle);
				}
				else
				{
					vTaskResume(task_back_Handle);
				}
			}
			else if(strcmp(cmd_receive, "LED RUN") == 0)
			{
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_led_run_Handle==NULL)
				{
					xTaskCreate(task_led_run, "task_led_run", TASK_LED_RUN_DEPTH, NULL, TASK_LED_RUN_PRIOPITY, &task_led_run_Handle);
				}
				else
				{
					vTaskResume(task_led_run_Handle);
				}
			}
			else if(strcmp(cmd_receive, "SERVO SIT") == 0)
			{	
				if(task_advance_Handle!=NULL)
				{
					vTaskSuspend(task_advance_Handle);
				}
				if(task_back_Handle!=NULL)
				{
					vTaskSuspend(task_back_Handle);
				}
				drv_servo_sit_down();
			}
			else if(strcmp(cmd_receive, "SERVO STAND") == 0)
			{	
				if(task_advance_Handle!=NULL)
				{
					vTaskSuspend(task_advance_Handle);
				}
				if(task_back_Handle!=NULL)
				{
					vTaskSuspend(task_back_Handle);
				}
				drv_servo_stand();
			}
			else if(strcmp(cmd_receive, "SERVO DOWN") == 0)
			{
				if(task_advance_Handle!=NULL)
				{
					vTaskSuspend(task_advance_Handle);
				}
				if(task_back_Handle!=NULL)
				{
					vTaskSuspend(task_back_Handle);
				}
				drv_servo_down();
			}
			else if(strcmp(cmd_receive, "LEDD2 OPEN") == 0)
			{
				drv_led_open(LED_ID_D2);
			}
			else if(strcmp(cmd_receive, "LEDD2 CLOSE") == 0)
			{
				drv_led_close(LED_ID_D2);
			}
			else
			{
				for(i = 0;i<10;i++)
				{
					drv_led_open(LED_ID_D2);
					vTaskDelay(pdMS_TO_TICKS(500));
					drv_led_close(LED_ID_D2);
					vTaskDelay(pdMS_TO_TICKS(500));
				}
			}
			vTaskDelay(pdMS_TO_TICKS(10));
		}
	}
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//中断组
	drv_led_init();
	drv_servo_init();
	drv_usart2_init();//用于接收语音模块数据
	drv_ustart6_init();//用于输出到电脑

	xUartSemaphore = xSemaphoreCreateBinary();

	xTaskCreate(task_cmd, "task_cmd", TASK_CMD_DEPTH, NULL, TASK_CMD_PRIOPITY, &task_cmd_Handle);
	vTaskStartScheduler();
	
}

