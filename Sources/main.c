//详情查看readme.md

#include "config.h"                     //头文件中已包含ai8051u.h, ai_usb.h以及其他头文件
#include "pic.h"
#include "oled.h"
#include  "ai_usb.h"	//USB库和USB虚拟设备的头文
#include "adc_key.h"
#include "time.h"

void delay_ms(unsigned int ms);

void main()
{
    unsigned char i;

    SYS_Init();              //系统初始化
    OLED_Init();             //初始化
    OLED_CLS();              //清屏
    OLED_Light(0x80);        //中间亮度
    OLED_CLS();              //清屏
	usb_init();
    Timer0_Init();
    adc_key_init();
    EA = 1;                 //使能全局中断

    for(i=0;i<128;i++)
    {
        WaveData[i]=mid_level_num;
    }

    while (1)
    {   
        //test_oled_duibidu();//产生现象为图案渐灭渐亮
        //test_6_gray_img();//测试6阶灰度图像显示
        //test_dis_line();//测试显示线
        //test_dis_boxing();测试静态波形图
        //ButtonWave();按键波动图形显示
        //time_out();

        // oled_draw_line(64,32,0,0,1,5);OLED_Show();delay_ms(500);
        // oled_draw_line(64,32,64,0,1,5);OLED_Show();delay_ms(500);
        // oled_draw_line(64,32,127,0,1,5);OLED_Show();delay_ms(500);
        // oled_draw_line(64,32,127,32,1,5);OLED_Show();delay_ms(500);

        // oled_draw_line(64,32,127,64,1,5);OLED_Show();delay_ms(500);
        // oled_draw_line(64,32,64,64,1,5);OLED_Show();delay_ms(500);
        // oled_draw_line(64,32,0,64,1,5);OLED_Show();delay_ms(500);
        // oled_draw_line(64,32,0,32,1,5);OLED_Show();delay_ms(500);
        
        //oled_draw_frame(0,0,127,63);OLED_Show();delay_ms(500);
        //oled_drawblock(32,0,64,63);OLED_Show();delay_ms(500);

        //oled_drawsin();
        oled_drawCircle(64,32,32,1);
    }
}

void SYS_Init(void)                 //系统初始化函数
{
    EAXFR = 1;                      //使能访问扩展XFR
    WTST = 0x00;                    //设置最快速度访问程序代码
    CKCON = 0x00;                   //设置最快速度访问内部XDATA
    
    P0M1 = 0x00;   P0M0 = 0x00;     //设置为准双向口
    P1M1 = 0x00;   P1M0 = 0x00;     //设置为准双向口
    P2M1 = 0x00;   P2M0 = 0x00;     //设置为准双向口
    P3M1 = 0x00;   P3M0 = 0x2c;     //设置为准双向口, P3.2,P3.3,P3.5设置为推挽输出
    P4M1 = 0x00;   P4M0 = 0x80;     //设置为准双向口, P4.7设置为推挽输出
    P5M1 = 0x00;   P5M0 = 0x00;     //设置为准双向口
    P6M1 = 0x00;   P6M0 = 0x00;     //设置为准双向口
    P7M1 = 0x00;   P7M0 = 0x00;     //设置为准双向口

    P3SR = 0x00;                    //电平转换速度快（改善IO口高速翻转信号）
    P3DR = 0xff;                    //端口驱动电流增强
}

//========================================================================
// 函数: void delay_ms(unsigned int ms)
// 描述: 毫秒级延时函数。
// 参数: ms,要延时的ms数,自动适应主时钟.
//=====================================================================
void delay_ms(unsigned int ms)
{
    unsigned int i;
    do{
        i = MAIN_Fosc / 6000;
        while(--i);
    } while(--ms);
}
