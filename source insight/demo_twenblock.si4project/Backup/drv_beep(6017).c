#include "drv_beep.h"
/*******************
函数名：drv_beep_init
函数作用：初始化蜂鸣器
输入参数：无
输出参数：无
其他：给1蜂鸣器响
*******************/
void drv_beep_init(void)
{
    /*
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//开启GPIOA时钟
    
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;//选定引脚
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//选择输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//选定输出速率
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽模式
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOA, &GPIO_InitStruct);//传参初始化
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);//默认不响
    */

    RCC->AHB1ENR |= 0x01;//用寄存器打开时钟
    GPIOA->MODER &= ~((uint32_t)0x03<<30);//清零
    GPIOA->MODER |= (0x01<<30);//选择引脚，选择为输出模式
    //默认为推挽模式和无上啦下拉，因为不是合作模式选择默认即可
    GPIOA->ODR |= (0x00<<15);//指定引脚给值，默认关闭
}

/*******************
函数名：drv_beep_open
函数作用：打开化蜂鸣器
输入参数：无
输出参数：无
其他：给1蜂鸣器响
*******************/
void drv_beep_open(void)
{
    //GPIO_SetBits(GPIOA, GPIO_Pin_15);//响
    GPIOA->ODR |= (0x01<<15);
}

/*******************
函数名：drv_beep_close
函数作用：关闭蜂鸣器
输入参数：无
输出参数：无
其他：给1蜂鸣器不响
*******************/
void drv_beep_close(void)
{
    //GPIO_ResetBits(GPIOA, GPIO_Pin_15);//不响
    GPIOA->ODR &= ~(0x01<<15);
}
