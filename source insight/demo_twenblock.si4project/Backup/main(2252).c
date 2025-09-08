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
fan:0关闭，1低档，2中档，3高档
servo:0站立，1坐下，2趴下，3前进，4后退
sht20:0关闭，1读取温度，2读取湿度，3读取温湿度
seg:0关闭，1显示温度，2显示湿度
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
	0,//8，seg
};

SemaphoreHandle_t xUartSemaphore = NULL;//通信量用于同步

//命令解析任务配置
#define TASK_CMD_DEPTH 128
#define TASK_CMD_PRIORITY 3
TaskHandle_t task_cmd_Handle = NULL;

//前进任务配置
#define TASK_ADVANCE_DEPTH 128
#define TASK_ADVANCE_PRIORITY 2
TaskHandle_t task_advance_Handle = NULL;

//后退任务配置
#define TASK_BACK_DEPTH 128
#define TASK_BACK_PRIORITY 2
TaskHandle_t task_back_Handle = NULL;

//流水灯任务配置
#define TASK_LED_RUN_DEPTH 128
#define TASK_LED_RUN_PRIORITY 2
TaskHandle_t task_led_run_Handle = NULL;

//读取温度任务配置
#define TASK_GET_TEM_DEPTH 128
#define TASK_GET_TEM_PRIORITY 2
TaskHandle_t task_get_tem_Handle = NULL;

//读取湿度任务配置
#define TASK_GET_HUM_DEPTH 128
#define TASK_GET_HUM_PRIORITY 2
TaskHandle_t task_get_hum_Handle = NULL;

//读取温湿度任务配置
#define TASK_GET_TEM_HUM_DEPTH 128
#define TASK_GET_TEM_HUM_PRIORITY 2
TaskHandle_t task_get_tem_hum_Handle = NULL;

//OLED任务配置
#define TASK_OLED_DEPTH 128
#define TASK_OLED_PRIORITY 4
TaskHandle_t task_oled_Handle = NULL;

//seg任务配置
#define TASK_SEG_DEPTH 128
#define TASK_SEG_PRIORITY 4
TaskHandle_t task_seg_Handle = NULL;


char cmd_write[50] = {0};
char cmd_receive[50] = {0};
uint8_t cmd_index = 0;
char parts[20] = {0};

void task_advance(void * pvParameters)
{
	printf("ADVANCE...\r\n");
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
	
	
	drv_sht20_gettem();
	printf("TEM = %.2f\r\n",tem_hum_data.tem);
	status[8] = 1;//seg,tem
	printf("status[8]:%d",status[8]);
	vTaskDelay(pdMS_TO_TICKS(1000));

	task_get_tem_Handle = NULL;
	vTaskDelete(NULL); 
}

void task_get_hum(void * pvParameter)
{
	printf("GET HUM...\r\n");
	drv_sht20_reset();
	vTaskDelay(pdMS_TO_TICKS(15));
	
	drv_sht20_gethum();
	printf("HUM = %.2f\r\n",tem_hum_data.hum);
	status[8] = 2;//seg,hum
	printf("status[8]:%d",status[8]);
	vTaskDelay(pdMS_TO_TICKS(1000));

	task_get_hum_Handle = NULL;
	vTaskDelete(NULL); 
}

void task_get_tem_hum(void * pvParameter)
{
	printf("GET TEM AND HUM...\r\n");
	drv_sht20_reset();
	vTaskDelay(pdMS_TO_TICKS(15));
	
	drv_sht20_gettem();
	drv_sht20_gethum();
	printf("TEMP = %.2f\r\n",tem_hum_data.tem);
	printf("HUM = %.2f\r\n",tem_hum_data.hum);
	status[8] = 0;
	printf("status[8]:%d",status[8]);
	vTaskDelay(pdMS_TO_TICKS(10));

	task_get_tem_hum_Handle = NULL;
	vTaskDelete(NULL); 
}

