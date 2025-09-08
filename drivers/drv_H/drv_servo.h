#ifndef __DRV_SERVO_H__
#define __DRV_SERVO_H__
#include <stm32f4xx.h>
//引脚
#define RIGHT_HIDE GPIO_Pin_0	//PB0
#define LEFT_HIDE GPIO_Pin_1	//PB1
#define LEFT_FRONT GPIO_Pin_7	//PA7
#define RIGHT_FRONT GPIO_Pin_6	//PA6
//端口 
#define SERVO_PORT_B GPIOB
#define SERVO_PORT_A GPIOA
//时钟
#define SERVO_RCC_A RCC_AHB1Periph_GPIOA
#define SERVO_RCC_B RCC_AHB1Periph_GPIOB
#define SERVO_RCC_TIM RCC_APB1Periph_TIM3

#define RIGHTF_UP drv_servo_set_angle(1,135);//右前腿往前
#define RIGHTF_DOWN drv_servo_set_angle(1,45);//右前腿往后
#define RIGHTF_STAND drv_servo_set_angle(1,90);

#define RIGHTH_UP  drv_servo_set_angle(3,135);//右后腿往前
#define RIGHTH_DOWN drv_servo_set_angle(3,45);//右后腿往后
#define RIGHTH_STAND drv_servo_set_angle(3,90);

#define LEFTF_UP drv_servo_set_angle(2,45);//左前腿抬起
#define LEFTF_DOWN drv_servo_set_angle(2,135);//左前腿往后
#define LEFTF_STAND drv_servo_set_angle(2,90);

#define LEFTH_UP drv_servo_set_angle(4,45);//左后腿往前
#define LEFTH_DOWN drv_servo_set_angle(4,135);//左后腿往后
#define LEFTH_STAND drv_servo_set_angle(4,90);

void drv_servo_init(void);
void drv_servo_set_angle(uint8_t channel, uint16_t angle);
void drv_servo_stand(void);
void drv_servo_down(void);
void drv_servo_sit_down(void);
void drv_servo_advance(void);
void drv_servo_back(void);
void drv_servo_turn_left(void);
void drv_servo_turn_right(void);


#endif //__DRV_SERVO_H__
