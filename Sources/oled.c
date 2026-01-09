//oled.c
//SPI驱动OLED显示图片、图形、字符

#include "config.h"                     //头文件中已包含ai8051u.h, ai_usb.h以及其他头文件
#include "font.h"
#include "ascii.h"
#include "pic.h"
#include "rand_num.h"
#include "oled.h"
#include "math.h"              //包含项目头文件


#define UseSPI    1     //0: IO模拟SPI;  1: 使用硬件SPI

//OLED屏引脚定义
sbit OLED_SCK	=  	P3^2;	//D0	SPI or IIC 的时钟脚
sbit OLED_SDA	=   P3^3;	//D1	SPI or IIC 的数据脚
sbit OLED_RES	=  	P4^7;	//RES	复位脚, 低电平复位
sbit OLED_DC	=  	P1^1;	//DC	数据或命令脚
sbit OLED_CS	=	P3^5;	//CS	片选脚

//SPI定义
sbit SPI_SCK  = P3^2;   //PIN6
sbit SPI_MOSI = P3^3;   //PIN5

unsigned char xdata ShowBuf[8][128]; //OLED全局缓存
unsigned char xdata WaveData[128]; //按键波数据缓存

extern void delay_ms(unsigned int ms);

//========================================================================
// 函数名称: OLED_WR_CMD(unsigned char Cmd)  
// 函数功能: OLED_写命令字
//========================================================================
void OLED_WR_CMD(unsigned char Cmd)
{ 
#if(UseSPI==0)
    unsigned char i;
#endif  
    OLED_SCK= 0;    //时钟低;
    OLED_DC = 0;    //写命令
    OLED_CS = 0;    //片选中
#if(UseSPI)
    SPDAT = Cmd;
    while(SPIF == 0);
    SPIF = 1;   //清SPIF标志
    WCOL = 1;   //清WCOL标志
#else
    for(i=0;i<8;i++)
    {
        if(Cmd&0x80)
            OLED_SDA=1;
        else
            OLED_SDA=0;
        Cmd <<= 1;    //左移位 //
        OLED_SCK = 1;
        OLED_SCK = 0;
    }
#endif
    OLED_CS = 1;    //片不选
}
//========================================================================
// 函数名称: OLED_WR_DAT(unsigned char Dat)  
// 函数功能: OLED_写数据字
//========================================================================
void OLED_WR_DAT(unsigned char Dat)
{ 
#if(UseSPI==0)
    unsigned char i;
#endif  
    OLED_SCK= 0;    //时钟低;
    OLED_DC = 1;    //写数据
    OLED_CS = 0;    //片选中
#if(UseSPI)
    SPDAT = Dat;
    while(SPIF == 0);
    SPIF = 1;   //清SPIF标志
    WCOL = 1;   //清WCOL标志
#else
    for(i=0;i<8;i++)
    {
        if(Dat&0x80)
            OLED_SDA=1;
        else
            OLED_SDA=0;
        Dat <<= 1;    //左移位 
        OLED_SCK = 1;
        OLED_SCK = 0;
    }
#endif  
    OLED_CS = 1;    //片不选
}
//========================================================================
// 函数名称: OLED_Disp(unsigned char OnOff)
// 函数功能: OLED开关显示 参数: 1:开 0:关
//========================================================================
void OLED_Disp(unsigned char On)  //开关显示
{
    if(On==1)
    {
        OLED_WR_CMD(0x8D); //电荷泵使能
        OLED_WR_CMD(0x14); //开启电荷泵
        OLED_WR_CMD(0xAF); //点亮屏幕
    }
    else
    {
        OLED_WR_CMD(0x8D); //电荷泵使能
        OLED_WR_CMD(0x10); //关闭电荷泵
        OLED_WR_CMD(0xAF); //关闭屏幕        
    }
}
//========================================================================
// 函数名称: OLED_Init() 显示屏初始化函数 
//========================================================================
void OLED_Init(void)
{
#if(UseSPI)
    P_SW1 = (P_SW1 & ~(3<<2)) | (3<<2);     //IO口切换. 0: P1.4 P1.5 P1.6 P1.7, 1: P2.4 P2.5 P2.6 P2.7, 2: P4.0 P4.1 P4.2 P4.3, 3: P3.5 P3.4 P3.3 P3.2
    HSSPI_CFG2 |= 0x40;   //交换MOSI与MISO脚
    SSIG = 1; //忽略 SS 引脚功能，使用 MSTR 确定器件是主机还是从机
    SPEN = 1; //使能 SPI 功能
    DORD = 0; //先发送/接收数据的高位（ MSB）
    MSTR = 1; //设置主机模式
    CPOL = 0; //SCLK 空闲时为低电平，SCLK 的前时钟沿为上升沿，后时钟沿为下降沿
    CPHA = 0; //数据 SS 管脚为低电平驱动第一位数据并在 SCLK 的后时钟沿改变数据
    SPCTL = (SPCTL & ~3) | 3;   //SPI 时钟频率选择, 0: 4T, 1: 8T,  2: 16T,  3: 2T
    SPI_SCK  = 0;  //时钟信号置低
    SPI_MOSI = 1;
    SPIF = 1;   //清SPIF标志
    WCOL = 1;   //清WCOL标志
#endif 
    OLED_SCK = 0;               //时钟端低
    OLED_SDA = 1; delay_ms(100); //数据端高 
    OLED_RES = 1; delay_ms(200);
    OLED_RES = 0; delay_ms(200); //复位
    OLED_RES = 1;
//SSD1315 驱动IC
    OLED_WR_CMD(0xAE);//--turn off oled panel
    OLED_WR_CMD(0x00);//--set low column address
    OLED_WR_CMD(0x10);//--set high column address
    OLED_WR_CMD(0x40);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_CMD(0x81);//--set contrast control register 设置对比度寄存器
    OLED_WR_CMD(0xCF);//--Set SEG Output Current Brightness 亮度... 取值范围0x00~0xFF 0xcf=207
    OLED_WR_CMD(0xA1);//--Set SEG/Column Mapping     0xA0左右反转 0xA1正常
    OLED_WR_CMD(0xC8);//--Set COM/Row Scan Direction 0xC0上下反转 0xC8正常
    OLED_WR_CMD(0xA6);//--set normal display
    OLED_WR_CMD(0xA8);//--set multiplex ratio(1 to 64)
    OLED_WR_CMD(0x3f);//--1/64 duty
    OLED_WR_CMD(0xD3);//--set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_CMD(0x00);//--not ofset
    OLED_WR_CMD(0xd5);//--set display clock divide ratio/oscillator frequency
    OLED_WR_CMD(0x80);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_CMD(0xD9);//--set pre-charge period
    OLED_WR_CMD(0xF1);//--Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_CMD(0xDA);//--set com pins hardware configuration
    OLED_WR_CMD(0x12);
    OLED_WR_CMD(0xDB);//--set vcomh
    OLED_WR_CMD(0x40);//--Set VCOM Deselect Level
    OLED_WR_CMD(0x20);//--Set Page Addressing Mode (0x00页地址模式/0x01水平地址模式/0x02垂直地址模式)
    OLED_WR_CMD(0x00);//
    OLED_WR_CMD(0x8D);//--set Charge Pump enable/disable
    OLED_WR_CMD(0x14);//--set(0x10) disable
    OLED_WR_CMD(0xA4);//--Disable Entire Display On (0xa4/0xa5)
    OLED_WR_CMD(0xA6);//--Disable Inverse Display On (0xa6/a7) 
    OLED_WR_CMD(0xAF);
    OLED_Disp(1);     //开显示
}
//========================================================================
// 函数名称: OLED_SetXY
// 函数功能: OLED设置显示位置
// 入口参数: X-水平坐标 0..127,Y-垂直坐标0..63
//========================================================================
void OLED_SetXY(unsigned char X, unsigned char Y) 
{ 
    OLED_WR_CMD((u8)(0xB0+Y));
    OLED_WR_CMD((u8)(((0xF0&X)>>4)|0x10));
    OLED_WR_CMD((u8)(((0x0f&X)|0x01))); 
}

