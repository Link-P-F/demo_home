#include "drv_beep.h"
/**********************************
函数名称: drv_beep_init
函数作用: 初始化底板有源蜂鸣器
输入参数: 无
输出参数: 无
其    他: 
**********************************/
void drv_beep_init(void)
{
    /*
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//����GPIOAʱ��
    
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;//ѡ������
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//ѡ�����
    GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//ѡ���������
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//����ģʽ
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOA, &GPIO_InitStruct);//���γ�ʼ��
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);//Ĭ�ϲ���
    */

    RCC->AHB1ENR |= 0x01;//�üĴ�����ʱ��
    GPIOA->MODER &= ~((uint32_t)0x03<<30);//����
    GPIOA->MODER |= (0x01<<30);//ѡ�����ţ�ѡ��Ϊ���ģʽ
    //Ĭ��Ϊ����ģʽ����������������Ϊ���Ǻ���ģʽѡ��Ĭ�ϼ���
    GPIOA->ODR |= (0x00<<15);//ָ�����Ÿ�ֵ��Ĭ�Ϲر�
}

/**********************************
函数名称: drv_beep_open
函数作用: 打开蜂鸣器
输入参数: 无
输出参数: 无
其    他: 
**********************************/

void drv_beep_open(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_15);//��
    //GPIOA->ODR |= (0x01<<15);
}

/**********************************
函数名称: drv_beep_close
函数作用: 关闭蜂鸣器
输入参数: 无
输出参数: 无
其    他: 
**********************************/

void drv_beep_close(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);//����
   // GPIOA->ODR &= ~(0x01<<15);
}

