#include "FreeRTOS.h"
#include "task.h"
#include "drv_usart.h"
#include "drv_led.h"
#include "semphr.h"
#include <stdio.h>
/***********************************************
函数名称：drv_usart2_init
函数作用：初始化串口2通过中断接收来自语音模块的信息
输入参数：无
输出参数：无
其    他：使用io口 TX->PA2	RX->PA3
***********************************************/
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
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x06;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
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

/***********************************************
函数名称：drv_usart3_init
函数作用：初始化串口3通过中断接收来自WIFI模块的信息
输入参数：无
输出参数：无
其    他：使用io口 TX->PB10	RX->PB11
***********************************************/
void drv_usart3_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	//时钟
	RCC_AHB1PeriphClockCmd(USART3_GPIO_CLOCK, ENABLE);
	RCC_APB1PeriphClockCmd(USART3_CLOCK, ENABLE);

	//初始化配置
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	//GPIO初始化
	GPIO_InitStruct.GPIO_Pin = USART3_RX | USART3_TX;
	GPIO_Init(USART3_GPIO_PORT, &GPIO_InitStruct);

	//复用功能绑定
	GPIO_PinAFConfig(USART3_GPIO_PORT, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(USART3_GPIO_PORT, GPIO_PinSource11, GPIO_AF_USART3);

	//开启接收中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	//中断配置
	NVIC_InitStruct.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x06;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	//串口配置
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	//USART_ClearFlag(USART3, USART_FLAG_TC);
	USART_Init(USART3, &USART_InitStruct);
	USART_Cmd(USART3, ENABLE);
}

/**************************************************
函数名称：drv_usart6_init
函数作用：串口6通过中断发送来自语音模块的信息到电脑
输入参数：无
输出参数：无
其    他：使用io口：RXD->PG9	TXD->PG14
**************************************************/
void drv_usart6_init(void)
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

/**************************************************
函数名称：fputc
函数作用：fputc函数重构，通过串口6发送数据
输入参数：ch，f
输出参数：ch
其    他：
**************************************************/
int fputc(int ch, FILE *f)
{
	USART_SendData(USART6, ch);
	while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);
	USART_ClearFlag(USART6, USART_FLAG_TC);
	return ch;
}

/**************************************************
函数名称：USART2_IRQHandler
函数作用：串口2中断，将接收的信息存入cmd_write
输入参数：无
输出参数：无
其    他：
**************************************************/
extern char cmd_write[50];
extern uint8_t cmd_index;
extern SemaphoreHandle_t xUartSemaphore;
extern TaskHandle_t task_cmd_Handle;

void USART2_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint32_t ulReturn;
	uint16_t data;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		
		data = USART_ReceiveData(USART2) & 0x00FF;

		//数据回显
		//USART_SendData(USART6, data);
		//while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);

		ulReturn = taskENTER_CRITICAL_FROM_ISR();
		if(data != '\r'&& data != '\n')
		{
			if(cmd_index < sizeof(cmd_write) - 1)  // 确保不越界
            {
                cmd_write[cmd_index] = data;
                cmd_index++;
            }
		}
		else
		{	
			if(cmd_index>0)
			{
				cmd_write[cmd_index] = '\0';
				cmd_index = 0;
				printf("cmd_write_isr:%s\r\n",cmd_write);
				// 释放信号量，通知task_start处理
        		xSemaphoreGiveFromISR(xUartSemaphore, &xHigherPriorityTaskWoken);
			}
		}
		taskEXIT_CRITICAL_FROM_ISR(ulReturn);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**************************************************
函数名称：USART3_IRQHandler
函数作用：串口3中断，将接收的信息存入cmd_write
输入参数：无
输出参数：无
其    他：
**************************************************/
uint8_t index = 0;
uint8_t parse_state = 0;
uint16_t target_len = 0; 
static uint8_t ipd_cnt = 0;  // 记录匹配"IPD,"的进度

void USART3_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	uint32_t ulReturn;
	uint16_t data;
	if(USART_GetITStatus(USART3, USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		
		data = USART_ReceiveData(USART3) & 0x00FF;

		//数据回显
		//USART_SendData(USART6, data);
		//while(USART_GetFlagStatus(USART6, USART_FLAG_TC) == RESET);

		ulReturn = taskENTER_CRITICAL_FROM_ISR();
		switch(parse_state)
				{
					// 状态0：等待"IPD,"前缀（忽略前面的无关字符）
					case 0:
						if (data == 'I' && ipd_cnt == 0) ipd_cnt = 1;
						else if (data == 'P' && ipd_cnt == 1) ipd_cnt = 2;
						else if (data == 'D' && ipd_cnt == 2) ipd_cnt = 3;
						else if (data == ',' && ipd_cnt == 3)  // 匹配到"IPD,"，进入解析长度状态
						{
							ipd_cnt = 0;
							parse_state = 1;  // 切换到“解析长度”状态
						}
						else ipd_cnt = 0;  // 匹配失败，重置进度
						break;
		
					// 状态1：解析长度（从","后提取数字，直到":"为止）
					case 1:
						if (data >= '0' && data <= '9')  // 是数字字符，累加计算长度
						{
							target_len = target_len * 10 + (data - '0');  // 字符串转数字（如"7"→7）
						}
						else if (data == ':')  // 遇到":"，长度解析完成，进入存储数据状态
						{
							// 验证长度合理性（避免异常值，如len>49导致缓冲区溢出）
							if (target_len > 0 && target_len < sizeof(cmd_write) - 1)
							{
								parse_state = 2;  // 切换到“存储数据”状态
							}
							else  // 长度无效，重置状态
							{
								target_len = 0;
								cmd_index = 0;
								parse_state = 0;
							}
						}
						else  // 遇到非数字、非":"，解析失败，重置
						{
							target_len = 0;
							cmd_index = 0;
							parse_state = 0;
						}
						break;
		
					// 状态2：按解析出的长度存储有效数据
					case 2:
						// 存储数据，直到达到target_len个字符
						if (cmd_index < target_len)
						{
							cmd_write[cmd_index] = data;
							cmd_index++;
						}
						// 数据存储完成（达到target_len），准备结束
						if (cmd_index == target_len)
						{
							cmd_write[cmd_index] = '\0';  // 添加字符串终止符
							parse_state = 3;  // 切换到“接收完成”状态
							// 释放信号量，通知任务解析命令
							xSemaphoreGiveFromISR(xUartSemaphore, &xHigherPriorityTaskWoken);
						}
						break;
		
					// 状态3：接收完成（等待下一次帧头，忽略后续字符如"\r\n"）
					case 3:
						// 遇到下一个可能的帧头（如"+"），重置状态准备下一次接收
						if (data == '+')
						{
							target_len = 0;
							cmd_index = 0;
							parse_state = 0;  // 回到初始状态，重新匹配"IPD,"
						}
						break;
				}
		taskEXIT_CRITICAL_FROM_ISR(ulReturn);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

