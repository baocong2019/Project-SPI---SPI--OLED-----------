#ifndef __CONFIG_H__            //防止头文件被重复包含
#define __CONFIG_H__

#define HIRC                    40000000UL
#define FOSC                    40000000UL
#define SYSCLK                  FOSC
#define MAIN_Fosc               FOSC

#define Timer0_Reload   (65536UL -(MAIN_Fosc / 1000))       //Timer 0 中断频率, 1000次/秒


#include <ai8051u.h>            //包含外部头文件
#include <string.h>
#include <stdlib.h>

#include "def.h"                //包含项目头文件


extern unsigned char xdata WaveData[128]; 


void SYS_Init(void);            //函数声明


#endif
