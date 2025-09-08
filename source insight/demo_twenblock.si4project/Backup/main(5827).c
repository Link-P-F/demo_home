#include <stm32f4xx.h>
#include "drv_led.h"
#include "drv_beep.h"
#include "drv_usart.h"
#include "drv_fan.h"
#include "drv_servo.h"
#include "drv_sht20.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
#include <stdio.h>
#include "drv_time.h"
#include "drv_spi.h"
#include "drv_seg.h"
#include "drv_systick.h"
#include "oled.h"
#include "drv_oled_show.h"

/*
led:0关闭，1开启
beep:0关闭，1开启
fan:0关闭，1开启
servo:0站立，1坐下，2趴下，3前进，4后退
sht20:0关闭，1读取温度，2读取湿度，3读取温湿度
*/
int status[] = 
{
	0,//0，led1
	0,//1，led2
	0,//2，led3
	0,//3，ledD2
	0,//4，beep
	0,//5，fan
	0,//6，servo
	0,//7，sht20
};

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

//读取温度任务配置
#define TASK_GET_TEM_DEPTH 128
#define TASK_GET_TEM_PRIOPITY 3
TaskHandle_t task_get_tem__Handle = NULL;

//读取湿度任务配置
#define TASK_GET_HUM_DEPTH 128
#define TASK_GET_HUM_PRIOPITY 3
TaskHandle_t task_get_hum__Handle = NULL;

//读取温湿度任务配置
#define TASK_GET_TEM_HUM_DEPTH 128
#define TASK_GET_TEM_HUM_PRIOPITY 3
TaskHandle_t task_get_tem_hum__Handle = NULL;



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

