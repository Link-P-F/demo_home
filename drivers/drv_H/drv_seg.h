#ifndef __DRV_SEG_H__
#define __DRV_SEG_H__
#include <stm32f4xx.h>

//数码管选择
#define wei_1 0x01
#define wei_2 0x02
#define wei_3 0x04
#define wei_4 0x08

//输出数据
#define duan_0 0x3f
#define duan_1 0x06
#define duan_2 0x5b
#define duan_3 0x4f
#define duan_4 0x66
#define duan_5 0x6d
#define duan_6 0x7d
#define duan_7 0x07
#define duan_8 0x7f
#define duan_9 0x6f
#define duan_a 0x77
#define duan_b 0x7c
#define duan_c 0x39
#define duan_d 0x5e
#define duan_e 0x79
#define duan_f 0x71

void drv_74hc595_init(void);
void drv_seg_display(uint8_t wei, uint8_t duan);
void drv_seg_display_double(double num);
#endif //__DRV_SEG_H__

