/*
 * led.c
 *
 *  Created on: 2019Äê7ÔÂ12ÈÕ
 *      Author: fenglong
 */


#include "System.h"
#include "led.h"

extern UINT8 SystemMode;
extern UINT8 SystemBootUpCount;
extern uchar RS485_MODE;
extern UINT16 SleepCount;
extern UINT16 BatteryCapacityRefreshCount;
//volatile uchar led_real_rate[3];
//volatile uchar led_set_rate[3];
volatile uchar led1_real_rate=0;
volatile uchar led2_real_rate=0;
volatile uchar led3_real_rate=0;

volatile uchar led1_set_rate=0;
volatile uchar led2_set_rate=0;
volatile uchar led3_set_rate=0;
union _LED_CTRL LED_CTRL;



void LEDForBootUpMode(void)
{
    if (SystemMode == SystemBootUp)
    {
        if(SystemBootUpCount ==0)
        {
        	LED_OFF();

            timer1_init();

        }
        switch (SystemBootUpCount)
        {
        	case 0:
        		LED_OFF();
        		break;

            case 0+2:
                LED_CTRL.Word = 0x0003;				//½¥½¥ÁÁ
                break;
            case 14+2:
                LED_CTRL.Word &= 0xFFFD;			//½¥½¥Ãð
                break;


            case 8+2:
                LED_CTRL.Word += 0x0030;			//½¥½¥ÁÁ
                break;
            case 22+2:
                LED_CTRL.Word &= 0xFFDF;			//½¥½¥Ãð
                break;

            case 16+2:
                LED_CTRL.Word += 0x0300;			//½¥½¥ÁÁ
                break;
            case 30+2:
                LED_CTRL.Word &= 0xFDFF;			//½¥½¥Ãð
                break;

            case 44+2:
            	LED_CTRL.Word = 0x0300;				//½¥½¥ÁÁ
            	break;
            case 44+8+2:
                LED_CTRL.Word += 0x0030;			//½¥½¥ÁÁ
                break;
            case 44+16+2:
                LED_CTRL.Word += 0x0003;			//½¥½¥ÁÁ
                break;

            case 60+14+8+2:
                LED_CTRL.Word &= 0xFDDD;			//½¥½¥Ãð
                break;

            case 60+14+8+6+2:
                LED_CTRL.Word =0;
                LED_OFF();
                close_timer1();
                break;

        }

        SystemBootUpCount = SystemBootUpCount + 1;

        if (SystemBootUpCount > 100)
        {
            RS485_MODE = 0 ;                                //fenglong 20190603
            Temp_DSG_Start_Check();                     //fenglong 20190709
            SystemMode = SystemDischargeMode;           //fenglong 20190709
            SleepCount=0;

//            if()
//            {
//
//            }
//            BatteryCapacityRefreshCount = TA0_10s; //TA0_21m - TA0_20m ;		//fenglong 20190714
        }
    }
}

void LED1_PWM( uchar LEDx)
{
    if( led1_real_rate < led1_set_rate )
    {
        P2OUT &= ~LEDx;
    }
    else
    {
        P2OUT |= LEDx;
    }
    led1_real_rate++;
    if(led1_real_rate>=100)
    {
        led1_real_rate=0;
    }
}

void LED2_PWM( uchar LEDx)
{
    if( led2_real_rate < led2_set_rate )
    {
        P2OUT &= ~LEDx;
    }
    else
    {
        P2OUT |= LEDx;
    }
    led2_real_rate++;
    if(led2_real_rate>=100)
    {
        led2_real_rate=0;
    }
}

void LED3_PWM( uchar LEDx)
{
    if( led3_real_rate < led3_set_rate )
    {
        P3OUT &= ~LEDx;
    }
    else
    {
        P3OUT |= LEDx;
    }
    led3_real_rate++;
    if(led3_real_rate>=100)
    {
        led3_real_rate=0;
    }
}


void LED_OFF(void)
{
    LED_1_Low;
    LED_2_Low;
    LED_3_Low;
    LED_4_Low;
    LED_5_Low;
    LED_6_Low;
    EN_LED_1_P21;
    EN_LED_2_P22;
    EN_LED_3_P33;
    EN_LED_4_P32;
    EN_LED_5_P24;
    EN_LED_6_P25;
}