//========================================================================
// 函数名称: OLED_Show
// 函数功能: OLED刷新显示(将显示缓存的数据刷到显示屏显示出来)
//========================================================================
void OLED_Show(void)     
{
    unsigned char xdata i,n;

    for(i=0;i<8;i++)       //8行(页)循环...
    {
        OLED_WR_CMD((u8)(0xB0+i)); //设置行起始地址
        OLED_WR_CMD(0x00);   //设置低列起始地址
        OLED_WR_CMD(0x10);   //设置高列起始地址
        for(n=0;n<128;n++)   //每行(页)128点(列)
        OLED_WR_DAT(ShowBuf[i][n]); //写数据...
    }
}
//========================================================================
// 函数名称: OLED_CLS 清屏函数
// 函数功能: OLED刷新显示(将显示缓存的0数据刷到显示屏显示出来)
//========================================================================
void OLED_CLS(void)     
{ 
    memset(ShowBuf,0,128*8); //清空缓存
    OLED_Show();             //刷新显示 
}
//========================================================================
// 函数名称: OLED_Light(unsigned char num)
// 函数功能: OLED亮度级设置 
//========================================================================
void OLED_Light(unsigned char Level)
{
    OLED_WR_CMD(0x81);  //
    OLED_WR_CMD(Level); //  
    OLED_WR_CMD(0xDB);  //--set vcomh
    OLED_WR_CMD(0x20);  //Set VCOM Deselect Level   
}

