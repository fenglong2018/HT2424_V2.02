/*
 *  ADC_temp.c
 *  ����RT3�¶Ȳɼ�
 *  Created on: 2019��8��28��
 *  Author: wk
 */

#include "msp430g2553.h"
#include "ADC_temp.h"
#include "calculate.h"
UINT16 RT3_AD=0;

void InitAdc()
{
//    ADC10CTL1 |= CONSEQ_2; //��ͨ���ظ�����ģʽ
//    ADC10CTL0 |= SREF_1+REFON+REF2_5V;//ѡ���ڲ��ο�Դ2.5V���򿪻�׼Դ
//    ADC10CTL0 |= ADC10SHT_3+MSC; //������������Ϊ64���������ڣ���ADת��
//    ADC10CTL1|= ADC10SSEL_1+ADC10DIV_1+SHS_0; //ACLK2��ƵΪ����ʱ�ӣ���ADC10SC�����ɼ�
//    ADC10CTL1 |=INCH_0; //ѡ��ͨ��A1
//    ADC10CTL0 |= ADC10ON; //����ADC10
    ADC10CTL0 &= ~ENC;    //ת������λ��ֹ
    ADC10CTL0  |= ADC10SHT_3 + ADC10ON+SREF_0;//VCC-VSS,64������
    ADC10CTL1  |= (INCH_0 + SHS_0 + CONSEQ_0 + ADC10SSEL_0) ;     //����ͨ��0��SHS=��������ԴΪADC10SC,CONSEQ_0=��ͨ������ת��ģʽ�� ADC10SSEL_0=ʱ��ѡ��ADC10OSC
}

void get_RT3_temp()
{
    UINT16 ADC10_temp = 0;
    UINT16 ADC10_max = 0;//���AD
    UINT16 ADC10_min = 0xffff;//��СAD
    char i;
    RT3_AD =0;
    ADC10CTL0  |= ENC + ADC10SC;
    _delay_cycles(100);
    for(i=0;i<6;i++)//ÿ���βɼ���ֵһ��
    {
       while(ADC10CTL1 & ADC10BUSY);//��֤ADC��ǰ�޽����л
       while(ADC10CTL0 & ADC10IFG==0);//��֤ADC�ж��ѽ���
       ADC10_temp = ADC10MEM;
       //ȡ�����ֵ
       if(ADC10_max < ADC10_temp)
       {
           ADC10_max = ADC10_temp;
       }

       //ȡ����Сֵ
       if(ADC10_min > ADC10_temp)
       {
           ADC10_min = ADC10_temp;
       }

       RT3_AD+=ADC10_temp; //��������ۼ�//+

       _delay_cycles(30);
       ADC10CTL0 &=~ ADC10IFG;//���жϣ�Ҳ��֪���ز���Ҫ��
    }
    ADC10CTL0 &= ~ENC;

    RT3_AD= RT3_AD - ADC10_min - ADC10_max;//ȥ�������Сֵ
    RT3_AD = RT3_AD >> 2;//���ģ�λ����λ��
}
