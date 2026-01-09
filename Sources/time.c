#include "config.h"                     //头文件中已包含ai8051u.h, ai_usb.h以及其他头文件
#include "time.h"
#include "adc_key.h"
#include "ai_usb.h"
#include "oled.h"

u8  cnt10ms;
bit B_1ms;          //1ms标志
u16 j;
u16 msecond;
u8 dis_num_str[10];
u8 num=0;

void Timer0_Init(void)
{
    AUXR |= 0x80;       //Timer0 set as 1T, 16 bits timer auto-reload, 
    TH0 = (u8)(Timer0_Reload / 256);
    TL0 = (u8)(Timer0_Reload % 256);
    ET0 = 1;            //Timer0 interrupt enable
    TR0 = 1;            //Tiner0 run
}

void time_out()
{

        if(B_1ms)   //1ms到
        {
            B_1ms = 0;
            if(++msecond >= 1000)   //1秒到
            {
                msecond = 0;
                
            }

            if(++cnt10ms >= 10) //10ms读一次ADC
            {
                cnt10ms = 0;
                j = Get_ADC12bitResult(0);  //参数0~15,查询方式做一次ADC, 返回值就是结果, == 4096 为错误
                if(j < 4096)    CalculateAdcKey(j); //计算按键
            }
            
        }
        if(KeyCode > 0)     //有键按下
            {
                if(KeyCode == 1)    
                {
                    WaveData[0]=high_level_num;
                }
                if(KeyCode == 2)    
                {
                    WaveData[0]=low_level_num;
                }
                if(KeyCode == 3)    
                {
                    num=0;
                }
                if(KeyCode == 4)    
                {
                    num=10;
                }
                KeyCode = 0;
            }

                    
}


void timer0() interrupt 1
{
    B_1ms = 1;      //1ms标志
}