void task_get_tem(void * pvParameter)
{
	printf("GET TEM...\r\n");
	drv_sht20_reset();
	vTaskDelay(pdMS_TO_TICKS(15));
	for(;;)
	{
		drv_sht20_gettem();
		printf("HUM = %.2f\r\n",tem_hum_data.tem);
		drv_seg_display_double(tem_hum_data.tem);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void task_get_hum(void * pvParameter)
{
	printf("GET HUM...\r\n");
	drv_sht20_reset();
	vTaskDelay(pdMS_TO_TICKS(15));
	for(;;)
	{
		drv_sht20_gethum();
		printf("HUM = %.2f\r\n",tem_hum_data.hum);
		drv_seg_display_double(tem_hum_data.hum);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void task_get_tem_hum(void * pvParameter)
{
	printf("GET TEM AND HUM...\r\n");
	drv_sht20_reset();
	vTaskDelay(pdMS_TO_TICKS(15));
	for(;;)
	{
		drv_sht20_gettem();
		drv_sht20_gethum();
		printf("TEMP = %.2f\r\n",tem_hum_data.tem);
		printf("HUM = %.2f\r\n",tem_hum_data.hum);
		//OLED
		drv_oled_tem_hum(tem_hum_data.tem,tem_hum_data.hum);
		vTaskDelay(pdMS_TO_TICKS(1000));
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
			if(strcmp(cmd_receive, "SERVO ADVANCE") == 0)//舵机前进
			{
				//若舵机正在后退，挂起舵机后退任务
				if(status[6] == 4)
				{
					vTaskSuspend(task_back_Handle);
				}
				//前进任务存在，解挂任务，任务不存在，创建任务
				if(task_advance_Handle==NULL)
				{
					status[6] = 3;
					xTaskCreate(task_advance, "task_advance", TASK_ADVANCE_DEPTH, NULL, TASK_ADVANCE_PRIOPITY, &task_advance_Handle);
				}
				else
				{
					status[6] = 3;
					vTaskResume(task_advance_Handle);
				}
			}
			else if(strcmp(cmd_receive, "SERVO BACK") == 0)//舵机后退
			{
				//若舵机正在前进，挂起舵机前进任务
				if(status[6] == 3)
				{
					vTaskSuspend(task_advance_Handle);
				}
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_back_Handle==NULL)
				{
					status[6] = 4;
					xTaskCreate(task_back, "task_back", TASK_BACK_DEPTH, NULL, TASK_BACK_PRIOPITY, &task_back_Handle);
				}
				else
				{
					status[6] = 4;
					vTaskResume(task_back_Handle);
				}
			}
			else if(strcmp(cmd_receive, "LED RUN1") == 0)//流水灯开
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
			else if(strcmp(cmd_receive, "LED RUN2") == 0)//流水灯关
			{
				if(task_led_run_Handle!=NULL)
				{
					status[0] = 0;
					drv_led_close(LED_ID_1);
					status[1] = 0;
					drv_led_close(LED_ID_2);
					status[2] = 0;
					drv_led_close(LED_ID_3);
					vTaskSuspend(task_led_run_Handle);
				}
			}
			else if(strcmp(cmd_receive, "GET TEM") == 0)//读取温度
			{
				//如果没有在读取温度，则挂起其他在用sht20的任务
				if(status[7] == 2)
				{
					vTaskSuspend(task_get_hum__Handle);
				}
				if(status[7] == 3)
				{
					vTaskSuspend(task_get_tem_hum__Handle);
				}
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_get_tem__Handle==NULL)
				{
					status[7] = 1;
					xTaskCreate(task_get_tem, "task_get_tem", TASK_GET_TEM_DEPTH, NULL, TASK_GET_TEM_PRIOPITY, &task_get_tem__Handle);
				}
				else
				{
					status[7] = 1;
					vTaskResume(task_get_tem__Handle);
				}
			}
			else if(strcmp(cmd_receive, "GET HUM") == 0)//读取湿度
			{
				//如果没有在读取湿度，则挂起其他在用sht20的任务
				if(status[7] == 1)
				{
					vTaskSuspend(task_get_tem__Handle);
				}
				if(status[7] == 3)
				{
					vTaskSuspend(task_get_tem_hum__Handle);
				}
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_get_hum__Handle==NULL)
				{
					status[7] = 2;
					xTaskCreate(task_get_hum, "task_get_hum", TASK_GET_HUM_DEPTH, NULL, TASK_GET_HUM_PRIOPITY, &task_get_hum__Handle);
				}
				else
				{
					status[7] = 2;
					vTaskResume(task_get_hum__Handle);
				}
			}
			else if(strcmp(cmd_receive, "GET TEM_HUM") == 0)//读取温湿度
			{
				//如果没有在读取温湿度，则挂起其他在用sht20的任务
				if(status[7] == 1)
				{
					vTaskSuspend(task_get_tem__Handle);
				}
				if(status[7] == 2)
				{
					vTaskSuspend(task_get_hum__Handle);
				}
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_get_tem_hum__Handle==NULL)
				{
					status[7] = 3;
					xTaskCreate(task_get_tem_hum, "task_get_tem_hum", TASK_GET_TEM_HUM_DEPTH, NULL, TASK_GET_TEM_HUM_PRIOPITY, &task_get_tem_hum__Handle);
				}
				else
				{
					status[7] = 3;
					vTaskResume(task_get_tem_hum__Handle);
				}
			}
			else if(strcmp(cmd_receive, "SERVO SIT") == 0)//动作控制，坐下
			{	
				//如果没有坐下,则挂起其他舵机任务
				if(status[6] == 3)
				{
					vTaskSuspend(task_advance_Handle);
				}
				if(status[6] == 4)
				{
					vTaskSuspend(task_back_Handle);
				}
				drv_servo_sit_down();
			}
			else if(strcmp(cmd_receive, "SERVO STAND") == 0)//动作控制，站立
			{	
				//如果没有站立,则挂起其他舵机任务
				if(status[6] == 3)
				{
					vTaskSuspend(task_advance_Handle);
				}
				if(status[6] == 4)
				{
					vTaskSuspend(task_back_Handle);
				}
				drv_servo_stand();
			}
			else if(strcmp(cmd_receive, "SERVO DOWM") == 0)//动作控制，趴下
			{
				//如果没有趴下,则挂起其他舵机任务
				if(status[6] == 3)
				{
					vTaskSuspend(task_advance_Handle);
				}
				if(status[6] == 4)
				{
					vTaskSuspend(task_back_Handle);
				}
				drv_servo_down();
			}
			else if(strcmp(cmd_receive, "LED1 ON") == 0)//led1，开启
			{
				if(task_led_run_Handle !=NULL)
				{
					vTaskSuspend(task_led_run_Handle);
				}
				drv_led_open(LED_ID_1);
				status[0] = 1;
			}
			else if(strcmp(cmd_receive, "LED1 OFF") == 0)//led1，关闭
			{
				drv_led_close(LED_ID_1);
				status[0] = 0;
			}
			else if(strcmp(cmd_receive, "LED2 ON") == 0)//led2，开启
			{
				if(task_led_run_Handle !=NULL)
				{
					vTaskSuspend(task_led_run_Handle);
				}
				drv_led_open(LED_ID_2);
				status[1] = 1;
			}
			else if(strcmp(cmd_receive, "LED2 OFF") == 0)//led2，关闭
			{
				drv_led_close(LED_ID_2);
				status[1] = 0;
			}
			else if(strcmp(cmd_receive, "LED3 ON") == 0)//led3，开启
			{
				if(task_led_run_Handle !=NULL)
				{
					vTaskSuspend(task_led_run_Handle);
				}
				drv_led_open(LED_ID_3);
				status[2] = 1;
			}
			else if(strcmp(cmd_receive, "LED3 OFF") == 0)//led3，关闭
			{
				drv_led_close(LED_ID_3);
				status[2] = 0;
			}
			else if(strcmp(cmd_receive, "LEDD2 ON") == 0)//ledD2，开启
			{
				drv_led_open(LED_ID_D2);
				status[3] = 1;
			}
			else if(strcmp(cmd_receive, "LEDD2 OFF") == 0)//ledD2，关闭
			{
				drv_led_close(LED_ID_D2);
				status[3] = 0;
			}
			else if(strcmp(cmd_receive, "BEEP ON") == 0)//蜂鸣器，开启
			{
				drv_beep_open();
				status[4] = 1;
			}
			else if(strcmp(cmd_receive, "BEEP OFF") == 0)//蜂鸣器，关闭
			{
				drv_beep_close();
				status[4] = 0;
			}
			else if(strcmp(cmd_receive, "FAN ON") == 0)//风扇,开启
			{
				drv_fan_open();
				status[5] = 1;
			}
			else if(strcmp(cmd_receive, "FAN OFF") == 0)//风扇,关闭
			{
				drv_fan_close();
				status[5] = 0;
			}
			else
			{
				printf("CMD ERROR!\r\n");
				for(i = 0;i<10;i++)
				{
					drv_led_open(LED_ID_D2);
					vTaskDelay(pdMS_TO_TICKS(200));
					drv_led_close(LED_ID_D2);
					vTaskDelay(pdMS_TO_TICKS(200));
				}
			}
			vTaskDelay(pdMS_TO_TICKS(10));
		}
	}
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//中断组
	//初始化
	drv_usart2_init();//串口2，用于接收语音模块数据
	drv_ustart6_init();//串口6，用于输出到电脑
	drv_led_init();
	drv_beep_init();//蜂鸣器，底板
	drv_servo_init();//舵机
	drv_fan_init();//风扇
	drv_time2_init();//定时器
	drv_sht20_init();
	drv_74hc595_init();//数码管
	OLED_Init();
	OLED_Clear();
	drv_oled_start();

	xUartSemaphore = xSemaphoreCreateBinary();

	xTaskCreate(task_cmd, "task_cmd", TASK_CMD_DEPTH, NULL, TASK_CMD_PRIOPITY, &task_cmd_Handle);
	vTaskStartScheduler();
	
}

