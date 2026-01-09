#ifndef __OLED_H__               //防止头文件被重复包含
#define __OLED_H__

/*  OLED Pixel */
#define WIDTH 128
#define HEIGHT 64
#define PAGES   8

/*  OLED Brightness */
#define BRIGHTNESS_MIN 1
#define BRIGHTNESS_MAX 25

/*  显示屏显示按键高低状态 */
#define high_level_num 50;
#define mid_level_num  30;
#define low_level_num  10;


extern unsigned char xdata WaveData[128]; //按键波数据缓存

/*  OLED Driver */
void OLED_Init(void);  //初始化 
void OLED_Show(void);  //显示(缓存数据刷屏) 
void OLED_Disp(unsigned char On);                  //开关显示
void OLED_CLS(void);   //清屏
void OLED_Light(unsigned char Level);              //亮度设置
void OLED_SetXY(unsigned char X,unsigned char Y);  //设定坐标
void OLED_Point(unsigned char X,unsigned char Y,u8 mode); //画一个点
void OLED_ClearPoint(unsigned char X,unsigned char Y,u8 mode);//清除一个点
void OLED_LineH(unsigned char X,unsigned char Y,unsigned char W,u8 mode);  //画一条水平线
void OLED_LineV(unsigned char X,unsigned char Y,unsigned char H,u8 mode);  //画一条垂直线
void OLED_Line( unsigned char X0, unsigned char Y0, unsigned char X1,unsigned char Y1,u8 mode);
void OLED_Box(unsigned char X,unsigned char Y,unsigned char W,unsigned char H,u8 mode);  //画一个方框
void OLED_Circle(unsigned char X, unsigned char Y, unsigned char r,u8 mode);             //画一个圆
void OLED_BMP(unsigned char X,unsigned char Y,unsigned char W,unsigned char H,unsigned char BMP[],u8 mode);
void OLED_A08(unsigned char X,unsigned char Y,char Ch, u8 turn,u8 mode);   //OLED显示一个5*7字符
void OLED_A16(unsigned char X,unsigned char Y,char Ch, u8 turn,u8 mode);   //OLED显示一个8*16字符
void OLED_HZ16(unsigned char X,unsigned char Y,char *Hz, u8 turn,u8 mode); //OLED显示一个16*16汉字
void OLED_Str5x7(unsigned char X,unsigned char Y,char *s,u8 turn,u8 mode); //OLED显示5*7点阵字符串
void OLED_String(unsigned char X,unsigned char Y,char *s,u8 turn,u8 mode); //OLED显示16点阵高字符串
void OLED_WR_DAT(unsigned char Dat);
void oled_mask_Gray(unsigned char x_start,unsigned char y_start);
void oled_mask_Gray_fan(unsigned char x_start,unsigned char y_start);
void test_oled_duibidu(void);
void test_6_gray_img(void);
void test_dis_line(void);
void test_Boxing_wave(unsigned char step);
void ButtonWave(void);
void test_dis_boxing(void);
void oled_draw_line(u8 x0,u8 y0,u8 x1,u8 y1,bit draw,u8 dot);
void oled_draw_frame(u8 x0,u8 y0,u8 x1,u8 y1);
void oled_drawblock(u8 x0,u8 y0,u8 x1,u8 y1);//画一个矩形块，间隔dot
void oled_drawsin(void);
void oled_drawCircle(u8 x0,u8 y0,u8 R,bit draw);


#endif
