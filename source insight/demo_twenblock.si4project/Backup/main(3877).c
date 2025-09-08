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
#include "bmp.h"

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
	if(task_get_hum__Handle)
	{
		vTaskSuspend(task_get_hum__Handle);
	}
	drv_sht20_reset();
	vTaskDelay(pdMS_TO_TICKS(15));
	for(;;)
	{
		drv_sht20_gettem();
		printf("TEMP = %.2f\r\n",tem_hum_data.tem);
		drv_seg_display_double(tem_hum_data.tem);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void task_get_hum(void * pvParameter)
{
	printf("GET HUM...\r\n");
	if(task_get_tem__Handle)
	{
		vTaskSuspend(task_get_tem__Handle);
	}
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
	if(task_get_tem__Handle)
	{
		vTaskSuspend(task_get_tem__Handle);
	}
	if(task_get_hum__Handle)
	{
		vTaskSuspend(task_get_hum__Handle);
	}
	drv_sht20_reset();
	vTaskDelay(pdMS_TO_TICKS(15));
	for(;;)
	{
		drv_sht20_gettem();
		drv_sht20_gethum();
		printf("TEMP = %.2f\r\n",tem_hum_data.tem);
		printf("HUM = %.2f\r\n",tem_hum_data.hum);
		//OLED
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
				if(task_back_Handle!=NULL)
				{
					vTaskSuspend(task_back_Handle);
				}
				//前进任务存在，解挂任务，任务不存在，创建任务
				if(task_advance_Handle==NULL)
				{
					xTaskCreate(task_advance, "task_advance", TASK_ADVANCE_DEPTH, NULL, TASK_ADVANCE_PRIOPITY, &task_advance_Handle);
				}
				else
				{
					vTaskResume(task_advance_Handle);
				}
			}
			else if(strcmp(cmd_receive, "SERVO BACK") == 0)//舵机后退
			{
				//若舵机正在前进，挂起舵机前进任务
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
			else if(strcmp(cmd_receive, "LED RUN2") == 0)
			{
				if(task_led_run_Handle!=NULL)
				{
					vTaskSuspend(task_led_run_Handle);
				}
			}
			else if(strcmp(cmd_receive, "GET TEM") == 0)//读取温度
			{
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_led_run_Handle==NULL)
				{
					xTaskCreate(task_get_tem, "task_get_tem", TASK_GET_TEM_DEPTH, NULL, TASK_GET_TEM_PRIOPITY, &task_get_tem__Handle);
				}
				else
				{
					vTaskResume(task_get_tem__Handle);
				}
			}
			else if(strcmp(cmd_receive, "GET HUM") == 0)//读取湿度
			{
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_led_run_Handle==NULL)
				{
					xTaskCreate(task_get_hum, "task_get_hum", TASK_GET_HUM_DEPTH, NULL, TASK_GET_HUM_PRIOPITY, &task_get_hum__Handle);
				}
				else
				{
					vTaskResume(task_get_hum__Handle);
				}
			}
			else if(strcmp(cmd_receive, "GET TEM_HUM") == 0)//读取温湿度
			{
				//任务存在，解挂任务，任务不存在，创建任务
				if(task_led_run_Handle==NULL)
				{
					xTaskCreate(task_get_tem_hum, "task_get_tem_hum", TASK_GET_TEM_HUM_DEPTH, NULL, TASK_GET_TEM_HUM_PRIOPITY, &task_get_tem_hum__Handle);
				}
				else
				{
					vTaskResume(task_get_tem_hum__Handle);
				}
			}
			else if(strcmp(cmd_receive, "SERVO SIT") == 0)//动作控制，坐下
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
			else if(strcmp(cmd_receive, "SERVO STAND") == 0)//动作控制，站立
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
			else if(strcmp(cmd_receive, "SERVO DOWN") == 0)//动作控制，趴下
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
			else if(strcmp(cmd_receive, "LED1 OPEN") == 0)//led1，开启
			{
				if(task_led_run_Handle !=NULL)
				{
					vTaskSuspend(task_led_run_Handle);
				}
				drv_led_open(LED_ID_1);
			}
			else if(strcmp(cmd_receive, "LED1 CLOSE") == 0)//led1，关闭
			{
				drv_led_close(LED_ID_1);
			}
			else if(strcmp(cmd_receive, "LED2 OPEN") == 0)//led2，开启
			{
				if(task_led_run_Handle !=NULL)
				{
					vTaskSuspend(task_led_run_Handle);
				}
				drv_led_open(LED_ID_2);
			}
			else if(strcmp(cmd_receive, "LED2 CLOSE") == 0)//led2，关闭
			{
				drv_led_close(LED_ID_2);
			}
			else if(strcmp(cmd_receive, "LED3 OPEN") == 0)//led3，开启
			{
				if(task_led_run_Handle !=NULL)
				{
					vTaskSuspend(task_led_run_Handle);
				}
				drv_led_open(LED_ID_3);
			}
			else if(strcmp(cmd_receive, "LED3 CLOSE") == 0)//led3，关闭
			{
				drv_led_close(LED_ID_3);
			}
			else if(strcmp(cmd_receive, "LEDD2 OPEN") == 0)//ledD2，开启
			{
				drv_led_open(LED_ID_D2);
			}
			else if(strcmp(cmd_receive, "LEDD2 CLOSE") == 0)//ledD2，关闭
			{
				drv_led_close(LED_ID_D2);
			}
			else if(strcmp(cmd_receive, "BEEP OPEN") == 0)//蜂鸣器，开启
			{
				drv_beep_open();
			}
			else if(strcmp(cmd_receive, "BEEP CLOSE") == 0)//蜂鸣器，关闭
			{
				drv_beep_close();
			}
			else if(strcmp(cmd_receive, "FAN OPEN") == 0)//风扇,开启
			{
				drv_fan_open();
			}
			else if(strcmp(cmd_receive, "FAN CLOSE") == 0)//风扇,关闭
			{
				drv_fan_close();
			}
			else
			{
				printf("CMD ERROR!\r\n");
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

	while(1) 
	{
		OLED_ShowPicture(0,0,128,64,BMP1,1);
		OLED_Refresh();
		drv_systick_ms(500);
		OLED_Clear();
		OLED_ShowChinese(0,0,0,16,1);//中
		OLED_ShowChinese(18,0,1,16,1);//景
		OLED_ShowChinese(36,0,2,16,1);//园
		OLED_ShowChinese(54,0,3,16,1);//电
		OLED_ShowChinese(72,0,4,16,1);//子
		OLED_ShowChinese(90,0,5,16,1);//技
		OLED_ShowChinese(108,0,6,16,1);//术
		OLED_ShowString(8,16,"ZHONGJINGYUAN",16,1);
		OLED_ShowString(20,32,"2014/05/01",16,1);
		OLED_ShowString(0,48,"ASCII:",16,1);  
		OLED_ShowString(63,48,"CODE:",16,1);
		OLED_ShowChar(48,48,t,16,1);//显示ASCII字符	   
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(103,48,t,3,16,1);
		OLED_Refresh();
		delay_ms(500);
		OLED_Clear();
		OLED_ShowChinese(0,0,0,16,1);  //16*16 中
	  OLED_ShowChinese(16,0,0,24,1); //24*24 中
		OLED_ShowChinese(24,20,0,32,1);//32*32 中
	  OLED_ShowChinese(64,0,0,64,1); //64*64 中
		OLED_Refresh();
	  delay_ms(500);
  	OLED_Clear();
		OLED_ShowString(0,0,"ABC",8,1);//6*8 “ABC”
		OLED_ShowString(0,8,"ABC",12,1);//6*12 “ABC”
	  OLED_ShowString(0,20,"ABC",16,1);//8*16 “ABC”
		OLED_ShowString(0,36,"ABC",24,1);//12*24 “ABC”
	  OLED_Refresh();
		delay_ms(500);
		OLED_ScrollDisplay(11,4,1);
	}
	xUartSemaphore = xSemaphoreCreateBinary();

	xTaskCreate(task_cmd, "task_cmd", TASK_CMD_DEPTH, NULL, TASK_CMD_PRIOPITY, &task_cmd_Handle);
	vTaskStartScheduler();
	
}

