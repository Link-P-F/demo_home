#include "FreeRTOS.h"
#include "task.h"
#include "drv_usart.h"
#include "event_groups.h"
#include "semphr.h"
void drv_usart2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	//时钟
	RCC_AHB1PeriphClockCmd(USART2_GPIO_CLOCK, ENABLE);
	RCC_APB1PeriphClockCmd(USART2_CLOCK, ENABLE);

	//初始化配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	//GPIO初始化
	GPIO_InitStruct.GPIO_Pin = USART2_RX | USART2_TX;
	GPIO_Init(USART2_GPIO_PORT, &GPIO_InitStruct);

	//复用功能绑定
	GPIO_PinAFConfig(USART2_GPIO_PORT, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(USART2_GPIO_PORT, GPIO_PinSource3, GPIO_AF_USART2);

	//开启接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	//中断配置
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	//串口配置
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	//USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_Init(USART2, &USART_InitStruct);
	USART_Cmd(USART2, ENABLE);
}

void drv_ustart6_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	//NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHB1PeriphClockCmd(USART6_GPIO_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(USART6_CLOCK, ENABLE);

	//GPIO配置及初始化
	GPIO_InitStruct.GPIO_Pin = USART_GPIO_PIN_RXD | USART_GPIO_PIN_TXD;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART6_GPIO_PORT, &GPIO_InitStruct);

	GPIO_PinAFConfig(USART6_GPIO_PORT, GPIO_PinSource9, GPIO_AF_USART6);
	GPIO_PinAFConfig(USART6_GPIO_PORT, GPIO_PinSource14, GPIO_AF_USART6);

	/*USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启接收中断
	
	NVIC_InitStruct.NVIC_IRQChannel = USART6_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);*/

	//串口配置及初始化
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_ClearFlag(USART6, USART_FLAG_TC);
	USART_Init(USART6, &USART_InitStruct);
	USART_Cmd(USART6, ENABLE);
}

int fputc(int ch, FILE *f)
{
	USART_SendData(USART6, ch);
	while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
	USART_ClearFlag(USART6, USART_FLAG_TC);
	return ch;
}



extern char cmd_write[50];
extern char cmd_ready;
extern uint8_t cmd_index;
static uint16_t data;
extern SemaphoreHandle_t xUartSemaphore;

void USART2_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint32_t ulReturn;
	if(cmd_ready == 0 && USART_GetITStatus(USART2, USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		
		data = USART_ReceiveData(USART2);

		//数据回显
		USART_SendData(USART6, data);
		while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);

		ulReturn = taskENTER_CRITICAL_FROM_ISR();
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
			printf("cmd_write:%s\r\n",cmd_write);
			// 释放信号量，通知task_start处理
        	xSemaphoreGiveFromISR(xUartSemaphore, &xHigherPriorityTaskWoken);
		}
		taskEXIT_CRITICAL_FROM_ISR(ulReturn);
	}
}

