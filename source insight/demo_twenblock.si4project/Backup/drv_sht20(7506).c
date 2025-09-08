#include "drv_sht20.h"
#include "drv_iic.h"
sht20_val tem_hum_data;
void drv_sht20_init(void)
{
	drv_iic_init();
}

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

uint16_t drv_sht20_read_tem(void)
{
	uint8_t data[2];
	uint16_t ret;
	drv_iic_start());
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
	vTaskDelay(pdMS_TO_TICKS(85));//延时以读取数据
	taskENTER_CRITICAL();
	data[0] = drv_iic_readdata(ACK);
	data[1] = drv_iic_readdata(NACK);
	drv_iic_stop();
	taskEXIT_CRITICAL();
	ret = ((data[0]<<8) | data[1]);
	return ret;
}

double drv_sht20_gettem(void)
{
	uint16_t temp = 0;
	temp = drv_sht20_read_tem();
	tem_hum_data.tem = -46.85 + 175.72 * ((double)temp/65536);
	printf("Temprature = %.2f\r\n",tem_hum_data.tem);
	return tem_hum_data.tem;
}

uint16_t drv_sht20_read_hum(void)
{
	uint16_t ret;
	uint8_t data[2];
	drv_iic_start();
	drv_iic_senddata(0x80);
	if(drv_iic_waitACK() != ACK)
	{
		drv_iic_stop();
		return NACK;
	}

	drv_iic_senddata(0xe5);
	if(drv_iic_waitACK() != ACK)
	{
		drv_iic_stop();
		return NACK;
	}
	vTaskDelay(pdMS_TO_TICKS(29));
	taskENTER_CRITICAL();
	data[0] = drv_iic_readdata(ACK);
	data[1] = drv_iic_readdata(NACK);
	drv_iic_stop();
	ret = ((data[0]<<8) | data[1]);
	return ret;
}

double drv_sht20_gethum(void)
{
	uint16_t hum;
	hum = drv_sht20_gethum();
	printf("Relative Humidity = %.2f\r\n",tem_hum_data.hum);
	return tem_hum_data.hum;
}
