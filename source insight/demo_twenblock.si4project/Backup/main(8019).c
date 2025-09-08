#include "drv_led.h"
#include "drv_usart.h"
#include <stdio.h>

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断组2
	drv_led_init();
	drv_usart2_init();
	printf("hello!\r\n");
    printf("hello 2507 class\r\n");
	while(1)
	{
        
	}
}
