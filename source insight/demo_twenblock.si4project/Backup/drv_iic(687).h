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
//应答
#define ACK 1
#define NACK 0
#endif //__DRV_IIC_H__

