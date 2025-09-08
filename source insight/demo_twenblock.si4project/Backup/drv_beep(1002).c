#include "drv_beep.h"
/**********************************
å‡½æ•°åç§°: drv_beep_init
å‡½æ•°ä½œç”¨: åˆå§‹åŒ–åº•æ¿æœ‰æºèœ‚é¸£å™¨
è¾“å…¥å‚æ•°: æ— 
è¾“å‡ºå‚æ•°: æ— 
å…¶    ä»–: 
**********************************/
void drv_beep_init(void)
{
    /*
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//¿ªÆôGPIOAÊ±ÖÓ
    
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;//Ñ¡¶¨Òý½Å
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//Ñ¡ÔñÊä³ö
    GPIO_InitStruct.GPIO_Speed = GPIO_Low_Speed;//Ñ¡¶¨Êä³öËÙÂÊ
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//ÍÆÍìÄ£Ê½
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//ÉÏÀ­
    GPIO_Init(GPIOA, &GPIO_InitStruct);//´«²Î³õÊ¼»¯
    
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);//Ä¬ÈÏ²»Ïì
    */

    RCC->AHB1ENR |= 0x01;//ÓÃ¼Ä´æÆ÷´ò¿ªÊ±ÖÓ
    GPIOA->MODER &= ~((uint32_t)0x03<<30);//ÇåÁã
    GPIOA->MODER |= (0x01<<30);//Ñ¡ÔñÒý½Å£¬Ñ¡ÔñÎªÊä³öÄ£Ê½
    //Ä¬ÈÏÎªÍÆÍìÄ£Ê½ºÍÎÞÉÏÀ²ÏÂÀ­£¬ÒòÎª²»ÊÇºÏ×÷Ä£Ê½Ñ¡ÔñÄ¬ÈÏ¼´¿É
    GPIOA->ODR |= (0x00<<15);//Ö¸¶¨Òý½Å¸øÖµ£¬Ä¬ÈÏ¹Ø±Õ
}

/**********************************
å‡½æ•°åç§°: drv_beep_open
å‡½æ•°ä½œç”¨: æ‰“å¼€èœ‚é¸£å™¨
è¾“å…¥å‚æ•°: æ— 
è¾“å‡ºå‚æ•°: æ— 
å…¶    ä»–: 
**********************************/

void drv_beep_open(void)
{
    GPIO_SetBits(GPIOA, GPIO_Pin_15);//Ïì
    //GPIOA->ODR |= (0x01<<15);
}

/**********************************
å‡½æ•°åç§°: drv_beep_close
å‡½æ•°ä½œç”¨: å…³é—­èœ‚é¸£å™¨
è¾“å…¥å‚æ•°: æ— 
è¾“å‡ºå‚æ•°: æ— 
å…¶    ä»–: 
**********************************/

void drv_beep_close(void)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_15);//²»Ïì
   // GPIOA->ODR &= ~(0x01<<15);
}

