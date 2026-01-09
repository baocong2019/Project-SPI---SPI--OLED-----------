#ifndef __pic_H__               //防止头文件被重复包含
#define __pic_H__

#include <ai8051u.h>            //包含外部头文件
#include <string.h>
#include <stdlib.h>
#include "def.h"

extern u8 code OLED12864_IMG0[8][128];
extern u8 code OLED12864_IMG1[8][128];
extern unsigned char xdata Battery[8][128];

#endif
