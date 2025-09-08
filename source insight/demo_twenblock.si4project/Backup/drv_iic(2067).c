#include "drv_iic.h"
//PB6->SCL  PB7->SDA
void drv_iic_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(GPIO_SCL, ENABLE);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT
	GPIO_Init(IIC_PORT, GPIO_InitTypeDef * GPIO_InitStruct)
}

void drv_iic_in(void)
{

}

void drv_iic_out(void)
{

}

void drv_iic_start(void)
{

}

void drv_iic_stop(void)
{

}


uint8_t drv_iic_sendACK(void)
{

}

uint8_t drv_iic_sendNACK(void)
{

}

void drv_iic_waitACK(void)
{

}

void drv_iic_senddata(uint32_t data)
{

}

uint32_t drv_iic_readdata(void)
{

}


