#ifndef __ADC_KEY_H__
#define __ADC_KEY_H__

#include "config.h"                     //头文件中已包含ai8051u.h, ai_usb.h以及其他头文件
extern u8  KeyCode;    //给用户使用的键码, 1~16有效

void adc_key_init(void);
u16 Get_ADC12bitResult(u8 channel); //channel = 0~15
void CalculateAdcKey(u16 adc);


#endif