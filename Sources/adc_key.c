#include "config.h"                     //头文件中已包含ai8051u.h, ai_usb.h以及其他头文件
#include "adc_key.h"                     //头文件中已包含ai8051u.h, ai_usb.h以及其他头文件


u8  ADC_KeyState,ADC_KeyState1,ADC_KeyState2,ADC_KeyState3; //键状态
u8  ADC_KeyHoldCnt; //键按下计时
u8  KeyCode;    //给用户使用的键码, 1~16有效


void adc_key_init(void)
{
    P1M1|= 0x01;   P1M0|= 0x00;   //设置为准双向口, P1.0高阻输入

    ADCTIM = 0x3f;		//设置 ADC 内部时序，ADC采样时间建议设最大值
    ADCCFG = 0x2f;		//设置 ADC 时钟为系统时钟/2/16/16
    ADC_CONTR = 0x80;   //使能 ADC 模块

    ADC_KeyState  = 0;
    ADC_KeyState1 = 0;
    ADC_KeyState2 = 0;
    ADC_KeyState3 = 0;  //键状态
    ADC_KeyHoldCnt = 0; //键按下计时
    KeyCode = 0;        //给用户使用的键码, 1~16有效
}

//========================================================================
// 函数: u16 Get_ADC12bitResult(u8 channel)
// 描述: 查询法读一次ADC结果.
// 参数: channel: 选择要转换的ADC.
// 返回: 12位ADC结果.
// 版本: V1.0, 2012-10-22
//========================================================================
u16 Get_ADC12bitResult(u8 channel)  //channel = 0~15
{
    ADC_RES = 0;
    ADC_RESL = 0;

    ADC_CONTR = (ADC_CONTR & 0xf0) | channel; //设置ADC转换通道
    ADC_START = 1;//启动ADC转换
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    while(ADC_FLAG == 0);   //wait for ADC finish
    ADC_FLAG = 0;     //清除ADC结束标志
    return  (((u16)ADC_RES << 8) | ADC_RESL);
}

/***************** ADC键盘计算键码 *****************************
电路和软件算法设计: Coody
本ADC键盘方案在很多实际产品设计中, 验证了其稳定可靠, 即使按键使用导电膜,都很可靠.
16个键,理论上各个键对应的ADC值为 (4096 / 16) * k = 256 * k, k = 1 ~ 16, 特别的, k=16时,对应的ADC值是4095.
但是实际会有偏差,则判断时限制这个偏差, ADC_OFFSET为+-偏差, 则ADC值在 (256*k-ADC_OFFSET) 与 (256*k+ADC_OFFSET)之间为键有效.
间隔一定的时间,就采样一次ADC,比如10ms.
为了避免偶然的ADC值误判, 或者避免ADC在上升或下降时误判, 使用连续3次ADC值均在偏差范围内时, ADC值才认为有效.
以上算法, 能保证读键非常可靠.
**********************************************/
#define ADC_OFFSET  64
void CalculateAdcKey(u16 adc)
{
    u8  i;
    u16 j;
    
    if(adc < (256-ADC_OFFSET))
    {
        ADC_KeyState = 0;   //键状态归0
        ADC_KeyHoldCnt = 0;
    }
    j = 256;
    for(i=1; i<=16; i++)
    {
        if((adc >= (j - ADC_OFFSET)) && (adc <= (j + ADC_OFFSET)))  break;  //判断是否在偏差范围内
        j += 256;
    }
    ADC_KeyState3 = ADC_KeyState2;
    ADC_KeyState2 = ADC_KeyState1;
    if(i > 16)  ADC_KeyState1 = 0;  //键无效
    else                        //键有效
    {
        ADC_KeyState1 = i;
        if((ADC_KeyState3 == ADC_KeyState2) && (ADC_KeyState2 == ADC_KeyState1) &&
           (ADC_KeyState3 > 0) && (ADC_KeyState2 > 0) && (ADC_KeyState1 > 0))
        {
            if(ADC_KeyState == 0)   //第一次检测到
            {
                KeyCode  = i;   //保存键码
                ADC_KeyState = i;   //保存键状态
                ADC_KeyHoldCnt = 0;
            }
            if(ADC_KeyState == i)   //连续检测到同一键按着
            {
                if(++ADC_KeyHoldCnt >= 100) //按下1秒后,以10次每秒的速度Repeat Key
                {
                    ADC_KeyHoldCnt = 90;
                    KeyCode  = i;   //保存键码
                }
            }
            else ADC_KeyHoldCnt = 0; //按下时间计数归0
        }
    }
}