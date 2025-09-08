#include <stm32f4xx.h>
#include "drv_led.h"
#include "drv_usart.h"
#include "drv_servo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include <stido.h>

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


char cmd_write[50] = {0};
char cmd_receive[50] = {0};
uint8_t cmd_index = 0;

void task_advance(void * pvParameters)
{
	printf("ACVANCE...\r\n")；
	for(;;)
	{
		drv_servo_advance();
	}
}

void task_back(void * pvParameters)
{
	printf("BACK...\r\n");
}

void task_cmd(void * pvParameters)
{
	for(;;)
	{
		if(xSemaphoreTake(xUartSemaphore, portMAX_DELAY) == pdPASS)	
		{
			strncpy(cmd_receive, (const char*)cmd_write, sizeof(cmd_receive)-1);//瞬间将数据写入读取数据部分
			cmd_receive[sizeof(cmd_receive)-1] = '\0';

			//数据处理
			if(strcmp(cmd_receive, "SERVO ADVANCE") == 0)
			{
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_advance_Handle==NULL)
				{
					xTaskCreate(task_cmd, "task_cmd", TASK_CMD_DEPTH, NULL, TASK_CMD_PRIOPITY, &task_cmd_Handle);
				}
				else
				{
					vTaskResume(task_advance_Handle);
				}
			}
			else if(strcmp(cmd_receive, "SERVO BACK") == 0)
		}
	}
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//中断组
	drv_led_init();
	drv_usart2_init();//用于接收语音模块数据
	drv_ustart6_init();//用于输出到电脑

	xUartSemaphore = xSemaphoreCreateBinary();

	xTaskCreate(task_cmd, "task_cmd", TASK_CMD_DEPTH, NULL, TASK_CMD_PRIOPITY, &task_cmd_Handle);
	vTaskStartScheduler();
	
}

