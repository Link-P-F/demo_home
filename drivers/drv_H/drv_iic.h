#ifndef __DRV_IIC_H__
#define __DRV_IIC_H__
#include <stm32f4xx.h>
//IIC端口
#define IIC_PORT GPIOB
//引脚
#define IIC_SDA GPIO_Pin_7
#define IIC_SCL GPIO_Pin_6
//GPIO时钟
#define GPIO_SCL RCC_AHB1Periph_GPIOB
//时钟线高低控制
#define SCL_H GPIO_SetBits(IIC_PORT, IIC_SCL)
#define SCL_L GPIO_ResetBits(IIC_PORT, IIC_SCL)
//数据线高低控制
#define SDA_H GPIO_SetBits(IIC_PORT, IIC_SDA)
#define SDA_L GPIO_ResetBits(IIC_PORT, IIC_SDA)
//数据读取
#define SDA_READ GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)

//应答
#define ACK 0
#define NACK 1

void drv_iic_init(void);
void drv_iic_in(void);
void drv_iic_out(void);
void drv_iic_start(void);
void drv_iic_stop(void);
uint8_t drv_iic_sendACK(void);
uint8_t drv_iic_sendNACK(void);
uint8_t drv_iic_waitACK(void);
void drv_iic_senddata(uint8_t data);
uint8_t drv_iic_readdata(uint8_t flag_ack);

#endif //__DRV_IIC_H__

