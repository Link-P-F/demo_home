#include "drv_usart.h"
void drv_usart2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//NVIC_InitTypeDef NVIC_InitStruct;
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

	/*//开启接收中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	//中断配置
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);*/

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

/*int fputc(int ch, FILE *f)
{
	USART_SendData(USART2, ch);
	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	USART_ClearFlag(USART2, USART_FLAG_TC);
	return ch;
}*/

/*
static char str[50];
static uint8_t str_index = 0;
static uint16_t data;
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		data = USART_ReceiveData(USART2);
		USART_SendData(USART2, data);
		if(data != '\r'&& data != '\n')
		{
			str[str_index] = data;
			str_index++;
		}
		else
		{
			str[str_index] = '\0';
			if(strcmp(str,"LED OPEN") == 0)
			{
				drv_led_open(LED_ID_D2);
			}
			else if(strcmp(str,"LED CLOSE") == 0)
			{
				drv_led_close(LED_ID_D2);
			}
			str_index = 0;
		}
	}
}
*/
