#include "oled.h"
#include "drv_oled_show.h"
#include <math.h>

void drv_oled_tem_hum(double tem, double hum)
{
	OLED_ShowString(0,16,"TEMP: ",16,1);
	OLED_ShowNum(50, 16, (u32)tem, 2, 16, 1);
	OLED_ShowChar(70, 16, '.', 16, 1);
	OLED_ShowNum(80, 16, (u32)((tem-floor(tem))*10), 2, 16, 1);
	OLED_ShowChar(100, 16, 'C', 16, 1);

	OLED_ShowString(0, 32, "HUM:  ", 16, 1);       // 前缀
	OLED_ShowNum(50, 32, (u32)hum, 2, 16, 1);     // 整数部分60
	OLED_ShowChar(70, 32, '.', 16, 1);            // 小数点
	OLED_ShowNum(80, 32, (u32)((hum-floor(hum))*10), 2, 16, 1);  // 小数部分2
	OLED_ShowChar(100, 32, '%', 16, 1); 

	OLED_Refresh();  // 刷新显示
}


