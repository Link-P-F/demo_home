#include "drv_iic.h"
#include "FreeRTOS.h"
#include "task.h"
#include "drv_time.h"
#include <stdio.h>
/***************************************************
函数名称：drv_iic_init
函数功能：iic通信初始化
输入参数：无
输出参数：无
其他：		//PB6->SCL  PB7->SDA
***************************************************/
void drv_iic_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(GPIO_SCL, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = IIC_SDA | IIC_SCL;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_PORT, &GPIO_InitStruct);

	SCL_H;
	SDA_H;
}

/***************************************************
函数名称：drv_iic_in
函数功能：设置数据线为输入模式
输入参数：无
输出参数：无
其他：		
***************************************************/
void drv_iic_in(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = IIC_SDA;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(IIC_PORT, &GPIO_InitStruct);
}

/***************************************************
函数名称：drv_iic_out
函数功能：设置数据线为输出模式
输入参数：无
输出参数：无
其他：		
***************************************************/
void drv_iic_out(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStruct.GPIO_Pin = IIC_SDA;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_PORT, &GPIO_InitStruct);
}

/***************************************************
函数名称：drv_iic_start
函数功能：iic通信起始信号
输入参数：无
输出参数：无
其他：		需要有效下降沿
***************************************************/
void drv_iic_start(void)
{
	taskENTER_CRITICAL();
	SCL_L;	//数据无效
	drv_iic_out();

	SDA_H;
	SCL_H;	//数据有效
	drv_time2_us(5);
	SDA_L;	//有效下降沿
	drv_time2_us(5);
	SCL_L;
	taskEXIT_CRITICAL();
}

/***************************************************
函数名称：drv_iic_stop
函数功能：iic通信停止信号
输入参数：无
输出参数：无
其他：		需要有效上升沿
***************************************************/
void drv_iic_stop(void)
{
	taskENTER_CRITICAL();
	SCL_L;
	drv_iic_out();

	SDA_L;
	SCL_H;	//数据有效
	drv_time2_us(5);
	SDA_H;	//有效上升沿
	drv_time2_us(5);
	SCL_L;
	taskEXIT_CRITICAL();
}

/***************************************************
函数名称：drv_iic_sendACK
函数功能：iic通信发送继续通信信号
输入参数：无
输出参数：无
其他：		有效低电平
***************************************************/
uint8_t drv_iic_sendACK(void)
{
	taskENTER_CRITICAL();
	SCL_L;
	drv_iic_out();

	SDA_L;
	SCL_H;	//有效低电平
	drv_time2_us(5);
	SCL_L;
	taskEXIT_CRITICAL();
	return ACK;
}

/***************************************************
函数名称：drv_iic_sendNACK
函数功能：iic通信发送停止通信信号
输入参数：无
输出参数：无
其他：		有效高电平
***************************************************/
uint8_t drv_iic_sendNACK(void)
{
	taskENTER_CRITICAL();
	SCL_L;
	drv_iic_out();

	SDA_H;
	SCL_H;	//有效高电平
	drv_time2_us(5);
	SCL_L;
	taskEXIT_CRITICAL();
	return NACK;
}

/***************************************************
函数名称：drv_iic_waitACK
函数功能：iic通信等待通信信号
输入参数：无
输出参数：无
其他：		
***************************************************/
uint8_t drv_iic_waitACK(void)
{
	uint16_t time_out = 0;
	taskENTER_CRITICAL();
	SCL_L;
	drv_iic_in();

	SCL_H;
	while(SDA_READ)
	{
		time_out++;
		if(time_out > 500)
		{
			drv_iic_stop();
			printf("I2C ACK timeout\r\n");
			return NACK;
		}
	}
	SCL_L;
	taskEXIT_CRITICAL();
	return ACK;
}

/***************************************************
函数名称：drv_iic_senddata
函数功能：iic通信发送数据
输入参数：无
输出参数：无
其他：		
***************************************************/
void drv_iic_senddata(uint8_t data)
{
	uint8_t i=0;
	taskENTER_CRITICAL();
	SCL_L;
	drv_iic_out();

	for(i=0;i<8;i++)
	{
		if(data&0x80)
		{
			SDA_H;
		}
		else
		{
			SDA_L;
		}
		SCL_H;
		drv_time2_us(1);
		SCL_L;
		drv_time2_us(1);
		data = data<<1;
	}
	taskEXIT_CRITICAL();
}

/***************************************************
函数名称：drv_iic_readdata
函数功能：iic通信接收数据
输入参数：无
输出参数：无
其他：		把时钟线拉高，获取数据线上的数据，接收完数据以后必须要把时钟线拉低，允许从机发送下一个位的数据
***************************************************/
uint8_t drv_iic_readdata(uint8_t flag_ack)
{
	uint8_t read_data = 0;
	uint8_t i=0;
	taskENTER_CRITICAL();
	SCL_L;
	drv_iic_in();

	for(i=0;i<8;i++)
	{
		read_data = read_data<<1;
		SCL_H;
		if(SDA_READ)
		{
			read_data |=0x01;
		}
		drv_time2_us(1);
		SCL_L;
		drv_time2_us(1);
	}
	printf("reade_data:%x\r\n",read_data);
	if(flag_ack==ACK)
	{
		drv_iic_sendACK();
	}
	else
	{
		drv_iic_sendNACK();
	}
	SCL_L;
	taskEXIT_CRITICAL();
	return read_data;
}