void task_oled(void * pvParameter)
{
	printf("oled...\r\n");
	drv_oled_start();
	vTaskDelay(pdMS_TO_TICKS(3000));
	for(;;)
	{
		drv_oled_status_1();
		drv_oled_status_2();
		drv_oled_status_3(tem_hum_data.tem, tem_hum_data.hum);
		drv_oled_logo();
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void task_seg(void * pvParameter)
{
	uint8_t i;
	printf("seg...\r\n");
	for(;;)
	{
		switch(status[8])
		{
			case 0://什么都不显示
				for(i = 0; i < 4; i++)
				{
					drv_seg_display(i, 0x00);
				}
				break;
			case 1://显示温度
				drv_seg_display_double(tem_hum_data.tem);
				break;
			case 2://显示湿度
				drv_seg_display_double(tem_hum_data.hum);
				break;
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}



void servo_task_cmd(void)
{
	//若舵机正在前进或后退，挂起舵机任务
	switch(status[6])
	{
		case 3:
			vTaskSuspend(task_advance_Handle);
			break;
		case 4:
			vTaskSuspend(task_back_Handle);
			break;
	}

	//命令解析
	if(strcmp(cmd_receive, "SERVO ADVANCE") == 0)//舵机前进
			{
				//前进任务存在，解挂任务，任务不存在，创建任务
				if(task_advance_Handle==NULL)
				{
					status[6] = 3;
					xTaskCreate(task_advance, "task_advance", TASK_ADVANCE_DEPTH, NULL, TASK_ADVANCE_PRIORITY, &task_advance_Handle);
				}
				else
				{
					status[6] = 3;
					vTaskResume(task_advance_Handle);
				}
			}
			else if(strcmp(cmd_receive, "SERVO BACK") == 0)//舵机后退
			{
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_back_Handle==NULL)
				{
					status[6] = 4;
					xTaskCreate(task_back, "task_back", TASK_BACK_DEPTH, NULL, TASK_BACK_PRIORITY, &task_back_Handle);
				}
				else
				{
					status[6] = 4;
					vTaskResume(task_back_Handle);
				}
			}
			else if(strcmp(cmd_receive, "SERVO SIT") == 0)//动作控制，坐下
			{	
				status[6] = 1;
				drv_servo_sit_down();
			}
			else if(strcmp(cmd_receive, "SERVO STAND") == 0)//动作控制，站立
			{	
				status[6] = 0;
				drv_servo_stand();
			}
			else if(strcmp(cmd_receive, "SERVO DOWN") == 0)//动作控制，趴下
			{
				status[6] = 2;
				drv_servo_down();
			}
}

void sht20_task_cmd(void)
{

	//命令解析
	if(strcmp(cmd_receive, "SHT20 TEM") == 0)//读取温度
	{
		//任务存在，解挂任务，任务不存在，创建任务
		if(task_get_tem_Handle==NULL)
		{
			status[7] = 1;
			xTaskCreate(task_get_tem, "task_get_tem", TASK_GET_TEM_DEPTH, NULL, TASK_GET_TEM_PRIORITY, &task_get_tem_Handle);
		}
	}
	else if(strcmp(cmd_receive, "SHT20 HUM") == 0)//读取湿度
	{
		//任务存在，解挂任务，任务不存在，创建任务
		if(task_get_hum_Handle==NULL)
		{
			status[7] = 2;
			xTaskCreate(task_get_hum, "task_get_hum", TASK_GET_HUM_DEPTH, NULL, TASK_GET_HUM_PRIORITY, &task_get_hum_Handle);
		}
	}
	else if(strcmp(cmd_receive, "SHT20 TEM_HUM") == 0)//读取温湿度
	{
		//任务存在，解挂任务，任务不存在，创建任务
		if(task_get_tem_hum_Handle==NULL)
		{
			status[7] = 3;
			xTaskCreate(task_get_tem_hum, "task_get_tem_hum", TASK_GET_TEM_HUM_DEPTH, NULL, TASK_GET_TEM_HUM_PRIORITY, &task_get_tem_hum_Handle);
		}
	}

	
}

void led_task_cmd(void)
{
	if(strcmp(cmd_receive, "LED RUN1") == 0)//流水灯开
	{
		//任务存在，解挂任务，任务不存在，创建任务
		if(task_led_run_Handle==NULL)
		{
			xTaskCreate(task_led_run, "task_led_run", TASK_LED_RUN_DEPTH, NULL, TASK_LED_RUN_PRIORITY, &task_led_run_Handle);
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
	else if(strcmp(cmd_receive, "LED 1 ON") == 0)//led1，开启
	{
		if(task_led_run_Handle !=NULL)
		{
			vTaskSuspend(task_led_run_Handle);
		}
		drv_led_open(LED_ID_1);
		status[0] = 1;
	}
	else if(strcmp(cmd_receive, "LED 1 OFF") == 0)//led1，关闭
	{
		drv_led_close(LED_ID_1);
		status[0] = 0;
	}
	else if(strcmp(cmd_receive, "LED 2 ON") == 0)//led2，开启
	{
		if(task_led_run_Handle !=NULL)
		{
			vTaskSuspend(task_led_run_Handle);
		}
		drv_led_open(LED_ID_2);
		status[1] = 1;
	}
	else if(strcmp(cmd_receive, "LED 2 OFF") == 0)//led2，关闭
	{
		drv_led_close(LED_ID_2);
		status[1] = 0;
	}
	else if(strcmp(cmd_receive, "LED 3 ON") == 0)//led3，开启
	{
		if(task_led_run_Handle !=NULL)
		{
			vTaskSuspend(task_led_run_Handle);
		}
		drv_led_open(LED_ID_3);
		status[2] = 1;
	}
	else if(strcmp(cmd_receive, "LED 3 OFF") == 0)//led3，关闭
	{
		drv_led_close(LED_ID_3);
		status[2] = 0;
	}
	else if(strcmp(cmd_receive, "LED D2 ON") == 0)//ledD2，开启
	{
		drv_led_open(LED_ID_D2);
		status[3] = 1;
	}
	else if(strcmp(cmd_receive, "LED D2 OFF") == 0)//ledD2，关闭
	{
		drv_led_close(LED_ID_D2);
		status[3] = 0;
	}
		
}

void beep_task_cmd(void)
{
	if(strcmp(cmd_receive, "BEEP ON") == 0)//蜂鸣器，开启
	{
		status[4] = 1;
		drv_beep_open();
	}
	else if(strcmp(cmd_receive, "BEEP OFF") == 0)//蜂鸣器，关闭
	{
		status[4] = 0;
		drv_beep_close();
	}
}

void fan_task_cmd(void)
{
	if(strcmp(cmd_receive, "FAN LOW") == 0)//风扇,低档
	{
		status[5] = 1;
		drv_fan_set_speed(FAN_LOW);
	}
	else if(strcmp(cmd_receive, "FAN MEDIUM") == 0)//风扇,中档
	{
		status[5] = 2;
		drv_fan_set_speed(FAN_MEDIUM);
	}
	else if(strcmp(cmd_receive, "FAN HIGH") == 0)//风扇,高档
	{
		status[5] = 3;
		drv_fan_set_speed(FAN_HIGH);
	}
	else if(strcmp(cmd_receive, "FAN OFF") == 0)//风扇,关闭
	{
		status[5] = 0;
		drv_fan_set_speed(FAN_STOP);
	}
}

void task_cmd(void * pvParameters)
{
	xTaskCreate(task_oled, "task_oled", TASK_OLED_DEPTH, NULL, TASK_OLED_PRIORITY, &task_oled_Handle);
	xTaskCreate(task_seg, "task_seg", TASK_SEG_DEPTH, NULL, TASK_SEG_PRIORITY, &task_seg_Handle);
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

			while(cmd_write[i]!='\0')
			{
				if(cmd_write[i]==' ')
				{
					break;
				}
				parts[i] = cmd_write[i];
				i++;
			}
			parts[i] = '\0';
			i = 0;
			printf("%s\r\n",parts);
			
			//数据处理
			if(strcmp(parts, "SERVO") == 0)//舵机
			{
				servo_task_cmd();
			}
			else if(strcmp(parts, "SHT20") == 0)//sht20
			{
				sht20_task_cmd();
			}
			else if(strcmp(parts, "LED") == 0)//灯
			{
				led_task_cmd();
			}
			else if(strcmp(parts, "BEEP") == 0)//蜂鸣器
			{
				beep_task_cmd();
			}
			else if(strcmp(parts, "FAN") == 0)//风扇,低档
			{
				fan_task_cmd();
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
		}
	}
}

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//中断组
	//初始化
	drv_usart2_init();//串口2，用于接收语音模块数据
	drv_usart6_init();//串口6，用于输出到电脑
	drv_led_init();
	drv_beep_init();//蜂鸣器，底板
	drv_servo_init();//舵机
	status[6] = 0;
	drv_fan_init();//风扇
	drv_time2_init();//定时器
	drv_usart3_init();//WIFI
	drv_sht20_init();
	drv_74hc595_init();//数码管
	OLED_Init();
	printf("1\r\n");

	xUartSemaphore = xSemaphoreCreateBinary();

	xTaskCreate(task_cmd, "task_cmd", TASK_CMD_DEPTH, NULL, TASK_CMD_PRIORITY, &task_cmd_Handle);
	vTaskStartScheduler();
	
}

