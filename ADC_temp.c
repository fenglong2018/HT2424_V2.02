/*
 *  ADC_temp.c
 *  用作RT3温度采集
 *  Created on: 2019年8月28日
 *  Author: wk
 */

#include "msp430g2553.h"
#include "ADC_temp.h"
#include "calculate.h"
UINT16 RT3_AD=0;

void InitAdc()
{
//    ADC10CTL1 |= CONSEQ_2; //单通道重复采样模式
//    ADC10CTL0 |= SREF_1+REFON+REF2_5V;//选择内部参考源2.5V，打开基准源
//    ADC10CTL0 |= ADC10SHT_3+MSC; //过采样率设置为64个采样周期，打开AD转换
//    ADC10CTL1|= ADC10SSEL_1+ADC10DIV_1+SHS_0; //ACLK2分频为采样时钟，用ADC10SC触发采集
//    ADC10CTL1 |=INCH_0; //选择通道A1
//    ADC10CTL0 |= ADC10ON; //开启ADC10
    ADC10CTL0 &= ~ENC;    //转换允许位禁止
    ADC10CTL0  |= ADC10SHT_3 + ADC10ON+SREF_0;//VCC-VSS,64个周期
    ADC10CTL1  |= (INCH_0 + SHS_0 + CONSEQ_0 + ADC10SSEL_0) ;     //定义通道0，SHS=采样触发源为ADC10SC,CONSEQ_0=单通道单次转换模式， ADC10SSEL_0=时钟选择ADC10OSC
}

void get_RT3_temp()
{
    UINT16 ADC10_temp = 0;
    UINT16 ADC10_max = 0;//最大AD
    UINT16 ADC10_min = 0xffff;//最小AD
    char i;
    RT3_AD =0;
    ADC10CTL0  |= ENC + ADC10SC;
    _delay_cycles(100);
    for(i=0;i<6;i++)//每六次采集均值一次
    {
       while(ADC10CTL1 & ADC10BUSY);//保证ADC当前无进行中活动
       while(ADC10CTL0 & ADC10IFG==0);//保证ADC中断已结束
       ADC10_temp = ADC10MEM;
       //取出最大值
       if(ADC10_max < ADC10_temp)
       {
           ADC10_max = ADC10_temp;
       }

       //取出最小值
       if(ADC10_min > ADC10_temp)
       {
           ADC10_min = ADC10_temp;
       }

       RT3_AD+=ADC10_temp; //采样结果累加//+

       _delay_cycles(30);
       ADC10CTL0 &=~ ADC10IFG;//清中断（也不知道必不必要）
    }
    ADC10CTL0 &= ~ENC;

    RT3_AD= RT3_AD - ADC10_min - ADC10_max;//去掉最大最小值
    RT3_AD = RT3_AD >> 2;//除四（位移两位）
}