//========================================================================
// 函数名称: OLED_Point
// 函数功能: OLED显示一个点
// 入口参数: X：水平点 0..127 Y:垂直点0..63
//========================================================================
void OLED_Point(unsigned char X,unsigned char Y,u8 mode)
{
    if(X>127)return;
    if(Y>63)return;

    ShowBuf[Y/8][X] |= 1<<(Y%8);  //垂直8点,高位在下,低位在上.
    if(mode)OLED_Show();
}

void OLED_ClearPoint(unsigned char X,unsigned char Y,u8 mode)
{
    if(X>127)return;
    if(Y>63)return;

    ShowBuf[Y/8][X] &= ~(1<<(Y%8));  
    if(mode)OLED_Show();
}
//========================================================================
// 函数名称: OLED_BMP
// 函数功能: OLED缓存写入BMP格式图片
// 入口参数: X：水平起点 0..127 Y:垂直起点0..63  W: 宽度 H:高度 *BMP图片数组
//========================================================================
void OLED_BMP(unsigned char X, unsigned char Y,unsigned char W, unsigned char H,unsigned char BMP[],u8 mode)
{
    unsigned int xdata num=0;
    unsigned char i,l;
    for( i=0;i<H;i++ )
    {
        for(l=0;l<W;l++)
        {
            ShowBuf[Y+i][X+l] = BMP[num++];
        }
    }
    if(mode)
    {
        OLED_Show();
    }
} 

