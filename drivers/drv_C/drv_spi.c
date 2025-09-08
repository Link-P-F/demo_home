#include "drv_spi.h"
//NSS->PB12
//SCK->PB13
//MISO->PB14
//MOSI->PB15
/***************************************************
函数名称：drv_spi2_init
函数功能：对spi2进行初始化
输入参数：无
输出参数：无
其他说明：		NSS->PB12	SCK->PB13
			MISO->PB14	MOSI->PB15
***************************************************/

void drv_spi2_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;//片选线
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//spi时钟分频
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;//第一个边沿采样
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;//时钟线空闲时位低
	SPI_InitStruct.SPI_CRCPolynomial = 7;//crc校验
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;//数据位
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//两线全双工
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;//
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;//主机模式
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;//片选由软件控制
	SPI_Init(SPI2, &SPI_InitStruct);
	SPI_Cmd(SPI2, ENABLE);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12);//拉高片选
}

/***************************************************
函数名称：drv_spi2_senddata
函数功能：发送数据
输入参数：无
输出参数：无
其他说明：
***************************************************/
void drv_spi2_senddata(uint8_t data)
{
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == 0);//等上次数据发送完成
	SPI_I2S_SendData(SPI2, data);
	
}


