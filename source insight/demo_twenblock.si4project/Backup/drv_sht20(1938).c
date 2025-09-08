#include "drv_sht20.h"
#include "drv_iic.h"
#include "FreeRTOS.h"
#include "task.h"
#include "drv_time.h"
#include <stdio.h>
sht20_val tem_hum_data;
/***************************************************
函数名称：drv_sht20_init
函数功能：对温湿度传感器进行初始化
输入参数：无
输出参数：无
其他说明：		
***************************************************/

void drv_sht20_init(void)
{
	drv_iic_init();
}

/***************************************************
函数名称：drv_sht20_reset
函数功能：对温湿度传感器进行复位
输入参数：无
输出参数：无
其他说明：		
***************************************************/
uint8_t drv_sht20_reset(void)
{
	drv_iic_start();
	drv_iic_senddata(0x80);//发送设备码及开启写入模式
	if(drv_iic_waitACK() != ACK)
	{
		drv_iic_stop();
		return NACK;
	}
	drv_iic_senddata(0xfe);//发送复位命令
	if(drv_iic_waitACK() != ACK)
	{
		drv_iic_stop();
		return NACK;
	}
	drv_iic_stop();
	return ACK;
}

/***************************************************
函数名称：drv_sht20_read_tem
函数功能：读取温湿度传感器中的温度数据
输入参数：无
输出参数：未经处理的温度数据
其他说明：		
***************************************************/
uint16_t drv_sht20_read_tem(void)
{
	uint8_t data[2];
	uint16_t ret;

	drv_iic_start();
	drv_iic_senddata(0x80);
	if(drv_iic_waitACK() != ACK)
	{
		drv_iic_stop();
		return NACK;
	}
	drv_iic_senddata(0xe3);//读取温度
	if(drv_iic_waitACK() != ACK)
	{
		drv_iic_stop();
		return NACK;
	}
	drv_iic_stop();

	drv_iic_start();
	drv_iic_senddata(0x81);
    if(drv_iic_waitACK() != ACK)      // 3. 等待读地址ACK
    {
        drv_iic_stop();
        return NACK;
    }
	//vTaskDelay(pdMS_TO_TICKS(85));//延时以读取数据
	vTaskDelay(pdMS_TO_TICKS(85));

	taskENTER_CRITICAL();
	data[0] = drv_iic_readdata(ACK);
	data[1] = drv_iic_readdata(NACK);
	drv_iic_stop();
	taskEXIT_CRITICAL();
	ret = ((data[0]<<8) | data[1]);
	return ret;
}

/***************************************************
函数名称：drv_sht20_gettem
函数功能：对温湿度传感器中的温度数据进行处理
输入参数：无
输出参数：已处理的温度数据
其他说明：		
***************************************************/

double drv_sht20_gettem(void)
{
	uint16_t temp = 0;
	temp = drv_sht20_read_tem();
	tem_hum_data.tem = -46.85 + 175.72 * ((double)temp/65536);
	printf("Temprature = %.2f\r\n",tem_hum_data.tem);
	return tem_hum_data.tem;
}

/***************************************************
函数名称：drv_sht20_read_hum
函数功能：读取温湿度传感器中的湿度数据
输入参数：无
输出参数：未经处理的湿度数据
其他说明：		
***************************************************/
uint16_t drv_sht20_read_hum(void)
{
	uint8_t data[2];
	uint16_t ret;

	drv_iic_start();
	drv_iic_senddata(0x80);
	if(drv_iic_waitACK() != ACK)
	{
		drv_iic_stop();
		return NACK;
	}
	drv_iic_senddata(0xe5);//读取湿度
	if(drv_iic_waitACK() != ACK)
	{
		drv_iic_stop();
		return NACK;
	}
	drv_iic_stop();

	drv_iic_start();
	drv_iic_senddata(0x81);
    if(drv_iic_waitACK() != ACK)      // 3. 等待读地址ACK
    {
        drv_iic_stop();
        return NACK;
    }
	//vTaskDelay(pdMS_TO_TICKS(85));//延时以读取数据
	vTaskDelay(pdMS_TO_TICKS(29));

	taskENTER_CRITICAL();
	data[0] = drv_iic_readdata(ACK);
	data[1] = drv_iic_readdata(NACK);
	drv_iic_stop();
	taskEXIT_CRITICAL();
	ret = ((data[0]<<8) | data[1]);
	return ret;
}

/***************************************************
函数名称：drv_sht20_gethum
函数功能：对温湿度传感器中的湿度数据进行处理
输入参数：无
输出参数：未经处理的温度数据
其他说明：		
***************************************************/
double drv_sht20_gethum(void)
{
	uint16_t hum;
	hum = drv_sht20_read_hum();
	tem_hum_data.hum = -6 + 125 * ((double)hum/65536);
	printf("Relative Humidity = %.2f\r\n",tem_hum_data.hum);
	return tem_hum_data.hum;
}