//========================================================================
// 画一条水平线
//========================================================================
void OLED_LineH(unsigned char X,unsigned char Y,unsigned char W,u8 mode)
{
    unsigned char i;
    for(i=0;i<W;i++)
    {
        OLED_Point((u8)(X+i),Y,0);
    }
    if(mode)
    {
        OLED_Show();
    }
}  
//========================================================================
// 画一条垂直线
//========================================================================
void OLED_LineV(unsigned char X,unsigned char Y,unsigned char H,u8 mode)
{
    unsigned char i;
    for(i=0;i<H;i++)
    {
        OLED_Point(X,(u8)(Y+i),0);
    }
    if(mode)
    {
        OLED_Show();
    }
}
//========================================================================
// 函数名称: OLED_BuffShowPoint
// 函数功能: OLED显示一条线
// 入口参数: X0起点 Y0起点 X1终点 Y1终点
//========================================================================
void OLED_Line( unsigned char X0, unsigned char Y0, unsigned char X1,unsigned char Y1,u8 mode)
{
    unsigned char x,y;
    if(X0>X1)  //为从左到右而交换坐标.
    {
        x=X0;X0=X1;X1=x;
        y=Y0;Y0=Y1;Y1=y;
    }
    if(X0!=X1)
    {
        for(x=0;x<(X1-X0);x++ )
        {
            if(Y1>Y0)
                OLED_Point((u8)(X0+x),(u8)(Y0+(u16)(Y1-Y0)*(u16)x/(u16)(X1-X0)),0); //计算斜率推算y坐标
            else
                OLED_Point((u8)(X0+x),(u8)(Y0-(u16)(Y0-Y1)*(u16)x/(u16)(X1-X0)),0);
        }
    }
    else
    {
        if(Y0>Y1)
        {
            for(y=Y1; y<=Y0; y++ )  OLED_Point(X0,y,0);
        }
        else
        {
            for(y=Y0; y<=Y1; y++ )  OLED_Point(X0,y,0);
        }
    }
    if(mode)
    {
        OLED_Show();
    }
}
//========================================================================
// 画一个方框
//========================================================================
void OLED_Box(unsigned char X,unsigned char Y,unsigned char W,unsigned char H,u8 mode)
{
    OLED_LineH(X,Y,W,0);
    OLED_LineV(X,Y,H,0);
    OLED_LineH(X,(u8)(Y+H),W,0);
    OLED_LineV((u8)(X+W),Y,H,0);
    if(mode)
    {
        OLED_Show();
    }
}  
//========================================================================
// 函数名称: OLED_Circle
// 函数功能: OLED显示一个圆形
// 入口参数: X点  Y点  r:半径
//     turn: 0:正显示 1:反显示  mode: 1:立即刷新显示 0:仅写缓存
//========================================================================
void OLED_Circle(unsigned char X,unsigned char Y,unsigned char r,u8 mode)
{
    int a, b, di;
    a = 0;
    b = r;
    di = 3 - (r << 1);        //判断下个点位置的标志
    while (a <= b)
    {
        OLED_Point((u8)(X+a),(u8)(Y-b),0);  //5
        OLED_Point((u8)(X+b),(u8)(Y-a),0);  //0
        OLED_Point((u8)(X+b),(u8)(Y+a),0);  //4
        OLED_Point((u8)(X+a),(u8)(Y+b),0);  //6
        OLED_Point((u8)(X-a),(u8)(Y+b),0);  //1
        OLED_Point((u8)(X-b),(u8)(Y+a),0);  //3
        OLED_Point((u8)(X-a),(u8)(Y-b),0);  //2
        OLED_Point((u8)(X-b),(u8)(Y-a),0);  //7
        a++;
        //使用Bresenham算法画圆
        if (di < 0)
        {
            di += 4 * a + 6;
        }
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
    if(mode)
    {
        OLED_Show();
    }
}
//========================================================================
// 函数名称: OLED_A08
// 函数功能: OLED显示一个6*8字符
// 入口参数: X: 水平坐标列 0..127  Y: 垂直坐标行(页) 0..7 
//     turn: 0:正显示 1:反显示  mode: 1:立即刷新显示 0:仅写缓存
//========================================================================
void OLED_A08(unsigned char X,unsigned char Y,char Ch,u8 turn,u8 mode)     //OLED显示一个5*7字符
{
    unsigned char i;
    for( i=0;i<6;i++ )
    {
        if(turn==0)            
            ShowBuf[Y][X+i]= ASC8[Ch-' '][i];
        else
            ShowBuf[Y][X+i]=~ASC8[Ch-' '][i];
    }
    if(mode)
    {
        OLED_Show();
    }
}
//========================================================================
// 函数名称: OLED_A16
// 函数功能: OLED显示一个8*16字符
// 入口参数: X: 水平坐标列 0..127  Y: 垂直坐标行(页) 0..7 
//     turn: 0:正显示 1:反显示  mode: 1:立即刷新显示 0:仅写缓存
//========================================================================
void OLED_A16(unsigned char X,unsigned char Y,char Ch,u8 turn,u8 mode)     //OLED显示一个8*16字符
{
    unsigned char i,j;
    for( j=0;j<2;j++ )
    {
        for( i=0;i<8;i++ )
        {
            if(turn==0)            
                ShowBuf[Y+j][X+i]= ASC16[Ch-' '][i+j*8];
            else
                ShowBuf[Y+j][X+i]=~ASC16[Ch-' '][i+j*8];
        }
    }
    if(mode)
    {
        OLED_Show();
    }
}
//========================================================================
// 函数名称: OLED_HZ16
// 函数功能: OLED显示一个16*16汉字
// 入口参数: X: 水平坐标列 0..127  Y: 垂直坐标行(页) 0..7 
//     turn: 0:正显示 1:反显示  mode: 1:立即刷新显示 0:仅写缓存
//========================================================================
void OLED_HZ16(unsigned char X,unsigned char Y,char *Hz,u8 turn,u8 mode)
{
    unsigned char i,j;
    unsigned int m;

    //查找字库汉字表
    for(m=0;m<FONT_NUM;m++)
    {
        if((Font_Data[m].txt[0]==Hz[0])&&(Font_Data[m].txt[1]==Hz[1]))
        {
            for( j=0;j<2;j++ )
            {
                for( i=0;i<16;i++ )
                {
                    if(turn==0)            
                        ShowBuf[Y+j][X+i]= Font_Data[m].dat[i+j*16];
                    else
                        ShowBuf[Y+j][X+i]=~Font_Data[m].dat[i+j*16];
                }
            }
        }
    }    
    if(mode)
    {
        OLED_Show();
    }
}
//========================================================================
// 函数名称: OLED_Str5x7
// 函数功能: OLED显示5*7点阵字符串
// 入口参数: X: 水平坐标列 0..127  Y: 垂直坐标行(页) 0..7 
//     turn: 0:正显示 1:反显示  mode: 1:立即刷新显示 0:仅写缓存
//========================================================================
void OLED_Str5x7(unsigned char X,unsigned char Y,char *s,u8 turn,u8 mode)
{ 
    while(*s != '\0') //字符串不为空，循环
    {
        OLED_A08(X,Y,*s,turn,0);
        X += 6;
        s++;
    }       
    if(mode)
    {
        OLED_Show();
    }
}
//========================================================================
// 函数名称: OLED_String
// 函数功能: OLED显示字符串。
// 入口参数: X: 水平坐标列 0..127  Y: 垂直坐标行(页) 0..7 
//     turn: 0:正显示 1:反显示  mode: 1:立即刷新显示 0:仅写缓存
//========================================================================
void OLED_String(unsigned char X,unsigned char Y,char *s,u8 turn,u8 mode)
{
    char hz[2]; 
	while(*s != '\0')       //字符串不为空，循环
	{
        if ((unsigned char)*s < 0x80)     //根据输入数据的大小判断是字符还是汉字，
        {
            OLED_A16(X,Y,*s,turn,0);
            X += 8;
            s++;
        }
        else
        {
            hz[0] = *s ;
            hz[1] = *(s+1) ;
            OLED_HZ16(X,Y,hz,turn,0);
            X += 16;
            s += 2;
        }
		if(X>127)   //行
        {
            X = 0;
            Y += 2;
        }
	}       
    if(mode)
    {
        OLED_Show();
    }
}

void oled_mask_Gray(unsigned char x_start,unsigned char y_start)
{
    unsigned char x,y;
    bit is_pixel_on;

    for(y=y_start;y<64;y+=2)
    {
        for(x=x_start;x<128;x+=3)
        {
            is_pixel_on =Battery[y/8][x]&(0x01<<(y%8));//判断当前像素是否为1
            if(is_pixel_on)
            {
                OLED_Point(x,y,0);
            }
        }
    }
    OLED_Show();
}

void oled_mask_Gray_fan(unsigned char x_start,unsigned char y_start)
{
    unsigned char x,y;
    bit is_pixel_on;

    for(y=y_start;y<64;y+=2)
    {
        for(x=x_start;x<128;x+=3)
        {
            is_pixel_on =Battery[y/8][x]&(0x01<<(y%8));
            if(is_pixel_on)
            {
                OLED_ClearPoint(x,y,0);//清除当前像素
            }
        }
    }
    OLED_Show();
}

void test_oled_duibidu()
{
    unsigned char i=0;
    /*
    设置对比度,查看图片变化
    */
        OLED_BMP(0,0,128,8,OLED12864_IMG0[0],1); //显式BMP图 深圳国芯STCMCU 32位单片机 最后一个参数为1，代表立即刷新屏幕
        for(i=0;i<255;i++)//从低到高 增加对比度？
        {
            OLED_Light(i);   
            delay_ms(10);
        }   

        //OLED_BMP(0,0,128,8,Battery[0],1); //显示电池图标

        OLED_CLS();                             //清屏
        OLED_String(5,1,"深圳大学+STC",0,1);    //中西文混合显示 
        OLED_String(5,3,"深圳大学+STC",1,1);    //正显/反显 
        OLED_Str5x7(16,6,"Ai8051U-34K64",0,1);  //正显  
        OLED_Str5x7(16,7,"Ai8051U-34K64",1,1);  //反显 

        for(i=255;i>0;i--)
        {
            OLED_Light(i);   //从高到低 减少对比度？
            delay_ms(10);
        }
}     

void test_6_gray_img()
{
    //以6阶灰度（田字格）显示
    oled_mask_Gray(0,0);delay_ms(250);//6阶灰度 x步长3 y步长2  把田字格填充到0,0上  参数1为x坐标 参数2为y坐标
    oled_mask_Gray(1,0);delay_ms(250);//mask:遮罩 gray：灰度  总体理解为蒙版
    oled_mask_Gray(2,0);delay_ms(250);
    oled_mask_Gray(0,1);delay_ms(250);
    oled_mask_Gray(1,1);delay_ms(250);
    oled_mask_Gray(2,1);delay_ms(250);

    oled_mask_Gray_fan(0,0);delay_ms(250);//6阶灰度 x步长3 y步长2 
    oled_mask_Gray_fan(1,0);delay_ms(250);
    oled_mask_Gray_fan(2,0);delay_ms(250);
    oled_mask_Gray_fan(0,1);delay_ms(250);
    oled_mask_Gray_fan(1,1);delay_ms(250);
    oled_mask_Gray_fan(2,1);delay_ms(250);
}

void test_dis_line()
{
    OLED_CLS();                //清屏
    OLED_LineH(10,10,108,1);   //画一水平线 作图算法...x,y,长度,是否立即显示
    delay_ms(1000);
    OLED_LineV(10,10,44,1);    //画一垂直线
    delay_ms(1000);
    OLED_Line(10,10,118,54,1); //两点一线 不同方向画线... x0,y0,x1,y1,是否立即显示
    delay_ms(1000);
    OLED_Line(10,54,118,10,1); //两点一线
    delay_ms(1000);
    OLED_Line(118,54,118,10,1);//两点一线
    delay_ms(1000);
    OLED_Line(118,54,10,54,1); //两点一线
    delay_ms(1000);            
}

void test_Boxing_wave(unsigned char step)//step:步长 波形图显示
{
    unsigned char Pointx=0;//x坐标
    unsigned char Pointy=0;//y坐标
    unsigned char Xtime;//x坐标时间 0-128按步长增长
    unsigned char range=63;//y坐标范围 12864 最宽就是63 ，进行等比例缩放
    float scale = (float)range/255;//scale:缩放比例
    unsigned char Pointxlast,Pointylast;
    char step_str[16]; // 用于存储step的字符串表示

    Pointx=0;
    Pointy=(unsigned char)(rand_num[0]*scale);//初始y坐标 随机数*缩放比例 取整

    sprintf(step_str, "step=%d", step);
    OLED_String(0,0,step_str,0,1);

    for(Xtime=0;Xtime<128;Xtime+=step)
    {
        Pointxlast=Pointx;
        Pointylast=Pointy;

        Pointx=Xtime;
        Pointy=rand_num[Xtime]*scale;//y坐标 随机数*缩放比例 
        OLED_Line(Pointxlast,(unsigned char)(63-Pointylast),Pointx,(unsigned char)(63-Pointy),1);
        delay_ms(50);
    }
}

void test_dis_boxing()
{
    test_Boxing_wave(1);
    OLED_CLS();     
    test_Boxing_wave(5);
    OLED_CLS();    
    test_Boxing_wave(10);
    OLED_CLS();    
    test_Boxing_wave(20);
    OLED_CLS();   
}


void ButtonWave()
{
    unsigned char Xtime;
    
    for(Xtime=127;Xtime>0;Xtime--)
    {
        WaveData[Xtime]=WaveData[Xtime-1];
    }

    // if(P34)//默认高电平
    // {
    //     WaveData[0]=high_level_num;
    // }
    // else
    // {
    //     WaveData[0]=low_level_num;
    // }

    OLED_CLS();
    for(Xtime=0;Xtime<127;Xtime++)
    {
        OLED_Line(Xtime,(unsigned char)(63-WaveData[Xtime]) ,Xtime,(unsigned char)(63-WaveData[Xtime+1]),0);//x0,y0,x1,y1,是否立即显示
    }
    OLED_Show();
    delay_ms(50);
}

void oled_draw_line(u8 x0,u8 y0,u8 x1,u8 y1,bit draw,u8 dot)//draw:
{
    u8 lx = abs(x1-x0); //lx:水平距离 abs()取绝对值
    u8 ly = abs(y1-y0);
    u8 dm,i;   //取xyl中较大的一个作为循环次数
    float dx,dy; //dx:水平增量 dy:垂直增量
    float x,y;


    if(lx >= ly)//如果水平距离大于等于垂直距离
    {
        dm = lx;
    }
    else
    {
        dm = ly;
    }

    dx=(float)(x1-x0)/dm;//每节的x变化增量
    dy=(float)(y1-y0)/dm;//每节的y变化增量
    x=(float)x0+0.5;//x0+0.5:为了避免浮点数计算误差，将x0,y0转换为浮点数后再加上0.5
    y=(float)y0+0.5;//y0+0.5:为了避免浮点数计算误差，将x0,y0转换为浮点数后再加上0.5
    for(i=0;i<=dm;i++)
    {
        if(draw)
        {
            if(!(i%dot))
            OLED_Point((u8)x,(u8)y,1);
        }
        else
        {
            OLED_ClearPoint((u8)x,(u8)y,1);
        }
        x+=dx;//画完一个点后，x坐标增加一个增量
        y+=dy;//画完一个点后，y坐标增加一个增量
    }
}

void oled_draw_frame(u8 x0,u8 y0,u8 x1,u8 y1)//画一个矩形框，间隔dot
{
    oled_draw_line(x0,y0,x1,y0,1,5);    //画上边框的一条水平线
    oled_draw_line(x1,y0,x1,y1,1,5);    //画右边框的一条垂直线
    oled_draw_line(x1,y1,x0,y1,1,5);    //画下边框的一条水平线
    oled_draw_line(x0,y1,x0,y0,1,5);    //画左边框的一条垂直线
}

void oled_drawblock(u8 x0,u8 y0,u8 x1,u8 y1)//画一个矩形块，间隔dot
{
    char i,d;//i:循环变量 d:绘制方向
    
    if(y1-y0>=0)        // 确定绘制方向：从上到下(y1>=y0)或从下到上(y1<y0)
    d=1;
    else
    d=-1;
                                            // 绘制一条水平线，从(x0, y0+i*d)到(x1, y0+i*d)
                                            // 参数说明：
                                            // - x0, y0+i*d: 线的起点坐标
                                            // - x1, y0+i*d: 线的终点坐标
                                            // - 1: 表示绘制线（而不是清除）
                                            // - 1: 表示每个像素都绘制（不跳过任何点）
    for(i=0;i<=abs(y1-y0);i++)
    {
        oled_draw_line(x0,(u8)(y0+i*d),x1,(u8)(y0+i*d),1,1);
        OLED_Show();
        delay_ms(50);
    }
}

void oled_drawsin()
{
    u8 x0=64,y0=32;         // 设置坐标系原点为屏幕中心(64,32)
    float si,sx,sy,rad;     // si:角度变量, sx:x坐标, sy:y坐标, rad:弧度值

    oled_draw_line(0,32,127,32,1,2);OLED_Show();    //画sin函数的x轴
    oled_draw_line(64,0,64,64,1,2);OLED_Show();    //画sin函数的y轴

    for(si=-180;si<180;si+=1)
    {
        rad=si*3.14/180;// 将角度转换为弧度（π/180）
        sx=si/3.5;//X轴缩放比例：1/3.5（将角度范围映射到屏幕宽度） // 除以3.5是为了将360度的角度范围适配到约103像素的宽度
        sy=sin(rad)*25;//Y轴缩放比例：25（将sin值范围映射到屏幕高度[-1,1]）// 正弦函数值范围[-1,1]，乘以25后范围为[-25,25]

        OLED_Point(x0+sx,64-(y0+sy),1);
        OLED_Show();
        delay_ms(10);
    }
    OLED_CLS();
}

void oled_drawCircle(u8 x0,u8 y0,u8 R,bit draw)
{
    float Rx,Ry,angle,rad;

    OLED_Point(x0,y0,1);

    for(angle=0;angle<360;angle+=1)
    {
        rad=angle*3.14/180;
        Rx=R*cos(rad);
        Ry=R*sin(rad);

        if(draw==1)
        {
            OLED_Point(x0+(u8)Rx,y0+(u8)Ry,1);
        }
        else
        {
            OLED_ClearPoint(x0+(u8)Rx,y0+(u8)Ry,1);
        }   
        OLED_Show();
    }
}